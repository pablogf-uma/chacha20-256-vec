<h1 align="center">ChaCha20 256-Bit Vectorized Implementation</h1>

This is an optimized version of the ChaCha20 algorithm that uses SIMD (Single Instruction, Multiple Data) instructions to process multiple pieces of data simultaneously, instead of the sequential processing provided by the regular chacha20 algorithm.

It is called 256-bit vectorization because it requires four 256-bit vectors as input (each vector is the concatenation of the 1st row of two consecutive state matrices).


<h2>Contents:</h2>




<h2>Performance:</h2>

