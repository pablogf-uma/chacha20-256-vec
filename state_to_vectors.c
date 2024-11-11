# include <stdio.h>
# include <stdint.h>

// This function separates the 4 rows into 4 vectors before they are passed through the whole_round function.

void state_to_vectors(uint32_t state[16], uint32_t v0[4], uint32_t v1[4], uint32_t v2[4], uint32_t v3[4])
{
    // Assign each row to their corresponding position within each vector.
    for (int i = 0; i < 4; i++)
    {
        v0[i] = state[i];
    }

    for (int i = 4; i < 8; i++)
    {
        v1[i - 4] = state[i];
    }

    for (int i = 8; i < 12; i++)
    {
        v2[i - 8] = state[i];
    }

    for (int i = 12; i < 16; i++)
    {
        v3[i - 12] = state[i];
    }
}