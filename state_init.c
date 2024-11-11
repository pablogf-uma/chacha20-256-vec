# include <stdio.h>
# include <string.h>
# include <stdint.h>

/*

 Sets the initial state of the state with the specified parameters.
 Parameters:
      - Constant: 16 bytes ASCII-encoded string “expand 32-byte k” in little endian.
            -  Even though Intel processors use little-endian format natively, the constant array could be stored in 
                    any byte order (big-endian or little-endian) depending on how the data was initially set up or received.
            - Manually construct the little-endian representation to ensure that regardless of how the constant data is stored 
                in memory, it will be correctly interpreted in little-endian format for the algorithm.
      - Key: 32 bytes.
      - Block Counter: 4 bytes.
      - Nonce: 12 bytes.

*/
void state_init(uint32_t state[16], const char *constant, const uint8_t key[32], uint32_t blockcount, const uint8_t nonce[12])
{
    // Load the initial state of 0s.
    for (int i = 0; i < 16; i++)
    {
        state[i] = 0;
    }

    // Separate the constant (in little endian) into 4 bytes (4 letters) and assign them to the first column of the state.
    // Left shift operation << moves the bits to the left by the specified number of positions.
    // Bitwise OR operation | combines the 4 bytes into a single 32-bit integer.
    for (int i = 0; i < 4; i++)
    {
        state[i] = ((uint32_t)constant[i * 4] & 0xff) |  // least significant byte first
                    ((uint32_t)constant[i * 4 + 1] & 0xff) << 8 |
                    ((uint32_t)constant[i * 4 + 2] & 0xff) << 16 |
                    ((uint32_t)constant[i * 4 + 3] & 0xff) << 24;  // most significant byte last
    }


    // Separate the key into 8 32-bit words (in little-endian format)
    for (int i = 0; i < 8; i++) 
    {
    state[4 + i] = ((uint32_t)key[i * 4] & 0xff) |
                    ((uint32_t)key[i * 4 + 1] & 0xff) << 8 |
                    ((uint32_t)key[i * 4 + 2] & 0xff) << 16 |
                    ((uint32_t)key[i * 4 + 3] & 0xff) << 24;
    }


    // Assign the blockcount to the first element of the last row of the state (in little-endian format)
    state[12] = blockcount;

    // Separate the nonce into 3 32-bit words (in little-endian format)
    for (int i = 0; i < 3; i++) {
        state[13 + i] = ((uint32_t)nonce[i * 4] & 0xff) |
                        ((uint32_t)nonce[i * 4 + 1] & 0xff) << 8 |
                        ((uint32_t)nonce[i * 4 + 2] & 0xff) << 16 |
                        ((uint32_t)nonce[i * 4 + 3] & 0xff) << 24;
    }
    

    /*
    
    // TESTING: Output the state matrix.
    printf("\nOriginal state matrix:\n");

    for (int a = 0; a < 4; a++)  // Loop over rows
    {
        for (int b = 0; b < 4; b++)  // Loop over columns
        {
            printf("%08x ", state[a * 4 + b]);  // Finding each element's index: row * 4 + column
        }
        printf("\n");
    }
    */
    
}


/*
// TESTING: Main function to test the state_init function.
int main()
{
    int state[16];
    char constant[16] = "expand 32-byte k";
    uint8_t key[32] = { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint32_t blockcount = 234521;
    uint8_t nonce[12] = { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00 };

    state_init(state, constant, key, blockcount, nonce);

    return 0;
}

*/

