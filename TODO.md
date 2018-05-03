Here is a mostly unordered list of stuff that should be done with
CTTK:

  - Big integers: division with unsigned interpretation.
  - Big integers: multiplication optimisation with Karatsuba.
  - Big integers: division optimisation (word-wise processing).
  - Big integers: conversions to and from strings (binary, decimal,
    hexadecimal).
  - Big integers: extra implementation with 15-bit words (i15).
  - Big integers: extra implementation with 63-bit words (i63).
  - Modular integers (with odd modulus, possibly prime).
  - SIMD optimisations (SSE2, AVX2...).
  - Oblivious RAM with sub-linear cost.
  - Storage and search structures (maps).
  - Automatic bitslicing metaprogramming tool.

The following features have been implemented:

  - Boolean type and operations.
  - Comparisons of 32-bit and 64-bit integers.
  - Constant-time conditional copy and swap of buffers.
  - Constant-time buffer comparisons (equality, lexicographic order).
  - Constant-time array lookup (with O(N) cost).
  - Hexadecimal encoder / decoder.
  - Base64 encoder / decoder.
  - Big integers: base definitions and conversions to/from native
    integers.
  - Big integers: extending and narrowing conversions.
  - Big integers: addition and subtraction.
  - Big integers: multiplication.
  - Big integers: Euclidean division.
  - Big integers: left and right shifts.
  - Big integers: boolean bitwise operations.
