#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "chacha20_functions_v256.h"
#include <x86intrin.h>

// Function to run a single test and measure clock cyles per byte for each test
int run_encrypt_test(test_vector_t *test) {
    
    uint32_t state1[16];
    uint32_t state2[16];
    unsigned long plaintext_length = test->plaintext_length;
    char ciphertext[plaintext_length];
    
    uint32_t v0[8];
    uint32_t v1[8];
    uint32_t v2[8];
    uint32_t v3[8];

    // Start cycle counting.
    unsigned long long start_cycles = __rdtsc();
    
    encrypt_v256(state1, state2, "expand 32-byte k", test->key, test->blockcount, test->nonce, v0, v1, v2, v3, test->plaintext, ciphertext, plaintext_length);
    
    // End cycle counting.
    unsigned long long end_cycles = __rdtsc();

    // Calculate cycles per byte
    unsigned long long cycles = end_cycles - start_cycles;   
    double cycles_per_byte = (double)cycles / plaintext_length;
    printf("Total input bytes: %lu", plaintext_length);
    printf("\nClock cycles per byte: %.2f\n", cycles_per_byte);

    // Compare ciphertext with expected ciphertext.
    if (memcmp(ciphertext, test->expected_ciphertext, plaintext_length) == 0) {
        
        return 1; // Test passed.
    } else {
        return 0; // Test failed.
    }
}