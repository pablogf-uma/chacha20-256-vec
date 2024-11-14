# include <stdio.h>
# include <stdint.h>

// This function generates 4 vectors from the concatenation of the states' rows with the same index

void state_to_vectors_v256(uint32_t state1[16], uint32_t state2[16], uint32_t v0[4], uint32_t v1[4], uint32_t v2[4], uint32_t v3[4])
{
    // Assign each elements from the first state to their position within the first vector
    for (int i = 0; i < 4; i++)
    {
        v0[i] = state1[i];
    }

    // Assign each element from the second state to their position within the first vector
    for (int i = 0; i < 4; i++)
    {
        v0[i + 4] = state2[i];
    }

    // 2nd vector
    for (int i = 4; i < 8; i++)
    {
        v1[i - 4] = state1[i];
    }

    for (int i = 4; i < 8; i++)
    {
        v1[i] = state2[i];
    }

    // 3rd vector
    for (int i = 8; i < 12; i++)
    {
        v2[i - 8] = state1[i];
    }

    for (int i = 8; i < 12; i++)
    {
        v2[i - 4] = state2[i];
    }

    // 4th vector
    for (int i = 12; i < 16; i++)
    {
        v3[i - 12] = state1[i];
    }

    for (int i = 12; i < 16; i++)
    {
        v3[i - 8] = state2[i];
    }
}