<h1 align="center">ChaCha20 256-Bit Vectorized Implementation</h1>

This is an optimized version of the ChaCha20 algorithm that uses SIMD (Single Instruction, Multiple Data) instructions to process multiple pieces of data simultaneously, instead of the sequential processing provided by the regular chacha20 algorithm.

This version utilizes Intel Intrinsics through the header immintrin.h.

It is called 256-bit vectorization because it requires four 256-bit vectors as input (each vector is the concatenation of the 1st row of two consecutive state matrices).

Like the 128-bit vectorization, this vectorization performs 4 consecutive operations of the non-vectorized version of chacha20 into 1 single vectorized operation:

- This operation, which I called a "Double whole round" can now be reduced to **just two vectorized operations** (one for the row round and one for the column round), because each vector operation handles **two quarter-rounds at once**.
- It will concatenate the columns and diagonal permutation rounds one after the other.
- Double (columns + diagonals) Whole (4 parallel quarter rounds) Round.
- However, the vectors are now longer, since they hold the rows of two consecutive states.

<h2>Resources:</h2>

These resources were used throughout the development of the project outlined above.

- [Paper](https://eprint.iacr.org/2013/759.pdf): Original research paper on ChaCha20 by Daniel J. Bernstein.
- [RFC 8439](https://www.rfc-editor.org/rfc/rfc8439#section-2.1.1): Standardized specification of ChaCha20 and its use in internet protocols.
- [SIMD Programming Blog entry](https://dennisrants.substack.com/p/how-to-simd-programming): Deep explanation of programming with SIMD instructions


---

## Tests

The compiled executable, `chacha20`, supports the following options for testing and usage:

- Run all available tests: Test vectors (decryption and encryption), and clock cycle tests
```c
./chacha20 --all-tests
````
- Encrypt test vector N (where N is from 1 to 5 for each test vector within the [RFC 8439](https://www.rfc-editor.org/rfc/rfc8439#section-2.1.1)).
```c
./chacha20 --enc-tv N
```
- Decrypt test vector N (where N is from 1 to 5).
```c
./chacha20 --dec-tv N
```
- Clock cycles test N (where N is from 1 to 5 for each plaintext length per [ECRYPT](https://bench.cr.yp.to/results-stream/amd64-panther.html)).
```c
./chacha20 --clock-ct N
```
Encrypt a custom input provided by the user.
```c
./chacha20 --enc-ci
```
- Decrypt a custom input provided by the user.
```c
./chacha20 --dec-ci
```

---

## Performance

This implementation of ChaCha20 has been optimized for performance and includes benchmarking scripts to measure its efficiency. The results are comparable to established benchmarks, such as those available at [ECRYPT](https://bench.cr.yp.to/results-stream/amd64-panther.html).
To measure the **cycles per byte**, use the provided automation scripts:

#### Windows
Run the `clock_cycles_tests.bat` script:
```bash
clock_cycles_tests.bat
```

### Linux
Run the `clock_cycles_tests.sh` script:
```bash
clock_cycles_tests.sh
```
These scripts perform automated benchmarking of the ChaCha20 implementation, similar to how benchmarks are conducted for cryptographic algorithms at [ECRYPT](https://bench.cr.yp.to/results-stream/amd64-panther.html). The average and median results in cycles per byte are as follows:

| Plaintext Length | Average | Median |
|------------------|---------|--------|
| 8-bytes         | 233.12   | 213.50 |
| 64-bytes        | 19.96    | 18.14  |
| 576-bytes       | 7.44     | 7.03   |
| 1536-bytes      | 5.82     | 5.77   |
| 4096-bytes      | 5.52     | 5.44   |
