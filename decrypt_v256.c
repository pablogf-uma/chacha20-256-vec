#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "chacha20_functions_v256.h"
#include <emmintrin.h>

void decrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *ciphertext)
{
    size_t ciphertext_len = strlen(ciphertext);
    // Calculate the number of full 128-byte blocks needed
    size_t n_blocks = (ciphertext_len + 127) / 128;


    for (size_t i = 0; i < n_blocks; i++) {
        uint8_t keystream[128];
        state_init(state1, constant, key, blockcount + i * 2, nonce);
        state_init(state2, constant, key, blockcount + i * 2 + 1, nonce);
        permute_state_v256(state1, state2, v0, v1, v2, v3, keystream);

        // Calculate bytes to process for the current block
        size_t bytes_this_block;
        if (i == n_blocks - 1) {
            bytes_this_block = ciphertext_len - i * 128;
        } else {
            bytes_this_block = 128;
        }


        // Process full 32-byte chunks
        size_t j;
        for (j = 0; j < bytes_this_block - (bytes_this_block % 32); j += 32) {
            __m256i ciphertext_v = _mm256_loadu_si256((__m256i *)&ciphertext[i * 128 + j]);
            __m256i keystream_v = _mm256_loadu_si256((__m256i *)&keystream[j]);
            __m256i plaintext_v = _mm256_xor_si256(ciphertext_v, keystream_v);
            _mm256_storeu_si256((__m256i*)&plaintext[i * 128 + j], plaintext_v);
        }

        // Handle remaining bytes byte-by-byte
        for (; j < bytes_this_block; j++) {
            plaintext[i * 128 + j] = ciphertext[i * 128 + j] ^ keystream[j];
        }
    }

    // Add null terminator
    plaintext[ciphertext_len] = '\0';

        /* TEST
    for (size_t i = 0; i < ciphertext_len; i++) {
        printf("%02x", (unsigned char)plaintext[i]);
        printf(" ");
    }
    printf("\n");
    */
}