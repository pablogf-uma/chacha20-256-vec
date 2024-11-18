#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "chacha20_functions_v256.h"
#include <emmintrin.h>

void encrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *output) {
    
    size_t plaintext_len = strlen(plaintext);
    size_t n_of_128_byte_blocks = plaintext_len / 128; // Because 256-bit vectorization on 2 states at a time
    size_t remaining_bytes = plaintext_len % 128;

    // For plaintext smaller than 128 bytes (2 states) 128-bit vectorization is performed
    if (n_of_128_byte_blocks < 1){
        size_t plaintext_len = strlen(plaintext);
        size_t number_of_blocks = plaintext_len / 64;
        size_t remaining_bytes = plaintext_len % 64;

        // Each block of ciphertext is 64 bytes long, each xored with a state containing a different counter value
        for (int i = 0; i < number_of_blocks; i++) {

            uint8_t keystream[64];
            state_init(state1, constant, key, blockcount + i, nonce);
            permute_state_v128(state1, v0, v1, v2, v3, keystream);

            // XOR the plaintext with the keystream
            for (int j = 0; j < 64; j += 16) {
                __m128i plaintext_v = _mm_loadu_si128((__m128i *)&plaintext[i * 64 + j]);
                __m128i keystream_v = _mm_loadu_si128((__m128i *)&keystream[j]);
                __m128i output_v = _mm_xor_si128(plaintext_v, keystream_v);
                _mm_storeu_si128((__m128i*)&output[i * 64 + j], output_v);
            }

            /* TEST:
            printf("\nBlock #%d\n", i + 1);
            for (size_t j = 0; j < 64; j++) {
                printf("%02x", (unsigned char)output[i * 64 + j]);
                printf(" ");
            }
            printf("\n");
            */
        }

        // If there are remaining bytes, encrypt them (with 128-bit vectorization too)
        if (remaining_bytes != 0) {

            uint8_t keystream[64];
            state_init(state1, constant, key, blockcount + number_of_blocks, nonce);
            permute_state_v128(state1, v0, v1, v2, v3, keystream);

            // Group the remaining block in chunks of 16 to vectorize what is possible
            size_t offset_bytes = number_of_blocks * 64;
            size_t n_blocks_of_16_byte = remaining_bytes / 16;
            size_t remaining_after_16_grouping = remaining_bytes % 16;

            for (size_t i = 0; i < n_blocks_of_16_byte; i++) {
                __m128i plaintext_v = _mm_loadu_si128((__m128i *)&plaintext[offset_bytes + i * 16]);
                __m128i keystream_v = _mm_loadu_si128((__m128i *)&keystream[i * 16]);
                __m128i output_v = _mm_xor_si128(plaintext_v, keystream_v);
                _mm_storeu_si128((__m128i*)&output[offset_bytes + i * 16], output_v);
            }

            if (remaining_after_16_grouping != 0) {
                uint8_t temp_plaintext[16] = {0};
                uint8_t temp_keystream[16] = {0};

                // Copy the remaining bytes after 16 bytes grouping to the buffer variables
                memcpy(temp_plaintext, &plaintext[offset_bytes + n_blocks_of_16_byte * 16], remaining_after_16_grouping);
                memcpy(temp_keystream, &keystream[n_blocks_of_16_byte * 16], remaining_after_16_grouping);

                // XOR the remaining bytes
                for (size_t i = 0; i < remaining_after_16_grouping; i++) {
                    output[offset_bytes + n_blocks_of_16_byte * 16 + i] = temp_plaintext[i] ^ temp_keystream[i];
                }
            }
        }
    }

    // For plaintext with at least 128 bytes, 
    else {
        // 1 iteration of this loop will processes 2 states.
        for (int i = 0; i < n_of_128_byte_blocks; i ++) {
        
            // Generate the keystream for the current block
            uint8_t keystream[128];
            state_init(state1, constant, key, blockcount + i * 2, nonce);
            state_init(state2, constant, key, blockcount + i * 2 + 1, nonce);
            permute_state_v256(state1, state2, v0, v1, v2, v3, keystream);

            // XOR the plaintext with the keystream (vectorized version)
            for (int j = 0; j < 128; j += 16) {
                __m256i plaintext_v = _mm256_loadu_si256((__m256i *)&plaintext[i * 128 + j]);
                __m256i keystream_v = _mm256_loadu_si256((__m256i *)&keystream[j]);
                __m256i output_v = _mm256_xor_si256(plaintext_v, keystream_v);
                _mm256_storeu_si256((__m256i*)&output[i * 128 + j], output_v);
            }

            /* TEST
            printf("\n 256-bit vectorized encrypted block:\n");
            for (size_t j = 0; j < 128; j++) {
                printf("%02x", (unsigned char)output[i * 128 + j]);
                printf(" ");
            }
            printf("\n");*/
        }

        // If there are remaining bytes, encrypt them (using 128-bit vectorization)
        if (remaining_bytes != 0) {

            // Group the remaining bytes in blocks of 16 to use 128-bit vectorization
            size_t offset_bytes = n_of_128_byte_blocks * 128;
            size_t n_of_64_byte_blocks = remaining_bytes / 64;
            size_t remaining_bytes_after_64_grouping = remaining_bytes % 64;

            // Each block of ciphertext is 64 bytes long, each xored with a state containing a different counter value
            for (int i = 0; i < n_of_64_byte_blocks; i++) {

                // Generate the keystream for the current block
                uint8_t keystream[64];
                state_init(state1, constant, key, blockcount + (n_of_128_byte_blocks * 2) + i, nonce);
                permute_state_v128(state1, v0, v1, v2, v3, keystream);

                // XOR the plaintext with the keystream (128-bit vectorized version, so group them in chunks of 16)
                for (int j = 0; j < 64; j += 16) {
                    __m128i plaintext_v = _mm_loadu_si128((__m128i *)&plaintext[offset_bytes + i * 64 + j]);
                    __m128i keystream_v = _mm_loadu_si128((__m128i *)&keystream[j]);
                    __m128i output_v = _mm_xor_si128(plaintext_v, keystream_v);
                    _mm_storeu_si128((__m128i*)&output[offset_bytes + i * 64 + j], output_v);
                }

                /* TEST
                printf("\nBlock #%d\n", i + 1);
                for (size_t j = 0; j < 64; j++) {
                    printf("%02x", (unsigned char)output[offset_bytes + i * 64 + j]);
                    printf(" ");
                }
                printf("\n");*/
            }

            if (remaining_bytes_after_64_grouping != 0) {

                uint8_t keystream[64];
                state_init(state1, constant, key, blockcount + (n_of_128_byte_blocks * 2) + n_of_64_byte_blocks, nonce);
                permute_state_v128(state1, v0, v1, v2, v3, keystream);

                size_t final_offset = offset_bytes + (n_of_64_byte_blocks * 64);
                
                uint8_t temp_plaintext[64] = {0};
                uint8_t temp_keystream[64] = {0};

                // Copy the remaining bytes after 64 bytes grouping to the buffer variables
                memcpy(temp_plaintext, &plaintext[final_offset], remaining_bytes_after_64_grouping);
                memcpy(temp_keystream, &keystream[0], remaining_bytes_after_64_grouping);

                // XOR the remaining bytes
                for (size_t i = 0; i < remaining_bytes_after_64_grouping; i++) {
                    output[final_offset + i] = temp_plaintext[i] ^ temp_keystream[i];
                }

                /* TEST
                printf("\nremaining bytes block:\n");
                for (size_t j = 0; j < remaining_bytes_after_64_grouping; j++) {
                    printf("%02x", (unsigned char)output[final_offset + j]);
                    printf(" ");
                }
                printf("\n");*/
            }
        }
    }

    // Add the null terminator to the output string
    output[plaintext_len] = '\0';
}