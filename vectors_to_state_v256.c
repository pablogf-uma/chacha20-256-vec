# include <stdio.h>
# include <stdint.h>
# include <immintrin.h>

// This function separates each vector into a lower (beginning) and higher (end) part,
// Then assigns each part to their correct position within each corresponding state

void vectors_to_state_v256(uint32_t state1[16], uint32_t state2[2], __m256i v0, __m256i v1, __m256i v2, __m256i v3)
{
    // Break down each vector in half
    __m128i v0_low = _mm256_extracti128_si256(v0, 0);
    __m128i v0_high = _mm256_extracti128_si256(v0, 1);
    __m128i v1_low = _mm256_extracti128_si256(v1, 0);
    __m128i v1_high = _mm256_extracti128_si256(v1, 1);
    __m128i v2_low = _mm256_extracti128_si256(v2, 0);
    __m128i v2_high = _mm256_extracti128_si256(v2, 1);
    __m128i v3_low = _mm256_extracti128_si256(v3, 0);
    __m128i v3_high = _mm256_extracti128_si256(v3, 1);

    // Assign the lower elements (beginning to half of the vector) to state1
    _mm_storeu_si128((__m128i*)&state1[0], v0_low);
    _mm_storeu_si128((__m128i*)&state1[4], v1_low);
    _mm_storeu_si128((__m128i*)&state1[8], v2_low);
    _mm_storeu_si128((__m128i*)&state1[12], v3_low);

    // Assign the higher elements (half to end of the vector) to state2
    _mm_storeu_si128((__m128i*)&state2[0], v0_high);
    _mm_storeu_si128((__m128i*)&state2[4], v1_high);
    _mm_storeu_si128((__m128i*)&state2[8], v2_high);
    _mm_storeu_si128((__m128i*)&state2[12], v3_high);
}