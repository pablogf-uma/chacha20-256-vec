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

    // Load permuted vectors back into the state
    vectors_to_state_v256(state1, state2, v0_permuted, v1_permuted, v2_permuted, v3_permuted);

    // Serialize the permuted state1 into the output keystream
    for (size_t i = 0; i < 16; i += 8) {
        __m256i vec1 = _mm256_loadu_si256((__m256i*)&state1[i]);
        uint8_t temp[32];
        _mm256_storeu_si256((__m256i*)temp, vec1);

        for (int j = 0; j < 32; j++) {
            keystream[i * 4 + j] = temp[j];
        }
    }

    // Serialize permuted state2
    for (size_t i = 0; i < 16; i += 8) {
        __m256i vec2 = _mm256_loadu_si256((__m256i*)&state2[i]);
        uint8_t temp[32];
        _mm256_storeu_si256((__m256i*)temp, vec2);

        for (int j = 0; j < 32; j++) {
            keystream[i * 4 + j + 64] = temp[j];
        }
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
