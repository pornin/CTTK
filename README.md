# Constant-Time Toolkit

_Constant-time code_ is a concept that has been developed in the area of
cryptography since about 2005. It relates to the fact that
implementations of cryptographic algorithms manipulate secret values, and
if they do not do so with enough care, outsiders may obtain some
information on such values through time-based side channels, e.g. by
measuring the total time taken to perform a computation. Memory caches,
as commonly used in modern computers, are a great source of such leaks.
We thus call "constant-time" implementations that take care not to
allow time-based side channels.

The concept of side channels is not restricted to cryptography. In fact,
any piece of code that processes confidential data, in a context where
attackers may make precise timing measures, is potentially vulnerable,
and should use mitigation measures. In particular, implementations of
security enclaves with [Intel SGX](https://software.intel.com/en-us/sgx)
or [ARM TrustZone](https://developer.arm.com/technologies/trustzone)
operate in a security model where side channel attacks are very
effective, since the attacker is supposed to run his own code on the
host system and can monitor cache accesses with great accuracy.
Constant-time coding techniques are highly relevant to about anything
that is implemented in an enclave.

This library, called CTTK, is **a collection of constant-time
implementations of primitive operations that may help in writing
constant-time code, including non-cryptographic constant-time code.**

It shall be noted that the expression "constant-time" is traditional but
slightly confusing: constant-time code does not always execute in a
constant amount of time; rather, this means that any variation in
execution time is uncorrelated with secret information. For more
information on constant-time coding, you may have a look at the
[Cryptography Coding
Standard](https://cryptocoding.net/index.php/Cryptography_Coding_Standard)
pages, and the [BearSSL
library](https://www.bearssl.org/constanttime.html).

# License

This library uses the MIT license. In plain words, this means that you
can reuse it in both opensource and proprietary projects; the only
requirement is that you keep the license text with each source file,
where it already is. This is for my protection: the license text
basically says that whatever happens, it's not my fault, and you
understand it.

This library is written and maintained by Thomas Pornin
`<pornin@bolet.org>`. Any comments and suggestions are welcome. Be
warned that if you submit a patch or pull request, and I find it good,
then I will still rewrite it completely, because I am a raving maniac.

# Status

CTTK is still very early in its development. It probably contains some
bugs. The API _may_ change in future versions. I will do my best not to
gratuitously break source or binary compatibility, but I cannot
guarantee that it won't happen.

**WARNING:** IF YOU ARE TRYING TO USE THIS LIBRARY, IN ITS CURRENT STATE,
FOR PRODUCTION CODE, THEN YOU ARE MOST CERTAINLY CRAZY. Of course, a
solid dose of craziness is often needed to promote innovation; but it
rarely ends well for the zealous early adopter.

A list of planned features and improvement can be seen in the
[TODO.md](TODO.md) file.

# Compilation

To compile, type `make`. This should work on any decent Linux or \*BSD
system, both with GNU make and BSD make. If you use Microsoft Visual C
command-line tools on Windows, you may type `nmake`, and it should work
too. You can tune compile-time options in the relevant file in the
`conf` directory; these can also be added on the compilation command-line,
e.g.:

    make BUILD=alt

will create a directory called `alt` and put the compilation output in
that directory instead of the default directory `build`.

There are some tunable configuration options in `src/config.h`. Such
options may be set either in that file, or through command-line options
for compilation (in the `CFLAGS` variable).

Compilation produces a static library (`libcttk.a` on Unix-like systems,
`cttks.lib` on Windows), a dynamic library (`libcttk.so` on Unix-like
system, `cttk.dll` on Windows), and a test executable (`testcttk`), all
in the build directory, which is created when needed (its default name
is `build`). The test executable can be run to perform some basic
self-tests.

There is no automated installation process yet; notably, in the context
of security enclaves such as SGX, a specific installation process would
be needed anyway. The external API is the `cttk.h` file located in the
`inc` directory.

If using the provided makefiles is inconvenient, the files may be
integrated in any other build system. The dependencies are simple:

  - Every `.c` file in `src` should be compiled into an object file.
  - Each such `.c` file includes `inner.h`, `config.h`, and `cttk.h`.
  - There are no other dependencies.

# API

CTTK is a C library. As a rule, C is a tricky language, full of
pitfalls, in particular undefined behaviours that may break any
implementation silently when changing the compiler version. More modern,
safer languages such as [Rust](https://www.rust-lang.org/en-US/) or
[Go](https://golang.org/) are almost always preferable; and even for
very low-level, bare metal processing, it makes sense to explore
alternatives such as [Forth](http://wiki.c2.com/?ForthLanguage).

On the other hand, C is still the _lingua franca_ of programming
languages, and a C compiler can be found for just about any hardware and
software environment. Moreover, in an ideal world, languages that are
under active development should integrate constant-time primitives as
part of the standard language definition; a third-party library like
CTTK makes sense only because C is a mostly frozen language.

CTTK is supposed to be usable from C++ as well, but with its C-like API.
There is no support for operator overloading or templates.

The API is mostly documented in the `cttk.h` file itself. An HTML
version of that documentation can be produced with
[Doxygen](http://www.doxygen.org); a configuration file (`Doxyfile`)
is provided.

## Namespaces

C has no namespace. Therefore, all external names provided by CTTK start
with a specific prefix to help with avoiding name collisions.

All objects with external linkage (functions, global variables) have a
name that starts with `cttk_`.

The header file (`cttk.h`) also defines function and macro names that
start with `cttk_`, `CTTK_` or `cti_`. These names only impact the
current translation unit, i.e. the files that include directly or
indirectly `cttk.h`.

## Header

To use CTTK in your application, include the `cttk.h` header. That file
pulls in a few standard library files that should be available on all C
implementations, including "freestanding" compilers for embedded
systems.

## Booleans

CTTK defines the `cttk_bool` type to contain a boolean value (true or
false). This type is defined as a `struct` so that it is NOT directly
usable to control conditional jumps, since these are, by definition,
not constant-time.

The `cttk_true` and `cttk_false` constants are provided for,
respectively, true and false values.

The `cttk_bool_of_u32()`, `cttk_bool_of_s32()` and `cttk_bool_to_int()`
functions can be used to convert between C "booleans" (i.e. integer
values 0 and 1) and CTTK booleans. Constant-time code should endeavour
to apply such conversions only when the boolean value is no longer
considered secret.

Boolean operations are implemented by `cttk_not()`, `cttk_and()`,
`cttk_or()`, `cttk_xor()` and `cttk_eqv()` (this last one is also
known as the "XORNOT" operation).

## Native Integers

Primitives for doing constant-time comparisons on native integers are
provided. In all names, `s32`, `u32`, `s64` and `u64` designate the C
types `int32_t`, `uint32_t`, `int64_t` and `uint64_t`, respectively.
Variants for all applicable types are provided, for the following
operations:

  - Multiplexer (selection of one of two operands, based on a `cttk_bool`
    value): `cttk_s32_mux`, `cttk_u32_mux`, `cttk_s64_mux`, `cttk_u64_mux`

  - Comparison with zero (returns true if the operand is not zero):
    `cttk_s32_neq0`, `cttk_u32_neq0`, `cttk_s64_neq0`, `cttk_u64_neq0`

  - Comparison with zero (returns true if the operand is zero):
    `cttk_s32_eq0`, `cttk_u32_eq0`, `cttk_s64_eq0`, `cttk_u64_eq0`

  - Equality comparison between two integers:
    `cttk_s32_eq`, `cttk_u32_eq`, `cttk_s64_eq`, `cttk_u64_eq`

  - Inequality comparison between two integers:
    `cttk_s32_neq`, `cttk_u32_neq`, `cttk_s64_neq`, `cttk_u64_neq`

  - Ordering ("greater than"):
    `cttk_s32_gt`, `cttk_u32_gt`, `cttk_s64_gt`, `cttk_u64_gt`

  - Ordering ("greater or equal"):
    `cttk_s32_geq`, `cttk_u32_geq`, `cttk_s64_geq`, `cttk_u64_geq`

  - Ordering ("lower than"):
    `cttk_s32_lt`, `cttk_u32_lt`, `cttk_s64_lt`, `cttk_u64_lt`

  - Ordering ("lower or equal"):
    `cttk_s32_leq`, `cttk_u32_leq`, `cttk_s64_leq`, `cttk_u64_leq`

  - Generic comparison (returns -1, 0 or 1):
    `cttk_s32_cmp`, `cttk_u32_cmp`, `cttk_s64_cmp`, `cttk_u64_cmp`

  - Comparison with zero ("greater than zero"):
    `cttk_s32_gt0`, `cttk_s64_gt0`

  - Comparison with zero ("greater than or equal to zero"):
    `cttk_s32_geq0`, `cttk_s64_geq0`

  - Comparison with zero ("lower than zero"):
    `cttk_s32_lt0`, `cttk_s64_lt0`

  - Comparison with zero ("lower than or equal to zero"):
    `cttk_s32_leq0`, `cttk_s64_leq0`

  - Generic sign extraction (-1, 0 or 1):
    `cttk_s32_sign`, `cttk_s64_sign`

The `cttk_u32_bitlength()` function computes the length, in bits, of an
unsigned 32-bit integer. The bit length of an integer _x_ is the
smallest integer _k_ such that _x_ is lower than two raised to the power
_k_.

## Hexadecimal Encoding And Decoding

`cttk_hextobin_gen` parses hexadecimal digits into binary.
`cttk_bintohex_gen` performs the reverse operation: encoding binary
data into hexadecimal. These functions are tunable:

  - Decoding may tolerate, or not, intervening whitespace.
  - Decoding may accept, or not, input data with an odd number of
    hexadecimal digits (a trailing `'0'` is then assumed to complete
    the last byte).
  - Encoding may use uppercase or lowercase letters.

The implementation protects the value of bytes and digits from
outsiders. It cannot, however, hide the _number_ of hexadecimal digits
or the length, in bytes, of the binary data. If whitespace is accepted
(and ignored), then location of whitespace within the source string may
conceptually leak as well.

## Base64 Encoding And Decoding

Constant-time Base64 encoding and decoding is planned but not yet
implemented. The API is already defined in `cttk.h`, similarly to
the hexadecimal encoding/decoding functions.

## Native Integer Multiplications

Not all CPU provide constant-time multiplication opcodes; see
[this page](https://www.bearssl.org/ctmul.html) for details. CTTK
provide some constant-time multiplication primitives:

  - `cttk_mulu32()`: multiplication of two 32-bit unsigned integers,
    with a 32-bit result (the low 32 bits of the result).

  - `cttk_muls32()`: multiplication of two 32-bit signed integers,
    with a 32-bit result (truncation to the low 32 bits). Note that
    in plain C, overflows on signed integers trigger undefined
    behaviour; with CTTK, truncation is guaranteed.

  - `cttk_mulu32w()`: multiplication of two 32-bit unsigned integers,
    with a 64-bit result (`uint64_t`).

  - `cttk_muls32w()`: multiplication of two 32-bit signed integers,
    with a 64-bit result (`int64_t`).

  - `cttk_mulu64()`: multiplication of two 64-bit unsigned integers,
    with a 64-bit result (the low 64 bits of the result).

  - `cttk_muls64()`: multiplication of two 64-bit signed integers,
    with a 64-bit result (truncation to the low 64 bits). As for
    `cttk_muls32()`, CTTK guarantees truncating behaviour.

The default implementation of these functions is (nominally)
constant-time on all architectures, but not very efficient. Some
compile-time options (see `config.h`) can be used to force use of the
native multiplication operator, if you are certain that your code will
always run on hardware platforms that provide constant-time
multiplications (the gist of the Web page linked to above is that such a
bet is risky).

## Big Integers

CTTK provides a constant-time implementation of big integers with a
configurable size. In fact, _several_ implementations are planned, for
better performance on various architectures; application code should use
the generic macros that will select the "right one" automatically.

A big integer value has the following characteristics:

  - It has a defined _size_ which qualifies the space in which the
    value exists. The size is expressed in bits. If the size is _n_
    bits, then the value may range between `-2**(n-1)` and `2**(n-1)-1`
    (where "`**`" stands for exponentiation). All bit sizes, starting
    from 1, are supported. For instance, you can have 17-bit integers.
    Note that the size includes the sign bit; thus, 16-bit integers
    will range from -32768 to +32767. There is no upper limit for the
    bit size except available RAM.

  - Each value may be either an integer in the defined range, or a NaN
    ("not a number"). A NaN is obtained whenever the mathematical result
    of an operation is not representable in the defined range (except
    for the operations that are explicitly defined as "truncating").
    NaNs propagate: if an operand to an arithmetic operation is a NaN,
    then the result will also be a NaN.

  - The C type for a big integer is an array. The `cti_def` macro is
    used to declare a local variable or structure field that is
    large enough for big integers up to a given size. Since that type
    is an array, it is automatically passed by reference to called
    functions.

  - To become usable, the big integer object must be initialized with
    `cti_init()` (alternatively, it may be declared and initialized as a
    local variable with the `cti_definit` macro). Initialization sets
    the value to NaN but also encodes the actual size of the integer.
    Using an uninitialized big integer with any of the functions may
    trigger "bad things" such as buffer overflows.

  - Big integers do not grow or shrink. They do not involve dynamic
    memory allocation either. Thus, there is no "release" function.
    They can be forgotten just like any plain local variable.

The following rules are applicable to all big integer functions:

  - The function name starts with `cti_`.

  - All these function names are actually macros that map to the
    selected implementation functions.

  - Some of the functions may be inline functions, for better
    performance.

  - Destination operands come before source operands. This mimics
    mathematical notation (in "d = a + b", the destination is on
    the left) and established usage in the C standard library
    (e.g. the `memcpy()` function).

  - All operand overlaps are allowed; that is, it is permitted
    to write things such as `cti_mul(x, x, y)` to perform a
    multiplication of `x` by `y` and write the result in `x`.

  - Some operations internally need temporary buffers. These will
    be usually stack-allocated, but for large integers, `malloc()`
    is used. This is transparent to applications: the buffers are
    released before returning to the caller. For the benefit of
    embedded systems which might not have a `malloc()`, it is
    possible to disable use of that function with the compile-time
    option `CTTK_NO_MALLOC`. If `malloc()` was disabled or failed
    to allocate the required memory, then the result will be set
    to NaN.

  - If operand sizes do not match (both source and destination),
    then the result is set to NaN.

  - Constant-time protection is on the integer values, not the
    sizes. The "size" here is not the actual bit length of the value
    (that information is protected) but the representable range
    used by the containing variable. Similarly, the location in RAM
    of any value cannot be protected.

  - Whether an integer value is NaN or not is protected against
    time-based side channels. The `cti_isnan()` function returns
    the NaN status of an integer as a `cttk_bool`.

  - For shift operators, default functions (e.g. `cti_lsh()`) protect
    the values of the source integer and the result value, but not the
    shift count. If the shift count is also secret, then alternate
    implementations are provided (`cti_lsh_prot()`...) but they are
    substantially slower.

An example of the use of CTTK big integer code is shown below. It is an
implementation of a function that computes the average of many 64-bit
integers, and prints the result in decimal (with a precision of 12
digits in the fractional part). The individual values are considered
secret, while the average is not. (This example is not meant to be
realistic, but to demonstrate the usage syntax.)

    #include <stdio.h>
    #include <stdlib.h>
    #include "cttk.h"

    void
    print_average(const uint64_t *values, uint64_t num)
    {
            /*
             * We may have up to 2^64 values of 64 bits each, thus a sum
             * of up to a bit less than 2^128. Since our big integers
             * are signed, we need 129-bit integers.
             */
            cti_definit(s, 129);
            cti_definit(x, 129);
            uint64_t u, hi, lo;

            /*
             * Compute the sum of all integers in s.
             */
            cti_set_u32(s, 0);
            for (u = 0; u < num; u ++) {
                    cti_set_u64(x, values[u]);
                    cti_add(s, s, x);
            }

            /*
             * Divide the sum by the number of integers. The quotient
             * will be the integral part of the average.
             */
            cti_set_u64(x, num);
            cti_divrem(x, s, s, x);
            hi = cti_to_u64(x);

            /*
             * To get the fractional part, properly rounded to 12
             * digits, we multiply the remainder by 10^12, then
             * add num/2 (for rounding), and divide by num.
             */
            cti_set_u64(x, 1000000000000);
            cti_mul(s, s, x);
            cti_set_u64(x, num >> 1);
            cti_add(s, s, x);
            cti_set_u64(x, num);
            cti_div(x, s, x);
            lo = cti_to_u64(x);

            printf("avg = %llu.%012llu\n",
                    (unsigned long long)hi, (unsigned long long)lo);
    }
