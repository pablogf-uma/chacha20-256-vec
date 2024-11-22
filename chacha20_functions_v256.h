# include <stdio.h>
# include <string.h>
# include <stdint.h>
# include <immintrin.h>

void state_init(uint32_t state[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12]);

void state_to_vectors_v256(uint32_t state[16], uint32_t state2[16], uint32_t v0[4], uint32_t v1[4], uint32_t v2[4], uint32_t v3[4]);

void double_whole_round_v256(uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3);

void permute_state_v256(uint32_t state1[16], uint32_t state2[16], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, uint8_t keystream[64]);

void vectors_to_state_v256(uint32_t state1[16], uint32_t state2[2], __m256i v0, __m256i v1, __m256i v2, __m256i v3);

void encrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *ciphertext);

typedef struct {uint8_t key[32]; uint8_t nonce[12]; uint32_t blockcount; char plaintext[100000]; char expected_ciphertext[100000];} test_vector_t;

int run_test(test_vector_t *test);

void calculate_throughput(test_vector_t *test);

void calculate_throughput_2();

void decrypt_v256(uint32_t state1[16], uint32_t state2[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12], uint32_t *v0, uint32_t *v1, uint32_t *v2, uint32_t *v3, char *plaintext, char *ciphertext);

int run_decrypt_test(test_vector_t *test);