# include <stdio.h>
# include <stdint.h>
# include <string.h>
#include "chacha20_functions_v256.h"
# include <immintrin.h>

void permute_state_v256(uint32_t state1[16], uint32_t state2[16], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, uint8_t keystream[64])
{
    // Initialize vectors
    state_to_vectors_v256(state1, state2, v0, v1, v2, v3);

    // Use of vectors for vectorized addition after the rounds
    __m256i v0_og = _mm256_loadu_si256((__m256i*)v0);
    __m256i v1_og = _mm256_loadu_si256((__m256i*)v1);
    __m256i v2_og = _mm256_loadu_si256((__m256i*)v2);
    __m256i v3_og = _mm256_loadu_si256((__m256i*)v3);

    // Perform permutations on the state: 20 total rounds (10 column-diagonal operations)
    for (int i = 0; i < 10; i++) {
        double_whole_round_v256(v0, v1, v2, v3);
    }

    __m256i v0_permuted = _mm256_loadu_si256((__m256i*)v0);
    __m256i v1_permuted = _mm256_loadu_si256((__m256i*)v1);
    __m256i v2_permuted = _mm256_loadu_si256((__m256i*)v2);
    __m256i v3_permuted = _mm256_loadu_si256((__m256i*)v3);

    // Add the permuted vectors to the original vectors for pseudo-randomness:
    v0_permuted = _mm256_add_epi32(v0_permuted, v0_og);
    v1_permuted = _mm256_add_epi32(v1_permuted, v1_og);
    v2_permuted = _mm256_add_epi32(v2_permuted, v2_og);
    v3_permuted = _mm256_add_epi32(v3_permuted, v3_og);

    // Load permuted vectors back into the state
    vectors_to_state_v256(state1, state2, v0_permuted, v1_permuted, v2_permuted, v3_permuted);

    // Serialize the permuted state1 into the output keystream
    for (size_t i = 0; i < 16; i++) {
        uint32_t word = state1[i];
        keystream[i * 4] = (word >> 0)  & 0xFF;
        keystream[i * 4 + 1] = (word >> 8)  & 0xFF;
        keystream[i * 4 + 2] = (word >> 16) & 0xFF;
        keystream[i * 4 + 3] = (word >> 24) & 0xFF;

        // Inline assembly statement that acts as memory barrier
        // This prevents the compiler from reordering the writes to output_keystream, 
        //      ensuring that the loop performs as expected even under -O3 optimizer.
        __asm__ __volatile__("" ::: "memory"); 
    }

    // Serialize permuted state2
    for (size_t i = 0; i < 16; i++) {
        uint32_t word = state2[i];
        keystream[i * 4 + 16] = (word >> 0)  & 0xFF; // Add 16 to jump over the already created elements of the keystream
        keystream[i * 4 + 1 + 16] = (word >> 8)  & 0xFF;
        keystream[i * 4 + 2 + 16] = (word >> 16) & 0xFF;
        keystream[i * 4 + 3 + 16] = (word >> 24) & 0xFF;

        __asm__ __volatile__("" ::: "memory"); 
    }

    /*
    // TESTING: Output the permuted vectors, state, and keystream

    uint32_t *vectors[4] = {v0, v1, v2, v3};
    for (int i = 0; i < 4; i++)
    {
        printf("Vector %i:\n", i + 1);
        for (int b = 0; b < 4; b++)
        {
            printf("%08x", vectors[i][b]);
        }
        printf("\n\n");
    }



    __m256i vectors2[4] = {v0_permuted, v1_permuted, v2_permuted, v3_permuted};

    for (int i = 0; i < 4; i++)
    {
        printf("Vector %i:\n", i + 1);
        for (int b = 0; b < 4; b++)
        {
            uint32_t value;
            switch (b) {
                case 0: value = _mm256_extract_epi32(vectors2[i], 0); break;
                case 1: value = _mm256_extract_epi32(vectors2[i], 1); break;
                case 2: value = _mm256_extract_epi32(vectors2[i], 2); break;
                case 3: value = _mm256_extract_epi32(vectors2[i], 3); break;
            }
            printf("%08x", value);
        }
        printf("\n\n");
    }



    
    for (int i = 0; i < 64; i++) {
        printf("%02x", keystream[i]);
        printf(":");
    }
    printf("\b \b");
    printf("\n");
 */
}