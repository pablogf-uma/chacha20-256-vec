#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chacha20_functions_v256.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" // To ignore the fgets warning for not usign its output

void decrypt_custom_input(char *plaintext)
{
    uint32_t state1[16];
    uint32_t state2[16];
    uint32_t v0[8];
    uint32_t v1[8];
    uint32_t v2[8];
    uint32_t v3[8];
    uint8_t key[32];
    char key_char[33]; // To accomodate the null terminator
    uint8_t nonce[12];
    char nonce_char[13];
    char input[256];
    uint8_t translated_input[256] = {0}; // To hold the input once it is transformed from string to bytes

    printf("\nInsert 256-bit Key (32 characters):\n");
    // Read key as a string
    fgets(key_char, sizeof(key_char), stdin);
    int key_char_len = strlen(key_char);
    // Convert each character of the string to hex and load it to the key array
    for (size_t i = 0; i < key_char_len; i++)
    {   
        key[i] = (uint8_t)key_char[i];
    }

    // Clear the input buffer to erase the newline character
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // Same operation for the nonce
    printf("\nInsert 96-bit Nonce (12 characters):\n");
    fgets(nonce_char, sizeof(nonce_char), stdin);
    int nonce_char_len = strlen(nonce_char);

    for (size_t i = 0; i < nonce_char_len; i++)
    {   
        nonce[i] = (uint8_t)nonce_char[i];
    }

    int d;
    while ((d = getchar()) != '\n' && d != EOF);

    printf("\nInsert Ciphertext (in hex: 8B1C...):\n");
    fgets(input, sizeof(input), stdin);
    int ciphertext_len = strlen(input) - 1; // -1 to eliminate the null terminator from the input string

    size_t translated_len = ciphertext_len / 2; // To group the characters in hex pairs
    // Group hex pairs together, convert them to hex, and add them to the array
    for (size_t i = 0; i < translated_len; i++) {
        char hex_pair[3] = { input[2 * i], input[2 * i + 1], '\0' };
        translated_input[i] = (uint8_t)strtol(hex_pair, NULL, 16);
    }

    /* TEST
    for (size_t i = 0; i < translated_len; i++) {
        printf("%02X ", translated_input[i]);
    }
    printf("\n");*/

    decrypt_v256(state1, state2, "expand 32-byte k", key, 0, nonce, v0, v1, v2, v3, plaintext, (char *)translated_input, translated_len);

    printf("\nPlaintext:\n");
    printf("%s", plaintext);
    printf("\n");
}

#pragma GCC diagnostic pop