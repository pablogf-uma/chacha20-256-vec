# include <stdio.h>
# include <stdint.h>
# include <immintrin.h>

// This function separates the 4 rows into 4 vectors before they are passed through the whole_round function.

void vectors_to_state(uint32_t state[16], __m128i v0, __m128i v1, __m128i v2, __m128i v3)
{
    // Extract the elements from each __m128i vector and assign them to the state array.
    _mm_storeu_si128((__m128i*)&state[0], v0);
    _mm_storeu_si128((__m128i*)&state[4], v1);
    _mm_storeu_si128((__m128i*)&state[8], v2);
    _mm_storeu_si128((__m128i*)&state[12], v3);
}