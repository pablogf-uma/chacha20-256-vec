# include <stdio.h>
# include <stdint.h>
# include <string.h>
#include "chacha20_functions_v256.h"
# include <immintrin.h>

void permute_state_v256(uint32_t state1[16], uint32_t state2[16], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, uint8_t keystream[128])
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

    // Serialize the vectors into the keystream using 128-bit vectorization
    __m256i permuted_vectors[4] = {v0_permuted, v1_permuted, v2_permuted, v3_permuted};

    for (size_t i = 0; i < 4; i++) { 
        __m128i state1_half = _mm256_extracti128_si256(permuted_vectors[i], 0); // Extract the lower 128 bits (state 1's row)
        __m128i state2_half = _mm256_extracti128_si256(permuted_vectors[i], 1); // Extract the higher 128 bits (state 2's row)

        _mm_storeu_si128((__m128i*)&keystream[i * 16], state1_half); // Store vectors into the keystream 128-bits at a time (1st state)
        _mm_storeu_si128((__m128i*)&keystream[i * 16 + 64], state2_half); // 2nd state
    }

    /*
    // TESTING: Output the permuted vectors, state, and keystream

    __m256i vectors2[4] = {v0_permuted, v1_permuted, v2_permuted, v3_permuted};

    for (int i = 0; i < 4; i++)
    {
        printf("Vector %i:\n", i + 1);
        for (int b = 0; b < 8; b++)
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


     for (int a = 0; a < 4; a++)  // Loop over rows
    {
        for (int b = 0; b < 4; b++)  // Loop over columns
        {
            printf("%08x ", state1[a * 4 + b]);  // Finding each element's index: row * 4 + column
        }
        printf("\n");
    }

    printf("\n");

     for (int a = 0; a < 4; a++)  // Loop over rows
    {
        for (int b = 0; b < 4; b++)  // Loop over columns
        {
            printf("%08x ", state2[a * 4 + b]);  // Finding each element's index: row * 4 + column
        }
        printf("\n");
    }

    printf("\n");
    
    for (int i = 0; i < 128; i++) {
        printf("%02x", keystream[i]);
        printf(":");
    }
    printf("\b \b");
    printf("\n");*/
}
