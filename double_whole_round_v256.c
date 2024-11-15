# include <stdio.h>
# include <stdint.h>
# include "chacha20_functions_v256.h"
# include <immintrin.h>

// 256-BIT VECTORIZATION: SIMD (immintrin.h) for parallel processing contains:

/*

ROTATE_BITS:

- Performs n-bit rotations for the elements inside vector v.
    - Combining a left shift and a right shift this way mimics a circular or left rotation.

- It will be used within the whole_round function.

- Contents from immintrin.h:
    __256i : Data type provided by immintrin.h that to represent a  256-bit vector.
    _mm256_slli_epi32(x, n): This function performs a left bitwise shift on each 32-bit element in the vector x by n bits.
    _mm256_srli_epi32(x, 32 - n): This function performs a right bitwise shift on each 32-bit element in x by 32 - n bits.
    _mm256_or_si256(...): This performs a bitwise OR between the two shifted results.

*/

__m256i rotate_left_v256(__m256i v, int n) {
    return _mm256_or_si256(_mm256_slli_epi32(v, n), _mm256_srli_epi32(v, 32 - n));
}

/*

WHOLE_ROUND:

- Does the job of 4 consecutive operations in the previous chacha into 1

- It will concatenate the columns and diagonal permutation rounds one after each other
     - Double (columns + diagonals) Whole (4 parallel quarter rounds) Round

- Parameters: 4 vectors (one for each row of the 2 states: 8 words (4 for state 1 and 4 for state2) * 4 bytes * 8 bits = 256 bits)
      - The bit shifting in between operation sets rearranges the vectors so they contain the diagonals

- Contents from immintrin.h:
    _mm_loadu_si128((__m256i*)vector) : Loads 256 bits (8 x 32-bit integers) from an unaligned memory location into the SIMD register
    _mm256_add_epi32(v1, v2) : Adds each element of v1 with the corresponding element of v2
    _mm256_xor_si256(v1, v2) : XOR's each element of v1 with the corresponding element of v2
    _mm256_shuffle_epi32(v1_vec, _MM_SHUFFLE(a,b,c,d)) : Shuffles the elements in a vector based on the mask provided
    _MM_SHUFFLE(d, c, b, a) specifies the new order for elements. The element at position 0 is moved to position a, element at position 2 is moved to position b...
    _mm256_storeu_si256((__m256i*)v1, v1_vec): Stores the values of the register back to the original arrays

*/

void double_whole_round_v256(uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3) 
{
    // Load vectors into SIMD register
    __m256i v0_vec = _mm256_loadu_si256((__m256i*)v0);
    __m256i v1_vec = _mm256_loadu_si256((__m256i*)v1);
    __m256i v2_vec = _mm256_loadu_si256((__m256i*)v2);
    __m256i v3_vec = _mm256_loadu_si256((__m256i*)v3);

    // First set of operations (columns):
    v0_vec = _mm256_add_epi32(v0_vec, v1_vec);
    v3_vec = _mm256_xor_si256(v3_vec, v0_vec);
    v3_vec = rotate_left_v256(v3_vec, 16);

    v2_vec = _mm256_add_epi32(v2_vec, v3_vec);
    v1_vec = _mm256_xor_si256(v1_vec, v2_vec);
    v1_vec = rotate_left_v256(v1_vec, 12);

    v0_vec = _mm256_add_epi32(v0_vec, v1_vec);
    v3_vec = _mm256_xor_si256(v3_vec, v0_vec);
    v3_vec = rotate_left_v256(v3_vec, 8);

    v2_vec = _mm256_add_epi32(v2_vec, v3_vec);
    v1_vec = _mm256_xor_si256(v1_vec, v2_vec);
    v1_vec = rotate_left_v256(v1_vec, 7);

    // Reorder elements in each vector to operate on diagonals:
    v1_vec = _mm256_shuffle_epi32(v1_vec, _MM_SHUFFLE(0,3,2,1));
    v2_vec = _mm256_shuffle_epi32(v2_vec, _MM_SHUFFLE(1,0,3,2));
    v3_vec = _mm256_shuffle_epi32(v3_vec, _MM_SHUFFLE(2,1,0,3));

    // Second set of operations (diagonals):
    v0_vec = _mm256_add_epi32(v0_vec, v1_vec);
    v3_vec = _mm256_xor_si256(v3_vec, v0_vec);
    v3_vec = rotate_left_v256(v3_vec, 16);

    v2_vec = _mm256_add_epi32(v2_vec, v3_vec);
    v1_vec = _mm256_xor_si256(v1_vec, v2_vec);
    v1_vec = rotate_left_v256(v1_vec, 12);

    v0_vec = _mm256_add_epi32(v0_vec, v1_vec);
    v3_vec = _mm256_xor_si256(v3_vec, v0_vec);
    v3_vec = rotate_left_v256(v3_vec, 8);

    v2_vec = _mm256_add_epi32(v2_vec, v3_vec);
    v1_vec = _mm256_xor_si256(v1_vec, v2_vec);
    v1_vec = rotate_left_v256(v1_vec, 7);
  
    // Restore original order:
    v1_vec = _mm256_shuffle_epi32(v1_vec, _MM_SHUFFLE(2,1,0,3));
    v2_vec = _mm256_shuffle_epi32(v2_vec, _MM_SHUFFLE(1,0,3,2));
    v3_vec = _mm256_shuffle_epi32(v3_vec, _MM_SHUFFLE(0,3,2,1));

    // Store result back in the input parameters:
    _mm256_storeu_si256((__m256i*)v0, v0_vec);
    _mm256_storeu_si256((__m256i*)v1, v1_vec);
    _mm256_storeu_si256((__m256i*)v2, v2_vec);
    _mm256_storeu_si256((__m256i*)v3, v3_vec);

}

/* TEST MAIN FUNCTION
// Function declarations
void print_vector_v256(__m256i vec);
void double_whole_round_v256(uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3);
__m256i rotate_left_v256(__m256i v, int n);

// Function to print vectors
void print_vector_v256(__m256i vec) {
    uint32_t* v = (uint32_t*)&vec;
    printf("%08x %08x %08x %08x %08x %08x %08x %08x\n", v[0], v[1], v[2], v[3]);
}

int main() {
    uint32_t v0[8] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    uint32_t v1[8] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};
    uint32_t v2[8] = {0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};
    uint32_t v3[8] = {0x00000001, 0x00000000, 0x4a000000, 0x00000000, 0x00000002, 0x00000000, 0x4a000000, 0x00000000};

    for (int i = 0; i < 10; i++) {
        double_whole_round_v256(v0, v1, v2, v3);
    }

    printf("Vector 1: ");
    print_vector_v256(_mm256_loadu_si256((__m256i*)v0));
    printf("Vector 2: ");
    print_vector_v256(_mm256_loadu_si256((__m256i*)v1));
    printf("Vector 3: ");
    print_vector_v256(_mm256_loadu_si256((__m256i*)v2));
    printf("Vector 4: ");
    print_vector_v256(_mm256_loadu_si256((__m256i*)v3));

    return 0;
}*/