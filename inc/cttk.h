/*
 * Copyright (c) 2018 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CTTK_H__
#define CTTK_H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

/*
 * On MSVC, disable the warning about applying unary minus on an
 * unsigned type: it is standard, we do it all the time, and for
 * good reasons.
 */
#if _MSC_VER
#pragma warning( disable : 4146 )
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ==================================================================== */
/*
 * Boolean values.
 */

/**
 * \brief Type for a boolean.
 *
 * This is conceptually a boolean; however, the C compiler sees it as
 * a type which may have many distinct values. It is meant to enable
 * or disable constant-time operations. This type is defined as a
 * `struct` so that it cannot be used directly to control a conditional
 * jump, since such an operation would contradict constant-time behaviour.
 */
typedef struct {
#ifndef CTTK_DOXYGEN_IGNORE
	uint32_t v;
#endif
} cttk_bool;

/**
 * \brief Initializer for a boolean.
 *
 * This macro is supposed to be used in variable initializers. The
 * parameter MUST evaluate to either 1 (for "true") or 0 (for "false").
 *
 * \param x   1 for "true", 0 for "false".
 */
#define CTTK_INIT(x)   { (x) }

/**
 * \brief Static constant value for a boolean "true".
 */
static const cttk_bool cttk_true = CTTK_INIT(1);

/**
 * \brief Static constant value for a boolean "false".
 */
static const cttk_bool cttk_false = CTTK_INIT(0);

/**
 * \brief Make a boolean out of a signed 32-bit integer.
 *
 * The source integer MUST have value 0 or 1.
 *
 * \param x   1 for "true", 0 for "false".
 * \return  the boolean value.
 */
static inline cttk_bool
cttk_bool_of_s32(int32_t x)
{
	cttk_bool c;

	c.v = (uint32_t)x;
	return c;
}

/**
 * \brief Make a boolean out of an unsigned 32-bit integer.
 *
 * The source integer MUST have value 0 or 1.
 *
 * \param x   1 for "true", 0 for "false".
 * \return  the boolean value.
 */
static inline cttk_bool
cttk_bool_of_u32(uint32_t x)
{
	cttk_bool c;

	c.v = x;
	return c;
}

/**
 * \brief Convert a boolean to an integer.
 *
 * This function returns 1 if `c` is "true", 0 otherwise. This function
 * allows returning values to "normal" C values, on which comparisons
 * and conditional jumps can be made. Of course, any such conditional
 * jump is non-constant-time; therefore, this function shall be called
 * only on values that are now deemed "non secret".
 *
 * \param c   the boolean to convert.
 * \return  0 for false, 1 for true.
 */
static inline int
cttk_bool_to_int(cttk_bool c)
{
	return (int)c.v;
}

/**
 * \brief Negate a control value.
 *
 * \param c   the value to negate.
 * \return  the negated value.
 */
static inline cttk_bool
cttk_not(cttk_bool c)
{
	c.v ^= (uint32_t)1;
	return c;
}

/**
 * \brief Logical AND of control values.
 *
 * \param c1   first operand.
 * \param c2   second operand.
 * \return  the logical AND of the two operands.
 */
static inline cttk_bool
cttk_and(cttk_bool c1, cttk_bool c2)
{
	c1.v &= c2.v;
	return c1;
}

/**
 * \brief Logical OR of control values.
 *
 * \param c1   first operand.
 * \param c2   second operand.
 * \return  the logical OR of the two operands.
 */
static inline cttk_bool
cttk_or(cttk_bool c1, cttk_bool c2)
{
	c1.v |= c2.v;
	return c1;
}

/**
 * \brief Logical XOR of control values.
 *
 * Result is "true" if both operands are different, "false" otherwise.
 *
 * \param c1   first operand.
 * \param c2   second operand.
 * \return  the logical XOR of the two operands.
 */
static inline cttk_bool
cttk_xor(cttk_bool c1, cttk_bool c2)
{
	c1.v ^= c2.v;
	return c1;
}

/**
 * \brief Logical EQV of control values.
 *
 * Result is "true" if both operands are equal, "false" otherwise.
 *
 * \param c1   first operand.
 * \param c2   second operand.
 * \return  the logical EQV of the two operands.
 */
static inline cttk_bool
cttk_eqv(cttk_bool c1, cttk_bool c2)
{
	return cttk_xor(c1, cttk_not(c2));
}

/* ==================================================================== */
/*
 * Elementary operations with 32-bit and 64-bit integers.
 */

/**
 * \brief Multiplexer for signed 32-bit integers.
 *
 * This returns `x` if `ctl` is true, or `y` if `ctl` is false.
 *
 * \param ctl   control value.
 * \param x     value to return if `ctl` is true.
 * \param y     value to return if `ctl` is false.
 * \return  the multiplexer output.
 */
static inline int32_t
cttk_s32_mux(cttk_bool ctl, int32_t x, int32_t y)
{
	return y ^ (-(int32_t)ctl.v & (x ^ y));
}

/**
 * \brief Multiplexer for unsigned 32-bit integers.
 *
 * This returns `x` if `ctl` is true, or `y` if `ctl` is false.
 *
 * \param ctl   control value.
 * \param x     value to return if `ctl` is true.
 * \param y     value to return if `ctl` is false.
 * \return  the multiplexer output.
 */
static inline uint32_t
cttk_u32_mux(cttk_bool ctl, uint32_t x, uint32_t y)
{
	return y ^ (-ctl.v & (x ^ y));
}

/**
 * \brief Multiplexer for signed 64-bit integers.
 *
 * This returns `x` if `ctl` is true, or `y` if `ctl` is false.
 *
 * \param ctl   control value.
 * \param x     value to return if `ctl` is true.
 * \param y     value to return if `ctl` is false.
 * \return  the multiplexer output.
 */
static inline int64_t
cttk_s64_mux(cttk_bool ctl, int64_t x, int64_t y)
{
	return y ^ (-(int64_t)ctl.v & (x ^ y));
}

/**
 * \brief Multiplexer for unsigned 64-bit integers.
 *
 * This returns `x` if `ctl` is true, or `y` if `ctl` is false.
 *
 * \param ctl   control value.
 * \param x     value to return if `ctl` is true.
 * \param y     value to return if `ctl` is false.
 * \return  the multiplexer output.
 */
static inline uint64_t
cttk_u64_mux(cttk_bool ctl, uint64_t x, uint64_t y)
{
	return y ^ (-(uint64_t)ctl.v & (x ^ y));
}

/**
 * \brief Test a signed 32-bit integer against 0.
 *
 * This returns false id `x` is 0, true otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is different from 0.
 */
static inline cttk_bool
cttk_s32_neq0(int32_t x)
{
	uint32_t q;

	q = (uint32_t)x | -(uint32_t)x;
	return cttk_bool_of_u32((q | -q) >> 31);
}

/**
 * \brief Test an unsigned 32-bit integer against 0.
 *
 * This returns false id `x` is 0, true otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is different from 0.
 */
static inline cttk_bool
cttk_u32_neq0(uint32_t x)
{
	uint32_t q;

	q = x | -x;
	return cttk_bool_of_u32((q | -q) >> 31);
}

/**
 * \brief Test a signed 64-bit integer against 0.
 *
 * This returns false id `x` is 0, true otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is different from 0.
 */
static inline cttk_bool
cttk_s64_neq0(int64_t x)
{
	uint64_t q;

	q = (uint64_t)x | -(uint64_t)x;
	return cttk_bool_of_u32((uint32_t)((q | -q) >> 63));
}

/**
 * \brief Test an unsigned 64-bit integer against 0.
 *
 * This returns false id `x` is 0, true otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is different from 0.
 */
static inline cttk_bool
cttk_u64_neq0(uint64_t x)
{
	uint64_t q;

	q = x | -x;
	return cttk_bool_of_u32((uint32_t)((q | -q) >> 63));
}

/**
 * \brief Equality test of a signed 32-bit integer with 0.
 *
 * This returns true id `x` is 0, false otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is equal to 0.
 */
static inline cttk_bool
cttk_s32_eq0(int32_t x)
{
	return cttk_not(cttk_s32_neq0(x));
}

/**
 * \brief Equality test of an unsigned 32-bit integer with 0.
 *
 * This returns true id `x` is 0, false otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is equal to 0.
 */
static inline cttk_bool
cttk_u32_eq0(uint32_t x)
{
	return cttk_not(cttk_u32_neq0(x));
}

/**
 * \brief Equality test of a signed 64-bit integer with 0.
 *
 * This returns true id `x` is 0, false otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is equal to 0.
 */
static inline cttk_bool
cttk_s64_eq0(int64_t x)
{
	return cttk_not(cttk_s64_neq0(x));
}

/**
 * \brief Equality test of an unsigned 64-bit integer with 0.
 *
 * This returns true id `x` is 0, false otherwise.
 *
 * \param x   the operand.
 * \return  true if and only if `x` is equal to 0.
 */
static inline cttk_bool
cttk_u64_eq0(uint64_t x)
{
	return cttk_not(cttk_u64_neq0(x));
}

/**
 * \brief Equality check for signed 32-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_s32_eq(int32_t x, int32_t y)
{
	return cttk_s32_eq0(x ^ y);
}

/**
 * \brief Equality check for unsigned 32-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_u32_eq(uint32_t x, uint32_t y)
{
	return cttk_u32_eq0(x ^ y);
}

/**
 * \brief Equality check for signed 64-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_s64_eq(int64_t x, int64_t y)
{
	return cttk_s64_eq0(x ^ y);
}

/**
 * \brief Equality check for unsigned 64-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_u64_eq(uint64_t x, uint64_t y)
{
	return cttk_u64_eq0(x ^ y);
}

/**
 * \brief Inequality check for signed 32-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_s32_neq(int32_t x, int32_t y)
{
	return cttk_s32_neq0(x ^ y);
}

/**
 * \brief Inequality check for unsigned 32-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_u32_neq(uint32_t x, uint32_t y)
{
	return cttk_u32_neq0(x ^ y);
}

/**
 * \brief Inequality check for signed 64-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_s64_neq(int64_t x, int64_t y)
{
	return cttk_s64_neq0(x ^ y);
}

/**
 * \brief Inequality check for unsigned 64-bit integers.
 *
 * This returns true if `x` and `y` contain the same value, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the comparison result.
 */
static inline cttk_bool
cttk_u64_neq(uint64_t x, uint64_t y)
{
	return cttk_u64_neq0(x ^ y);
}

/**
 * \brief Comparison between signed 32-bit integers ("greater than").
 *
 * This returns true if `x` is strictly greater than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_s32_gt(int32_t x, int32_t y)
{
	/*
	 * We perform operations on unsigned types to avoid any overflow
	 * (since that would induce undefined behaviour).
	 *
	 * If x and y have the same sign, then y-x will have its high bit
	 * set if x > y, cleared otherwise.
	 *
	 * If x and y have different signs, then the result is the
	 * high bit of y (if that bit is set, then y < 0).
	 */
	uint32_t z;

	z = (uint32_t)y - (uint32_t)x;
	return cttk_bool_of_u32((z ^ (((uint32_t)x ^ (uint32_t)y)
		& ((uint32_t)y ^ z))) >> 31);
}

/**
 * \brief Comparison between unsigned 32-bit integers ("greater than").
 *
 * This returns true if `x` is strictly greater than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_u32_gt(uint32_t x, uint32_t y)
{
	/*
	 * If both x < 2^31 and y < 2^31, then y-x will have its high
	 * bit set if x > y, cleared otherwise.
	 *
	 * If either x >= 2^31 or y >= 2^31 (but not both), then the
	 * result is the high bit of x.
	 *
	 * If both x >= 2^31 and y >= 2^31, then we can virtually
	 * subtract 2^31 from both, and we are back to the first
	 * case. Since (y-2^31)-(x-2^31) = y-x, the direct subtraction
	 * is already fine.
	 */
	uint32_t z;

	z = y - x;
	return cttk_bool_of_u32((z ^ ((x ^ y) & (x ^ z))) >> 31);
}

/**
 * \brief Comparison between signed 64-bit integers ("greater than").
 *
 * This returns true if `x` is strictly greater than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_s64_gt(int64_t x, int64_t y)
{
	/*
	 * We perform operations on unsigned types to avoid any overflow
	 * (since that would induce undefined behaviour).
	 *
	 * If x and y have the same sign, then y-x will have its high bit
	 * set if x > y, cleared otherwise.
	 *
	 * If x and y have different signs, then the result is the
	 * high bit of y (if that bit is set, then y < 0).
	 */
	uint64_t z;

	z = (uint64_t)y - (uint64_t)x;
	return cttk_bool_of_u32((uint32_t)((z ^ (((uint64_t)x ^ (uint64_t)y)
		& ((uint64_t)y ^ z))) >> 63));
}

/**
 * \brief Comparison between unsigned 64-bit integers ("greater than").
 *
 * This returns true if `x` is strictly greater than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_u64_gt(uint64_t x, uint64_t y)
{
	/*
	 * If both x < 2^63 and y < 2^63, then y-x will have its high
	 * bit set if x > y, cleared otherwise.
	 *
	 * If either x >= 2^63 or y >= 2^63 (but not both), then the
	 * result is the high bit of x.
	 *
	 * If both x >= 2^63 and y >= 2^63, then we can virtually
	 * subtract 2^63 from both, and we are back to the first
	 * case. Since (y-2^63)-(x-2^63) = y-x, the direct subtraction
	 * is already fine.
	 */
	uint64_t z;

	z = y - x;
	return cttk_bool_of_u32((uint32_t)((z ^ ((x ^ y) & (x ^ z))) >> 63));
}

/**
 * \brief Comparison between signed 32-bit integers ("greater or equal").
 *
 * This returns true if `x` is greater than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_s32_geq(int32_t x, int32_t y)
{
	return cttk_not(cttk_s32_gt(y, x));
}

/**
 * \brief Comparison between unsigned 32-bit integers ("greater or equal").
 *
 * This returns true if `x` is greater than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_u32_geq(uint32_t x, uint32_t y)
{
	return cttk_not(cttk_u32_gt(y, x));
}

/**
 * \brief Comparison between signed 64-bit integers ("greater or equal").
 *
 * This returns true if `x` is greater than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_s64_geq(int64_t x, int64_t y)
{
	return cttk_not(cttk_s64_gt(y, x));
}

/**
 * \brief Comparison between unsigned 64-bit integers ("greater or equal").
 *
 * This returns true if `x` is greater than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_u64_geq(uint64_t x, uint64_t y)
{
	return cttk_not(cttk_u64_gt(y, x));
}

/**
 * \brief Comparison between signed 32-bit integers ("lower than").
 *
 * This returns true if `x` is strictly lower than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_s32_lt(int32_t x, int32_t y)
{
	return cttk_s32_gt(y, x);
}

/**
 * \brief Comparison between unsigned 32-bit integers ("lower than").
 *
 * This returns true if `x` is strictly lower than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_u32_lt(uint32_t x, uint32_t y)
{
	return cttk_u32_gt(y, x);
}

/**
 * \brief Comparison between signed 64-bit integers ("lower than").
 *
 * This returns true if `x` is strictly lower than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_s64_lt(int64_t x, int64_t y)
{
	return cttk_s64_gt(y, x);
}

/**
 * \brief Comparison between unsigned 64-bit integers ("lower than").
 *
 * This returns true if `x` is strictly lower than `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x > y, false otherwise.
 */
static inline cttk_bool
cttk_u64_lt(uint64_t x, uint64_t y)
{
	return cttk_u64_gt(y, x);
}

/**
 * \brief Comparison between signed 32-bit integers ("lower or equal").
 *
 * This returns true if `x` is lower than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_s32_leq(int32_t x, int32_t y)
{
	return cttk_not(cttk_s32_gt(x, y));
}

/**
 * \brief Comparison between unsigned 32-bit integers ("lower or equal").
 *
 * This returns true if `x` is lower than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_u32_leq(uint32_t x, uint32_t y)
{
	return cttk_not(cttk_u32_gt(x, y));
}

/**
 * \brief Comparison between signed 64-bit integers ("lower or equal").
 *
 * This returns true if `x` is lower than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_s64_leq(int64_t x, int64_t y)
{
	return cttk_not(cttk_s64_gt(x, y));
}

/**
 * \brief Comparison between unsigned 64-bit integers ("lower or equal").
 *
 * This returns true if `x` is lower than or equal to `y`, false otherwise.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if x >= y, false otherwise.
 */
static inline cttk_bool
cttk_u64_leq(uint64_t x, uint64_t y)
{
	return cttk_not(cttk_u64_gt(x, y));
}

/**
 * \brief Generic comparison between signed 32-bit integers.
 *
 * Return value is -1, 0 or 1, if `x` is lower than, equal to, or greater
 * than `y`, respectively. Since the returned value is numerical, it can
 * be used for conditional jumps, which are non-constant-time.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  -1, 0 or 1.
 */
static inline int32_t
cttk_s32_cmp(int32_t x, int32_t y)
{
	return (int32_t)cttk_s32_gt(x, y).v | -(int32_t)cttk_s32_gt(y, x).v;
}

/**
 * \brief Generic comparison between unsigned 32-bit integers.
 *
 * Return value is -1, 0 or 1, if `x` is lower than, equal to, or greater
 * than `y`, respectively. Since the returned value is numerical, it can
 * be used for conditional jumps, which are non-constant-time.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  -1, 0 or 1.
 */
static inline int32_t
cttk_u32_cmp(uint32_t x, uint32_t y)
{
	return (int32_t)cttk_u32_gt(x, y).v | -(int32_t)cttk_u32_gt(y, x).v;
}

/**
 * \brief Generic comparison between signed 64-bit integers.
 *
 * Return value is -1, 0 or 1, if `x` is lower than, equal to, or greater
 * than `y`, respectively. Since the returned value is numerical, it can
 * be used for conditional jumps, which are non-constant-time.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  -1, 0 or 1.
 */
static inline int32_t
cttk_s64_cmp(int64_t x, int64_t y)
{
	return (int32_t)cttk_s64_gt(x, y).v | -(int32_t)cttk_s64_gt(y, x).v;
}

/**
 * \brief Generic comparison between unsigned 64-bit integers.
 *
 * Return value is -1, 0 or 1, if `x` is lower than, equal to, or greater
 * than `y`, respectively. Since the returned value is numerical, it can
 * be used for conditional jumps, which are non-constant-time.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  -1, 0 or 1.
 */
static inline int32_t
cttk_u64_cmp(uint64_t x, uint64_t y)
{
	return (int32_t)cttk_u64_gt(x, y).v | -(int32_t)cttk_u64_gt(y, x).v;
}

/**
 * \brief Compare a signed 32-bit integer with 0 ("greater than").
 *
 * \param x   the operand.
 * \return  true if x > 0, false otherwise.
 */
static inline cttk_bool
cttk_s32_gt0(int32_t x)
{
	/*
	 * High bit of -x is 0 is x == 0, but 1 if x > 0.
	 */
	uint32_t q;

	q = (uint32_t)x;
	return cttk_bool_of_u32((~q & -q) >> 31);
}

/**
 * \brief Compare a signed 64-bit integer with 0 ("greater than").
 *
 * \param x   the operand.
 * \return  true if x > 0, false otherwise.
 */
static inline cttk_bool
cttk_s64_gt0(int64_t x)
{
	/*
	 * High bit of -x is 0 is x == 0, but 1 if x > 0.
	 */
	uint64_t q;

	q = (uint64_t)x;
	return cttk_bool_of_u32((uint32_t)((~q & -q) >> 63));
}

/**
 * \brief Compare a signed 32-bit integer with 0 ("greater or equal").
 *
 * \param x   the operand.
 * \return  true if x >= 0, false otherwise.
 */
static inline cttk_bool
cttk_s32_geq0(int32_t x)
{
	return cttk_bool_of_u32((~(uint32_t)x) >> 31);
}

/**
 * \brief Compare a signed 64-bit integer with 0 ("greater or equal").
 *
 * \param x   the operand.
 * \return  true if x >= 0, false otherwise.
 */
static inline cttk_bool
cttk_s64_geq0(int64_t x)
{
	return cttk_bool_of_u32((uint32_t)((~(uint64_t)x) >> 63));
}

/**
 * \brief Compare a signed 32-bit integer with 0 ("lower than").
 *
 * \param x   the operand.
 * \return  true if x < 0, false otherwise.
 */
static inline cttk_bool
cttk_s32_lt0(int32_t x)
{
	return cttk_bool_of_u32((uint32_t)x >> 31);
}

/**
 * \brief Compare a signed 64-bit integer with 0 ("lower than").
 *
 * \param x   the operand.
 * \return  true if x < 0, false otherwise.
 */
static inline cttk_bool
cttk_s64_lt0(int64_t x)
{
	return cttk_bool_of_u32((uint32_t)((uint64_t)x >> 63));
}

/**
 * \brief Compare a signed 32-bit integer with 0 ("lower or equal").
 *
 * \param x   the operand.
 * \return  true if x <= 0, false otherwise.
 */
static inline cttk_bool
cttk_s32_leq0(int32_t x)
{
	/*
	 * We return true if the high bit of x is set, or if x is zero.
	 */
	uint32_t q;

	q = (uint32_t)x;
	return cttk_bool_of_u32((q | ~(q | -q)) >> 31);
}

/**
 * \brief Compare a signed 64-bit integer with 0 ("lower or equal").
 *
 * \param x   the operand.
 * \return  true if x <= 0, false otherwise.
 */
static inline cttk_bool
cttk_s64_leq0(int64_t x)
{
	/*
	 * We return true if the high bit of x is set, or if x is zero.
	 */
	uint64_t q;

	q = (uint64_t)x;
	return cttk_bool_of_u32((uint32_t)((q | ~(q | -q)) >> 63));
}

/**
 * \brief Get the sign of a 32-bit integer.
 *
 * This function returns -1, 0 or 1, depending on whether `x` is
 * negative, 0 or positive.
 *
 * \param x   the operand.
 * \return  the sign of `x` (-1, 0 or 1).
 */
static inline int32_t
cttk_s32_sign(int32_t x)
{
	/*
	 * If x < 0, then its high bit is set, and the first part of
	 * the expression below fills the output with 1 (i.e. you get
	 * a result of -1).
	 *
	 * If x > 0, then the high bit of x is 0, and '-q' will have
	 * is high bit set, leading to a result of 1.
	 *
	 * If x = 0, then both parts of the expression are 0, so the
	 * result is 0.
	 */
	uint32_t q;

	q = (uint32_t)x;
	return -(int32_t)(q >> 31) | (int32_t)(-q >> 31);
}

/**
 * \brief Get the sign of a 64-bit integer.
 *
 * This function returns -1, 0 or 1, depending on whether `x` is
 * negative, 0 or positive.
 *
 * \param x   the operand.
 * \return  the sign of `x` (-1, 0 or 1).
 */
static inline int32_t
cttk_s64_sign(int64_t x)
{
	/*
	 * If x < 0, then its high bit is set, and the first part of
	 * the expression below fills the output with 1 (i.e. you get
	 * a result of -1).
	 *
	 * If x > 0, then the high bit of x is 0, and '-q' will have
	 * is high bit set, leading to a result of 1.
	 *
	 * If x = 0, then both parts of the expression are 0, so the
	 * result is 0.
	 */
	uint64_t q;

	q = (uint64_t)x;
	return -(int32_t)(q >> 63) | (int32_t)(-q >> 63);
}

/**
 * \brief Get the bitlength of an unsigned 32-bit integer.
 *
 * The bit length of `x` is the smallest integer n such that
 * `x` is lower than 2 raised to the power n. The bit length of 0
 * is 0.
 *
 * \param x   the operand.
 * \return  the bit length of `x` (0 to 32).
 */
static inline uint32_t
cttk_u32_bitlength(uint32_t x)
{
	uint32_t xh, b;
	uint32_t r;

	r = 0;

	/*
	 * We split x into its high and low halves. If the high half is
	 * 0, then the bit length of x is the bit length of the low
	 * half. If the high half is not 0, then the bitlength is 16
	 * added to the bit length of the high half.
	 *
	 * To test for equality with 0, we add 0xFFFF: this produces a
	 * carry into bit 16, except for 0.
	 */
	xh = x >> 16;
	b = (xh + 0xFFFF) >> 16;
	x ^= (x ^ xh) & -b;
	r |= b << 4;

	/*
	 * We use the same method to reduce the length of x to 8 bits,
	 * then 4 bits, then 2 bits.
	 */
	xh = x >> 8;
	b = (xh + 0xFF) >> 8;
	x ^= (x ^ xh) & -b;
	r |= b << 3;
	xh = x >> 4;
	b = (xh + 0x0F) >> 4;
	x ^= (x ^ xh) & -b;
	r |= b << 2;
	xh = x >> 2;
	b = (xh + 0x03) >> 2;
	x ^= (x ^ xh) & -b;
	r |= b << 1;

	/*
	 * Now, x fits on two bits: its value is 0, 1, 2 or 3. The
	 * possible lengths (to add to the current value of r) are:
	 *   x  length
	 *   0    0
	 *   1    1
	 *   2    2
	 *   3    2
	 *
	 * Thus, x is equal to its length, unless it has value 3.
	 */
	return r + x - ((x + 1) >> 2);
}

/* ==================================================================== */

/**
 * \brief Conditionally copy data from `src` into `dst`.
 *
 * If `ctl` is "true", then the `len` bytes starting at address `dst`
 * are copied into `src`; otherwise, the contents of `dst` are unmodified.
 * `src` and `dst` may overlap partially or totally; in case of overlap,
 * the new contents of the object pointed to by `dst` are set (if `ctl`
 * is "true") to a copy of the contents of the source object before the
 * operation. This matches the semantics of the standard function
 * `memmove()`.
 *
 * The object pointed to by `src` is unmodified, except in case of overlap
 * with the destination object, as per the semantics explained above.
 *
 * It is acceptable that `len` is zero, in which case nothing is copied.
 *
 * \param ctl   control value.
 * \param dst   destination buffer.
 * \param src   source buffer.
 * \param len   number of bytes to conditionally copy.
 */
void cttk_cond_copy(cttk_bool ctl, void *dst, const void *src, size_t len);

/**
 * \brief Conditionally swap the contents of two memory areas.
 *
 * If `ctl` is "true", then the `len` bytes located at the address `a`
 * are copied into the `len` bytes located at the address `b`, and vice
 * versa. If `ctl` is "false", then no value is modified. It is acceptable
 * that `len` is zero, in which case no byte is modified.
 *
 * The two areas MUST NOT overlap in any way.
 *
 * \param ctl   control value.
 * \param a     first area.
 * \param b     second area.
 * \param len   number of bytes to conditionally swap.
 */
void cttk_cond_swap(cttk_bool ctl, void *a, void *b, size_t len);

/**
 * \brief Constant-time array look-up (read).
 *
 * Given an array of `num_len` elements, of size `elt_len` bytes each,
 * this function reads element `index` and writes it into the buffer `d`.
 * The value `a` points at the first array element, whose index is 0.
 * The array values _and_ the index are protected. The cost of this
 * function is proportional to the total array size.
 *
 * The caller is responsible for making sure that the requested index
 * is within the proper array range.
 *
 * \param d         destination buffer (must have length at least `elt_len`).
 * \param a         pointer to first array element.
 * \param elt_len   individual element length (in bytes).
 * \param num_len   number of elements in the array.
 * \param index     index of the element to read.
 */
void cttk_array_read(void *d,
	const void *a, size_t elt_len, size_t num_len, size_t index);

/**
 * \brief Constant-time array look-up (write).
 *
 * Given an array of `num_len` elements, of size `elt_len` bytes each,
 * this function writes a given value into the array element at `index`.
 * The value `a` points at the first array element, whose index is 0.
 * The array values _and_ the index are protected. The cost of this
 * function is proportional to the total array size.
 *
 * The caller is responsible for making sure that the requested index
 * is within the proper array range.
 *
 * \param a         pointer to first array element.
 * \param elt_len   individual element length (in bytes).
 * \param num_len   number of elements in the array.
 * \param index     index of the element to write.
 * \param s         source value (`elt_len` bytes).
 */
void cttk_array_write(void *a, size_t elt_len, size_t num_len,
	size_t index, const void *s);

/**
 * \brief Constant-time comparison (equality).
 *
 * This function compares the two buffers at addresses `src1` and `src2`,
 * both of length `len` bytes. Returned value is true if the buffers
 * have equal contents, false otherwise.
 *
 * All source bytes are touched; the comparison result, and the offset
 * of the first differing byte, do not leak.
 *
 * \param src1   first buffer for comparison.
 * \param src2   second buffer for comparison.
 * \param len    buffer length (in bytes).
 * \return  true on equal contents.
 */
cttk_bool cttk_array_eq(const void *src1, const void *src2, size_t len);

/**
 * \brief Constant-time comparison (inequality).
 *
 * This function compares the two buffers at addresses `src1` and `src2`,
 * both of length `len` bytes. Returned value is true if the buffers
 * have different contents, false otherwise.
 *
 * All source bytes are touched; the comparison result, and the offset
 * of the first differing byte, do not leak.
 *
 * \param src1   first buffer for comparison.
 * \param src2   second buffer for comparison.
 * \param len    buffer length (in bytes).
 * \return  true on different contents.
 */
static inline cttk_bool
cttk_array_neq(const void *src1, const void *src2, size_t len)
{
	return cttk_not(cttk_array_eq(src1, src2, len));
}

/**
 * \brief Constant-time comparison (lexicographic order).
 *
 * This function compares the two buffers at addresses `src1` and `src2`,
 * both of length `len` bytes. Returned value is -1, 0 or 1, depending
 * on whether the first buffer is lower than, equal to, or greater than
 * the second buffer. Lexicographic order is applied on the individual
 * byte values, considered as unsigned integer in the 0 to 255 range.
 *
 * All source bytes are touched; the comparison result, and the offset
 * of the first differing byte, do not leak.
 *
 * \param src1   first buffer for comparison.
 * \param src2   second buffer for comparison.
 * \param len    buffer length (in bytes).
 * \return  -1, 0 or 1.
 */
int32_t cttk_array_cmp(const void *src1, const void *src2, size_t len);

/* ==================================================================== */

/**
 * \brief Get the numerical value of an hexadecimal digit.
 *
 * Hexadecimal digits are ASCII digits from `'0'` to `'9'`, and ASCII
 * letters `'A'` to `'F'` (or `'a'` to `'f'`). This function returns
 * the numerical value of the character, or -1 if the character is not
 * an hexadecimal digit.
 *
 * \param c   hexadecimal digit.
 * \return  the digit numerical value, or -1.
 */
int cttk_hexval(char c);

/**
 * \brief Get the hexadecimal digit for a small value.
 *
 * The source value `x` MUST be between 0 and 15, inclusive. Returned
 * value is either an ASCII digit (`'0'` to `'9'`) or an ASCII
 * letter (`'A'` to `'F'`, if `uppercase` is non-zero, or `'a'` to
 * `'f'`, if `uppercase` is zero).
 *
 * \param x           value to convert (0 to 15).
 * \param uppercase   non-zero to get an uppercase output.
 * \return  the hexadecimal digit.
 */
char cttk_hexdigit(int x, int uppercase);

/**
 * \brief Scan a source string for hexadecimal characters.
 *
 * This function returns the offset of the first character in the
 * provided string which is not "acceptable" by the hexadecimal parser.
 * Acceptable characters are ASCII digits, ASCII uppercase letters
 * from `'A'` to `'F'`, and ASCII lowercase letters from `'a'` to `'f'`.
 * If `skipws` is non-zero, then whitespace is also considered acceptable
 * (whitespace is ASCII space and ASCII all control characters, i.e.
 * codes from 0x00 to 0x20, inclusive).
 *
 * If all `src_len` characters are acceptable, then `src_len` is returned.
 *
 * Constant-time: individual non-whitespace character values are protected.
 * Location of whitespace character, source length, and returned value,
 * may leak.
 *
 * \param src       source string.
 * \param src_len   source string length (in characters).
 * \param skipws    non-zero to accept whitespace as well.
 * \return  the number of acceptable hex characters.
 */
size_t cttk_hexscan(const char *src, size_t src_len, int skipws);

/**
 * \brief Convert an hexadecimal string into bytes.
 *
 * The `src_len` characters, starting at address `src`, are parsed as
 * hexadecimal digits; the resulting byte values are accumulated in
 * `dst`. If `src_len` is 0, then it is acceptable that `src` is `NULL`.
 *
 * If `dst` is `NULL`, then `dst_len` is ignored, and the returned value
 * is the number of bytes that should have been produced. If `dst` is
 * not `NULL`, then at most `dst_len` bytes are produced; if the source
 * string contains more encoded bytes than can fit in `dst`, then an
 * error is reported.
 *
 * Upon any parsing error, decoding stops; the returned value is the
 * number of bytes produced so far (or number of bytes that should have
 * been produced, if `dst` is `NULL`). If `err` is not `NULL`, then
 * `*err` is set to point at the first source character where the error
 * was noticed. If the entire source string could be processed with no
 * error, and `err` is not `NULL`, then `*err` is set to `NULL`.
 *
 * When the error is an insufficient output buffer length, `*err` is
 * set to point to the first hex digit that exceeds the capacity of the
 * output buffer (regardless of whether that digit is alone or not). If
 * `err` is `NULL`, then it is not possible to distinguish between a
 * fully successful decoding, and an insufficient output buffer capacity.
 *
 * The `flags` modify the decoding behaviour:
 *
 *   - If `CTTK_HEX_PAD_ODD` is set, then a final half-byte (the number of
 *     hexadecimal digits was odd) is not considered an error, and an
 *     implicit 0 is added. The padding is also applied if an invalid
 *     character is encountered after an odd number of valid hex digits.
 *     If the number of hex digits is odd and that flag is _not_ set,
 *     then the error will be reported as `*err` pointing to the character
 *     immediately beyond the last one of the string.
 *
 *   - If `CTTK_HEX_SKIP_WS` is set, then "whitespace" characters are
 *     simply skipped and do not trigger an error. For the purposes of
 *     this function, "whitespace" consists in bytes of value 32 or less
 *     (i.e. ASCII space, and all ASCII control characters).
 *
 * Constant-time behaviour: the values of hex digits are protected, but
 * not their number or location. Side channels may leak the total number
 * of hex digits, and the position of whitespace characters (if skipped).
 *
 * \param dst       destination buffer (or `NULL`).
 * \param dst_len   maximum size of the destination buffer (in bytes).
 * \param src       source string (can be `NULL` if `src_len` is zero).
 * \param src_len   source string length (in characters).
 * \param err       receiver for error character pointer, or `NULL`.
 * \param flags     behavioural flags.
 * \return  number of decoded bytes.
 */
size_t cttk_hextobin_gen(void *dst, size_t dst_len,
	const char *src, size_t src_len, const char **err, unsigned flags);

/**
 * \brief Hex decoding flag: pad final half-byte with a 0.
 */
#define CTTK_HEX_PAD_ODD       0x0001

/**
 * \brief Hex decoding flag: silently skip whitespace.
 */
#define CTTK_HEX_SKIP_WS       0x0002

/**
 * \brief Encode bytes into hexadecimal.
 *
 * The provided source bytes are encoded into hexadecimal. The destination
 * buffer length (`dst_len`) must be large enough to accommodate the
 * characters _and_ a terminating null byte.
 *
 * If `dst` is `NULL`, then `dst_len` is ignored; the returned value will
 * then be the number of digits that would be produced, i.e. exactly twice
 * the value of `src_len`.
 *
 * Returned value is the number of characters written, excluding the
 * terminating null byte. The terminating null byte will still be written,
 * except if `dst` is `NULL`, or `dst_len` is 0. If the output buffer is
 * too small, then output is truncated, but still with a terminating 0.
 *
 * The `flags` modify the behaviour:
 *
 *  - If `CTTK_HEX_UPPERCASE` is set, then produced hexadecimal digits
 *    will use uppercase letters (`A` to `F`, for values 10 to 15).
 *    Default behaviour is to use lowercase letters (`a` to `f`).
 *
 * Constant-time behaviour: the byte values are protected, but not the
 * source or destination lengths.
 *
 * \param dst       destination buffer, or `NULL`.
 * \param dst_len   destination buffer length (in characters).
 * \param src       source bytes (may be `NULL` if `src_len` is zero).
 * \param src_len   source length (in bytes).
 * \param flags     behavioural flags.
 * \return  the number of hexadecimal digits produced.
 */
size_t cttk_bintohex_gen(char *dst, size_t dst_len,
	const void *src, size_t src_len, unsigned flags);

/**
 * \brief Hex encoding flag: use uppercase letters.
 */
#define CTTK_HEX_UPPERCASE     0x0001

/* ==================================================================== */

/**
 * \brief Decode a Base64 string into bytes.
 *
 * The `src_len` characters, starting at address `src`, are parsed as
 * Base64 data; the resulting byte values are accumulated in
 * `dst`. If `src_len` is 0, then it is acceptable that `src` is `NULL`.
 *
 * If `dst` is `NULL`, then `dst_len` is ignored, and the returned value
 * is the number of bytes that should have been produced. If `dst` is
 * not `NULL`, then at most `dst_len` bytes are produced; if the source
 * string contains more encoded bytes than can fit in `dst`, then an
 * error is reported.
 *
 * Upon any parsing error, decoding stops; the returned value is the
 * number of bytes produced so far (or number of bytes that should have
 * been produced, if `dst` is `NULL`). If `err` is not `NULL`, then
 * `*err` is set to point at the first source character where the error
 * was noticed. If the entire source string could be processed with no
 * error, and `err` is not `NULL`, then `*err` is set to `NULL`.
 *
 * When the error is an insufficient output buffer length, `*err` is set
 * to point to the first character that makes the problem definite (in
 * order to maintain strict constant-time processing, reporting of such
 * a problem that could be detected only conditionally to the decoded
 * bit values is delayed to the next character). If `err` is `NULL`,
 * then it is not possible to distinguish between a fully successful
 * decoding, and an insufficient output buffer capacity.
 *
 * The `flags` modify the decoding behaviour:
 *
 *   - If `CTTK_B64DEC_NO_PAD` is set, then final padding characters
 *     (`'='` signs) are not expected. If such a character is present,
 *     then it will stop decoding at that point, and `*err` will be
 *     set to point to that character.
 *
 *   - If `CTTK_B64DEC_NO_WS` is set, then all source characters MUST
 *     be Base64 characters; whitespace (including line breaks) will not
 *     be tolerated, and be reported as an error if encountered. For the
 *     purposes of this function, "whitespace" consists in bytes of
 *     value 32 or less (i.e. ASCII space, and all ASCII control
 *     characters).
 *
 * An error is reported, pointing at the first character past the source
 * array, if the source buffer end is reached, or an `'='` padding sign
 * is read, and there are non-zero buffered bits at that point.
 *
 * Constant-time behaviour: the values of hex digits are protected, but
 * not their number or location. Side channels may leak the total number
 * of hex digits, and the position of whitespace characters (if skipped).
 * If the source string is erroneous by having extra non-zero bits in the
 * last chunk, then the value of these extra bits may leak as well (but
 * not of the non-extra bits).
 *
 * \param dst       destination buffer (or `NULL`).
 * \param dst_len   maximum size of the destination buffer (in bytes).
 * \param src       source string (can be `NULL` if `src_len` is zero).
 * \param src_len   source string length (in characters).
 * \param err       receiver for error character pointer, or `NULL`.
 * \param flags     behavioural flags.
 * \return  number of decoded bytes.
 */
size_t cttk_b64tobin_gen(void *dst, size_t dst_len,
	const char *src, size_t src_len, const char **err, unsigned flags);

/**
 * \brief Base64-decoding flag: do not expect `'='` signs as padding.
 */
#define CTTK_B64DEC_NO_PAD     0x0001

/**
 * \brief Base64-decoding flag: reject whitespace character in source string.
 */
#define CTTK_B64DEC_NO_WS      0x0002

/**
 * \brief Encode bytes into Base64.
 *
 * The provided source bytes are encoded into Base64. The destination
 * buffer length (`dst_len`) must be large enough to accommodate the
 * characters _and_ a terminating null byte.
 *
 * If `dst` is `NULL`, then `dst_len` is ignored; the returned value will
 * then be the number of characters that would be produced (not counting
 * the terminating null byte).
 *
 * Returned value is the number of characters written, excluding the
 * terminating null byte. The terminating null byte will still be written,
 * except if `dst` is `NULL`, or `dst_len` is 0.
 *
 * The `flags` modify the behaviour:
 *
 *   - If `CTTK_B64ENC_NO_PAD` is set, then the final `'='` signs (for
 *     padding the last character group, if needed) are not produced.
 *
 *   - If `CTTK_B64ENC_NEWLINE` is set, then line breaks will be added
 *     every 76 characters (by default). A line break will also be
 *     produced at the end of the last line, unless the input is empty.
 *
 *   - If `CTTK_B64ENC_CRLF` is set, then line breaks (if produced) will
 *     use a CR+LF sequence (0x0D and 0x0A bytes, in that order). By
 *     default, line breaks use a single LF (0x0A).
 *
 *   - If `CTTK_B64ENC_LINE64` is set, then line breaks (if produced)
 *     will occur every 64 characters instead of the default of 76. Some
 *     implementations of Base64 don't tolerate lines longer than 64
 *     characters.
 *
 * Constant-time behaviour: the source byte values are protected, but not
 * the source or destination lengths.
 *
 * \param dst       destination buffer, or `NULL`.
 * \param dst_len   destination buffer length (in characters).
 * \param src       source bytes (may be `NULL` if `src_len` is zero).
 * \param src_len   source length (in bytes).
 * \param flags     behavioural flags.
 * \return  the number of characters produced.
 */
size_t cttk_bintob64_gen(char *dst, size_t dst_len,
	const void *src, size_t src_len, unsigned flags);

/**
 * \brief Base64-encoding flag: do not add `'='` signs for padding.
 */
#define CTTK_B64ENC_NO_PAD     0x0001

/**
 * \brief Base64-encoding flag: break lines regularly.
 */
#define CTTK_B64ENC_NEWLINE    0x0002

/**
 * \brief Base64-encoding flag: use CR+LF instead of LF for line breaks.
 */
#define CTTK_B64ENC_CRLF       0x0004

/**
 * \brief Base64-encoding flag: break lines every 64 characters instead of 76.
 */
#define CTTK_B64ENC_LINE64     0x0008

/* ==================================================================== */

/**
 * \brief Multiplication unsigned 32x32 -> 32.
 *
 * Result is computed modulo 2^32 (no undefined behaviour on overflow).
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
uint32_t cttk_mulu32(uint32_t x, uint32_t y);

/**
 * \brief Multiplication signed 32x32 -> 32.
 *
 * This function guarantees two's complement behaviour with truncation
 * to the low 32 bits (no undefined behaviour on overflow/underflow).
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
int32_t cttk_muls32(int32_t x, int32_t y);

/**
 * \brief Multiplication unsigned 32x32 -> 64.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
uint64_t cttk_mulu32w(uint32_t x, uint32_t y);

/**
 * \brief Multiplication signed 32x32 -> 64.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
int64_t cttk_muls32w(int32_t x, int32_t y);

/**
 * \brief Multiplication unsigned 64x64 -> 64.
 *
 * Result is computed modulo 2^64 (no undefined behaviour on overflow).
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
uint64_t cttk_mulu64(uint64_t x, uint64_t y);

/**
 * \brief Multiplication signed 64x64 -> 64.
 *
 * This function guarantees two's complement behaviour with truncation
 * to the low 64 bits (no undefined behaviour on overflow/underflow).
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  the result.
 */
int64_t cttk_muls64(int64_t x, int64_t y);

/* ==================================================================== */
/*
 * Big integers (non-modular).
 */

#if CTTK_DOXYGEN_IGNORE

/**
 * \brief Define a big integer variable or field.
 *
 * This macro defines a local variable or a structure field for a big integer
 * of size at most `size` bits. `size` MUST NOT be zero. `size` MUST be a
 * constant expression. The actual size of the integer is not set;
 * `cti_init()` must be used for that.
 *
 * This macro may also be used in a `typedef` declaration:
 *
 *     typedef cti_def(my_int, 300);
 *
 * This example defines the type `my_int` to be a container for a big
 * integer of up to 300 bits.
 *
 * Big integer types are internally arrays. Therefore, when used as
 * function arguments, they are passed by reference: the called function
 * receives a pointer to the first array element, not a copy of the
 * value. This allows efficient handling of large integers, but requires
 * the caller to take care of allocation.
 *
 * The element type of these arrays is unspecified (see the `cti_elt`
 * type). CTTK may contain several implementations, and macros select
 * one of them, appropriate for the current architecture.
 *
 * \param name   variable or field name.
 * \param size   maximum size (in bits).
 */
#define cti_def(name, size)

/**
 * \brief Define a big integer variable and initialize it.
 *
 * This macro defines a local variable for a big integer of size _exactly_
 * `size` bits, and initializes it to NaN. `size` MUST NOT be zero. If
 * `size` is not a constant expression, then this yields a "variable-size
 * array" allocated in the stack; care should be taken to ensure that the
 * size is small enough to fit on the stack.
 *
 * \param name   variable or field name.
 * \param size   size (in bits).
 */
#define cti_definit(name, size)

/**
 * \brief Type for a big integer element.
 *
 * The `cti_elt` macro evaluates to the type of individual elements
 * in the array type that incarnates big integers. The purpose of this
 * macro is to allow declaration of function parameters that are big
 * integers without having to specify an explicit size.
 */
#define cti_elt

/**
 * \brief Initialize an integer structure.
 *
 * This function sets the provided big integer to a NaN of the
 * provided size (in bits). The big integer structure MUST have been
 * allocated with a sufficient size. `size` MUST NOT be zero.
 *
 * \param x      structure to initialize.
 * \param size   size (in bits).
 */
void cti_init(cti_elt *x, unsigned size);

/**
 * \brief Set a big integer to the provided value.
 *
 * If the source value is too large for the target big integer, then a
 * NaN is produced.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_u32(cti_elt *x, uint32_t v);

/**
 * \brief Set a big integer to the provided value with truncation.
 *
 * Truncation is applied if necessary to make the value fit. This may
 * result in a negative value.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_u32_trunc(cti_elt *x, uint32_t v);

/**
 * \brief Set a big integer to the provided value.
 *
 * If the source value is too large for the target big integer, then a
 * NaN is produced.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_u64(cti_elt *x, uint64_t v);

/**
 * \brief Set a big integer to the provided value with truncation.
 *
 * Truncation is applied if necessary to make the value fit. This may
 * result in a negative value.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_u64_trunc(cti_elt *x, uint64_t v);

/**
 * \brief Set a big integer to the provided value.
 *
 * If the source value is too large for the target big integer, then a
 * NaN is produced.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_s32(cti_elt *x, int32_t v);

/**
 * \brief Set a big integer to the provided value.
 *
 * If the source value is too large for the target big integer, then a
 * NaN is produced.
 *
 * \param x   structure to set.
 * \param v   the value.
 */
void cti_set_s64(cti_elt *x, int64_t v);

/**
 * \brief Size conversion.
 *
 * This function sets `d` to the value of `a`. Operands need not match
 * in size. If the source value is NaN, or is outside of the range of
 * representable values in `d`, then `d` is set to NaN.
 *
 * \param d   destination operand.
 * \param a   source operand.
 */
void cti_set(cti_elt *d, const cti_elt *a);

/**
 * \brief Size conversion (truncating).
 *
 * This function sets `d` to the value of `a`. Operands need not match
 * in size. If the source value is NaN, then `d` is set to NaN. If the
 * source value does not fit in the range of representable values in `d`,
 * then it is truncated.
 *
 * \param d   destination operand.
 * \param a   source operand.
 */
void cti_set_trunc(cti_elt *d, const cti_elt *a);

/**
 * \brief Test a value for being a NaN.
 *
 * \param x   value to test.
 * \return  true if NaN.
 */
cttk_bool cti_isnan(const cti_elt *x);

/**
 * \brief Convert back integer to a native 32-bit unsigned int.
 *
 * Returned value is the integer reduced modulo 2^32. If the big integer
 * is NaN, then the returned value is 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
uint32_t cti_to_u32_trunc(const cti_elt *x);

/**
 * \brief Convert back integer to a native 32-bit signed int.
 *
 * Returned value is the integer reduced modulo 2^32, in the -2^31 to
 * 2^31-1 range. If the big integer is NaN, then the returned value is 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
int32_t cti_to_s32_trunc(const cti_elt *x);

/**
 * \brief Convert back integer to a native 64-bit unsigned int.
 *
 * Returned value is the integer reduced modulo 2^64. If the big integer
 * is NaN, then the returned value is 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
uint64_t cti_to_u64_trunc(const cti_elt *x);

/**
 * \brief Convert back integer to a native 64-bit signed int.
 *
 * Returned value is the integer reduced modulo 2^64, in the -2^63 to
 * 2^63-1 range. If the big integer is NaN, then the returned value is 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
int64_t cti_to_s64_trunc(const cti_elt *x);

/**
 * \brief Convert back integer to a native 32-bit unsigned int.
 *
 * If the source operand `x` is NaN or out of range for the result, then
 * this function returns 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
uint32_t cti_to_u32(const cti_elt *x);

/**
 * \brief Convert back integer to a native 32-bit signed int.
 *
 * If the source operand `x` is NaN or out of range for the result, then
 * this function returns 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
int32_t cti_to_s32(const cti_elt *x);

/**
 * \brief Convert back integer to a native 64-bit unsigned int.
 *
 * If the source operand `x` is NaN or out of range for the result, then
 * this function returns 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
uint64_t cti_to_u64(const cti_elt *x);

/**
 * \brief Convert back integer to a native 64-bit signed int.
 *
 * If the source operand `x` is NaN or out of range for the result, then
 * this function returns 0.
 *
 * \param x   value to convert.
 * \return  the converted value.
 */
int64_t cti_to_s64(const cti_elt *x);

/**
 * \brief Decode a big integer from bytes (big-endian, signed).
 *
 * The most significant bit of the first byte is used as sign bit. If
 * the value is out of range for the result, then `x` is set to NaN.
 * If the source array is empty, then a NaN is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decbe_signed(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (big-endian, unsigned).
 *
 * The source value is interpreted as an unsigned integer. If
 * the value is out of range for the result, then `x` is set to NaN.
 * If the source array is empty, then 0 is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decbe_unsigned(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (big-endian, signed).
 *
 * The most significant bit of the first byte is used as sign bit. If
 * the value is out of range for the result, then it is truncated to
 * the recipient size. If the source array is empty, then a NaN is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decbe_signed_trunc(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (big-endian, unsigned).
 *
 * The source value is interpreted as an unsigned integer. If
 * the value is out of range for the result, then it is truncated to
 * the recipient size. If the source array is empty, then 0 is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decbe_unsigned_trunc(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (little-endian, signed).
 *
 * The most significant bit of the last byte is used as sign bit. If
 * the value is out of range for the result, then `x` is set to NaN.
 * If the source array is empty, then a NaN is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decle_signed(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (little-endian, unsigned).
 *
 * The source value is interpreted as an unsigned integer. If
 * the value is out of range for the result, then `x` is set to NaN.
 * If the source array is empty, then 0 is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decle_unsigned(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (little-endian, signed).
 *
 * The most significant bit of the last byte is used as sign bit. If
 * the value is out of range for the result, then it is truncated to
 * the recipient size. If the source array is empty, then a NaN is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decle_signed_trunc(cti_elt *x, const void *src, size_t len);

/**
 * \brief Decode a big integer from bytes (little-endian, unsigned).
 *
 * The source value is interpreted as an unsigned integer. If
 * the value is out of range for the result, then it is truncated to
 * the recipient size. If the source array is empty, then 0 is obtained.
 *
 * \param x     result recipient.
 * \param src   source byte pointer.
 * \param len   source length (in bytes).
 */
void cti_decle_unsigned_trunc(cti_elt *x, const void *src, size_t len);

/**
 * \brief Encode a big integer into bytes (big-endian).
 *
 * The provided bytes are filled with a big-endian encoding of the value
 * held in `x`. Value is sign-extended or truncated as needed to fit the
 * destination buffer. If `x` is NaN, then the buffer is filled with zeros.
 *
 * \param dst   destination buffer.
 * \param len   destination buffer length (in bytes).
 * \param x     source value.
 */
void cti_encbe(void *dst, size_t len, const cti_elt *x);

/**
 * \brief Encode a big integer into bytes (little-endian).
 *
 * The provided bytes are filled with a little-endian encoding of the value
 * held in `x`. Value is sign-extended or truncated as needed to fit the
 * destination buffer. If `x` is NaN, then the buffer is filled with zeros.
 *
 * \param dst   destination buffer.
 * \param len   destination buffer length (in bytes).
 * \param x     source value.
 */
void cti_encle(void *dst, size_t len, const cti_elt *x);

/**
 * \brief Test a value for being zero.
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if zero.
 */
cttk_bool cti_eq0(const cti_elt *x);

/**
 * \brief Test a value for being distinct from zero.
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if non-zero.
 */
cttk_bool cti_neq0(const cti_elt *x);

/**
 * \brief Test a value for being positive (greater than zero).
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if positive.
 */
cttk_bool cti_gt0(const cti_elt *x);

/**
 * \brief Test a value for being negative (lower than zero).
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if negative.
 */
cttk_bool cti_lt0(const cti_elt *x);

/**
 * \brief Test a value for being nonnegative (greater than or equal to
 * zero).
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if nonnegative.
 */
cttk_bool cti_geq0(const cti_elt *x);

/**
 * \brief Test a value for being nonpositive (lower than or equal to
 * zero).
 *
 * If the value `x` is NaN, then this returns false.
 *
 * \param x   value to test.
 * \return  true if nonpositive.
 */
cttk_bool cti_leq0(const cti_elt *x);

/**
 * \brief Compare two integers (equality).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is equal to `y`.
 */
cttk_bool cti_eq(const cti_elt *x, const cti_elt *y);

/**
 * \brief Compare two integers (inequality).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is not equal to `y`.
 */
cttk_bool cti_neq(const cti_elt *x, const cti_elt *y);

/**
 * \brief Compare two integers (lower-than).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is lower than `y`.
 */
cttk_bool cti_lt(const cti_elt *x, const cti_elt *y);

/**
 * \brief Compare two integers (lower-or-equal).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is lower than or equal to `y`.
 */
cttk_bool cti_leq(const cti_elt *x, const cti_elt *y);

/**
 * \brief Compare two integers (greater-than).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is greater than `y`.
 */
cttk_bool cti_gt(const cti_elt *x, const cti_elt *y);

/**
 * \brief Compare two integers (greater-or-equal).
 *
 * If `x` and/or `y` is NaN, or `x` and `y` do not have the same
 * storage size, then this function returns false.
 *
 * \param x   first operand.
 * \param y   second operand.
 * \return  true if `x` is greater than or equal to `y`.
 */
cttk_bool cti_geq(const cti_elt *x, const cti_elt *y);

/**
 * \brief Get the sign of an integer.
 *
 * This function returns -1, 0 or 1, depending on whether the value is
 * negative, zero or positive. If `x` is a NaN, then 0 is returned.
 *
 * CAUTION: returned value is amenable to non-constant-time operations
 * such as conditional jumps, which would leak the value result.
 *
 * \param x   value to test.
 * \return  the sign of `x`.
 */
int cti_sign(const cti_elt *x);

/**
 * \brief Compare two integers.
 *
 * Returned value is -1, 0 or 1, depending on whether `x` is lower than,
 * equal to, or greater than `y`. If `x` and/or `y` is NaN, then 0 is
 * returned.
 *
 * CAUTION: returned value is amenable to non-constant-time operations
 * such as conditional jumps, which would leak the value result.
 *
 * \param x   first operand.
 * \param y   first operand.
 * \return  -1, 0 or 1.
 */
int32_t cti_cmp(const cti_elt *x, const cti_elt *y);

/**
 * \brief Copy an integer.
 *
 * If `d` (destination) and `s` (source) do not have the same size, then
 * `d` is set to NaN. Otherwise, the value of `s` is copied into `d`.
 * Source and destination may be the same array.
 *
 * \param d   destination.
 * \param s   source.
 */
void cti_copy(cti_elt *d, const cti_elt *s);

/**
 * \brief Conditionally copy an integer.
 *
 * If `d` (destination) and `s` (source) do not have the same size, then
 * `d` is set to NaN. Otherwise, if `ctl` is true, then the value of `s`
 * is copied into `d`; if `ctl` is false, `d` keeps its current value.
 * Source and destination may be the same array.
 *
 * \param ctl   copy control flag.
 * \param d     destination.
 * \param s     source.
 */
void cti_cond_copy(cttk_bool ctl, cti_elt *d, const cti_elt *s);

/**
 * \brief Swap two integers.
 *
 * If `a` and `b` do not have the same size, then both are set to NaN.
 * Otherwise, the values of `a` and `b` are exchanged.
 * The two operands may be the same array.
 *
 * \param a   first value.
 * \param b   second value.
 */
void cti_swap(cti_elt *a, cti_elt *b);

/**
 * \brief Conditionally swap two integers.
 *
 * If `a` and `b` do not have the same size, then both are set to NaN.
 * Otherwise, if `ctl` is true, then the values of `a` and `b` are
 * exchanged; if `ctl` is false, then `a` and `b` keep their current values.
 * The two operands may be the same array.
 *
 * \param ctl   copy control flag.
 * \param a     first value.
 * \param b     second value.
 */
void cti_cond_swap(cttk_bool ctl, cti_elt *a, cti_elt *b);

/**
 * \brief Copy one of two integers.
 *
 * If `a`, `b` and `d` do not all share the same size, then `d` is set
 * to NaN. Otherwise, if `ctl` is true, then the value of `a` is
 * copied into `d`; if `ctl` is false, the value of `b` is copied into `d`.
 * Operands need not be distinct.
 *
 * \param ctl   control flag.
 * \param d     destination.
 * \param a     first source operand (copied if `ctl` is true).
 * \param b     second source operand (copied if `ctl` is false).
 */
void cti_mux(cttk_bool ctl, cti_elt *d,
	const cti_elt *a, const cti_elt *b);

/**
 * \brief Addition of two big integers.
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, or the result overflows/underflows, then the result is set
 * to NaN. Operands (both source and destination) need not be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_add(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Addition of two big integers (truncating).
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, then the result is set to NaN. On overflow or underflow, the
 * result is truncated. Operands (both source and destination) need not
 * be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_add_trunc(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Subtraction of two big integers.
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, or the result overflows/underflows, then the result is set
 * to NaN. Operands (both source and destination) need not be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_sub(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Subtraction of two big integers (truncating).
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, then the result is set to NaN. On overflow or underflow, the
 * result is truncated. Operands (both source and destination) need not
 * be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_sub_trunc(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Negation of a big integer.
 *
 * If the source and destination operands to not match in size, or
 * the source operand is a NaN, or the result overflows (when the
 * source is the minimum representable value), then the result is
 * set to NaN. Source and destination operands need not be distinct.
 *
 * \param d   result recipient.
 * \param x   source operand.
 */
void cti_neg(cti_elt *d, const cti_elt *x);

/**
 * \brief Negation of a big integer (truncating).
 *
 * If the source and destination operands to not match in size, or
 * the source operand is a NaN, then the result is set to NaN.
 * If the source value is the minimum representable value, then it
 * is kept unchanged (this is truncating behaviour on overflow).
 * Source and destination operands need not be distinct.
 *
 * \param d   result recipient.
 * \param x   source operand.
 */
void cti_neg_trunc(cti_elt *d, const cti_elt *x);

/**
 * \brief Multiplication of two big integers.
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, or the result overflows/underflows, then the result is set
 * to NaN. Operands (both source and destination) need not be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_mul(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Multiplication of two big integers (truncating).
 *
 * If the operands do not match in size, or one of the source operands
 * is NaN, then the result is set to NaN. On overflow or underflow, the
 * result is truncated. Operands (both source and destination) need not
 * be distinct.
 *
 * \param d   result recipient.
 * \param a   first source operand.
 * \param b   second source operand.
 */
void cti_mul_trunc(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Big integer left-shift.
 *
 * This function computes a left shift of `a` by `n` bits, which is
 * equivalent to multiplication by 2 raised to the power `n`. Left-shifting
 * a negative value is permitted. If the mathematical result is not
 * representable in the operand range, then NaN is produced.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects the shifted value and the
 * result, but NOT the shift count. The shift count may leak. Consider
 * using `cti_lsh_prot()` to have a constant-time shift with a
 * protected shift count.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_lsh(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Big integer left-shift (protected).
 *
 * This function computes a left shift of `a` by `n` bits, which is
 * equivalent to multiplication by 2 raised to the power `n`. Left-shifting
 * a negative value is permitted. If the mathematical result is not
 * representable in the operand range, then NaN is produced.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects not only the shifted value and
 * the result, but also the shift count. The shift count must fit on
 * 32 bits. This function is less efficient than `cti_lsh()`.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_lsh_prot(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Big integer left-shift (truncating).
 *
 * This function computes a left shift of `a` by `n` bits, which is
 * equivalent to multiplication by 2 raised to the power `n`. Left-shifting
 * a negative value is permitted. If the mathematical result is not
 * representable in the operand range, then it is truncated to the
 * type size.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects the shifted value and the
 * result, but NOT the shift count. The shift count may leak. Consider
 * using `cti_lsh_trunc_prot()` to have a constant-time shift with a
 * protected shift count.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_lsh_trunc(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Big integer left-shift (truncating, protected).
 *
 * This function computes a left shift of `a` by `n` bits, which is
 * equivalent to multiplication by 2 raised to the power `n`. Left-shifting
 * a negative value is permitted. If the mathematical result is not
 * representable in the operand range, then it is truncated to the
 * type size.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects not only the shifted value and
 * the result, but also the shift count. The shift count must fit on
 * 32 bits. This function is less efficient than `cti_lsh_trunc()`.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_lsh_trunc_prot(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Big integer right-shift.
 *
 * This function computes an arithmetic right shift of `a` by `n` bits.
 * If the source value is negative, then the sign bit is extended as
 * appropriate. Note that overflows/underflows are not possible, hence
 * there is no "truncating" version of this function.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects the shifted value and the
 * result, but NOT the shift count. The shift count may leak. Consider
 * using `cti_rsh_prot()` to have a constant-time shift with a
 * protected shift count.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_rsh(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Big integer right-shift (protected).
 *
 * This function computes an arithmetic right shift of `a` by `n` bits.
 * If the source value is negative, then the sign bit is extended as
 * appropriate. Note that overflows/underflows are not possible, hence
 * there is no "truncating" version of this function.
 *
 * If source and destination operands do not have the same size, or if
 * source is a NaN, then a NaN is produced.
 *
 * Source and destination operands may be identical.
 *
 * Constant-time: this function protects not only the shifted value and
 * the result, but also the shift count. The shift count must fit on
 * 32 bits. This function is less efficient than `cti_rsh()`.
 *
 * \param d   recipient for the shifted value.
 * \param a   source value to shift.
 * \param n   shift count.
 */
void cti_rsh_prot(cti_elt *d, const cti_elt *a, uint32_t n);

/**
 * \brief Division of two big integers.
 *
 * The quotient and remainder of the division of `a` by `b` are
 * written in `q` and `r`, respectively. Either `q` or `r` may be `NULL`,
 * in which case the corresponding value is not produced. If `q` and
 * `r` are both distinct from `NULL`, then they MUST point to distinct
 * arrays; apart from this requirement, operands needs not be distinct
 * from each other.
 *
 * The division follows the "truncation towards zero" rules:
 *
 *   - In all cases, `a = q*b + r`.
 *   - `|a| = |b|*|q| + |r|` with `0 <= |r| < |b|`.
 *   - The remainder `r` has the sign of the dividend `a`.
 *   - The quotient `q` is negative (or zero) if `a` and `b` have
 *     opposite signs, positive (or zero) if `a` and `b` have the
 *     same sign.
 *
 * These rules match the `/` and `%` operators in C.
 *
 * If the operands to not match in size, or one of the source operands
 * is NaN, or the divisor is zero, then both quotients and remainder are
 * set to NaN. If the dividend is equal to the minimum representable
 * value and the divisor is minus one, then the quotient is set to NaN
 * (it cannot be represented), but the remainder is set to zero. In all
 * other cases, both quotient and remainder are defined and representable.
 *
 * \param q   recipient for the quotient (or `NULL`).
 * \param r   recipient for the remainer (or `NULL`).
 * \param a   dividend.
 * \param b   divisor.
 */
void cti_divrem(cti_elt *q, cti_elt *r,
	const cti_elt *a, const cti_elt *b);

/**
 * \brief Division of two integers (quotient-only).
 *
 * This function simply calls `cti_divrem()` with a remainder set
 * to `NULL`.
 *
 * \param q   recipient for the quotient.
 * \param a   dividend.
 * \param b   divisor.
 */
static inline void
cti_div(cti_elt *q, const cti_elt *a, const cti_elt *b)
{
	cti_divrem(q, NULL, a, b);
}

/**
 * \brief Division of two integers (remainder-only).
 *
 * This function simply calls `cti_divrem()` with a quotient set
 * to `NULL`.
 *
 * \param r   recipient for the remainder.
 * \param a   dividend.
 * \param b   divisor.
 */
static inline void
cti_rem(cti_elt *r, const cti_elt *a, const cti_elt *b)
{
	cti_divrem(NULL, r, a, b);
}

/**
 * \brief Modular reduction.
 *
 * This function reduces `a` modulo `b`. The result is written in `m`.
 * Contrary to the remainder computed from `cti_divrem()`, the
 * result of this function is always nonnegative; i.e. the obtained
 * value for `m` is greater than or equal to zero, and lower than
 * the absolute value of `b`.
 *
 * If the operands do not match in size, or one of the source operands is NaN,
 * or the divisor is zero, then the result is set to NaN. By definition,
 * overflows/underflows are not possible. Operands need not be distinct.
 *
 * \param m   recipient for the modular reduction result.
 * \param a   dividend.
 * \param b   divisor.
 */
void cti_mod(cti_elt *m, const cti_elt *a, const cti_elt *b);

/**
 * \brief Bitwise AND.
 *
 * This function computes the bitwise AND of `a` and `b`; result is
 * written in `d`. Since two's complement is used, the result is
 * negative only if both source operands are negative.
 *
 * If the operands do not match in size, or one of the source operands is NaN,
 * then the result is set to NaN. By definition, overflows/underflows are
 * not possible. Operands need not be distinct.
 *
 * \param d   recipient for the bitwise boolean operation.
 * \param a   first operand.
 * \param b   second operand.
 */
void cti_and(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Bitwise OR.
 *
 * This function computes the bitwise OR of `a` and `b`; result is
 * written in `d`. Since two's complement is used, the result is
 * nonnegative only if both source operands are nonnegative.
 *
 * If the operands do not match in size, or one of the source operands is NaN,
 * then the result is set to NaN. By definition, overflows/underflows are
 * not possible. Operands need not be distinct.
 *
 * \param d   recipient for the bitwise boolean operation.
 * \param a   first operand.
 * \param b   second operand.
 */
void cti_or(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Bitwise XOR.
 *
 * This function computes the bitwise XOR of `a` and `b`; result is
 * written in `d`. Since two's complement is used, the result is
 * negative only if exactly one of the operands is negative.
 *
 * If the operands do not match in size, or one of the source operands is NaN,
 * then the result is set to NaN. By definition, overflows/underflows are
 * not possible. Operands need not be distinct.
 *
 * \param d   recipient for the bitwise boolean operation.
 * \param a   first operand.
 * \param b   second operand.
 */
void cti_xor(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Bitwise EQV (also known as XORNOT).
 *
 * This function computes the bitwise EQV of `a` and `b`; result is
 * written in `d`. EQV sets each output bit to 1 if the two corresponding
 * input bits are equal, to 0 otherwise.
 *
 * If the operands do not match in size, or one of the source operands is NaN,
 * then the result is set to NaN. By definition, overflows/underflows are
 * not possible. Operands need not be distinct.
 *
 * \param d   recipient for the bitwise boolean operation.
 * \param a   first operand.
 * \param b   second operand.
 */
void cti_eqv(cti_elt *d, const cti_elt *a, const cti_elt *b);

/**
 * \brief Bitwise NOT.
 *
 * This function computes the bitwise complement of `a`; result is
 * written in `d`. Each bit in `d` is the opposite of the corresponding
 * bit in `a`; this is also known as two's complement.
 *
 * If the operands do not match in size, or the source operand is NaN,
 * then the result is set to NaN. By definition, overflows/underflows are
 * not possible. Operands need not be distinct.
 *
 * \param d   recipient for the bitwise boolean operation.
 * \param a   operand.
 */
void cti_not(cti_elt *d, const cti_elt *a);

#else

/*
 * Default big integer implementation is "i31".
 */
#define cti_def                    cttk_i31_def
#define cti_definit                cttk_i31_definit
#define cti_elt                    cttk_i31_elt
#define cti_init                   cttk_i31_init
#define cti_set_u32                cttk_i31_set_u32
#define cti_set_u32_trunc          cttk_i31_set_u32_trunc
#define cti_set_u64                cttk_i31_set_u64
#define cti_set_u64_trunc          cttk_i31_set_u64_trunc
#define cti_set_s32                cttk_i31_set_s32
#define cti_set_s64                cttk_i31_set_s64
#define cti_set                    cttk_i31_set
#define cti_set_trunc              cttk_i31_set_trunc
#define cti_isnan                  cttk_i31_isnan
#define cti_to_u32_trunc           cttk_i31_to_u32_trunc
#define cti_to_s32_trunc           cttk_i31_to_s32_trunc
#define cti_to_u64_trunc           cttk_i31_to_u64_trunc
#define cti_to_s64_trunc           cttk_i31_to_s64_trunc
#define cti_to_u32                 cttk_i31_to_u32
#define cti_to_s32                 cttk_i31_to_s32
#define cti_to_u64                 cttk_i31_to_u64
#define cti_to_s64                 cttk_i31_to_s64
#define cti_decbe_signed           cttk_i31_decbe_signed
#define cti_decbe_unsigned         cttk_i31_decbe_unsigned
#define cti_decbe_signed_trunc     cttk_i31_decbe_signed_trunc
#define cti_decbe_unsigned_trunc   cttk_i31_decbe_unsigned_trunc
#define cti_decle_signed           cttk_i31_decle_signed
#define cti_decle_unsigned         cttk_i31_decle_unsigned
#define cti_decle_signed_trunc     cttk_i31_decle_signed_trunc
#define cti_decle_unsigned_trunc   cttk_i31_decle_unsigned_trunc
#define cti_encbe                  cttk_i31_encbe
#define cti_encle                  cttk_i31_encle
#define cti_eq0                    cttk_i31_eq0
#define cti_neq0                   cttk_i31_neq0
#define cti_gt0                    cttk_i31_gt0
#define cti_lt0                    cttk_i31_lt0
#define cti_geq0                   cttk_i31_geq0
#define cti_leq0                   cttk_i31_leq0
#define cti_eq                     cttk_i31_eq
#define cti_neq                    cttk_i31_neq
#define cti_lt                     cttk_i31_lt
#define cti_leq                    cttk_i31_leq
#define cti_gt                     cttk_i31_gt
#define cti_geq                    cttk_i31_geq
#define cti_sign                   cttk_i31_sign
#define cti_cmp                    cttk_i31_cmp
#define cti_copy                   cttk_i31_copy
#define cti_cond_copy              cttk_i31_cond_copy
#define cti_swap                   cttk_i31_swap
#define cti_cond_swap              cttk_i31_cond_swap
#define cti_mux                    cttk_i31_mux
#define cti_add                    cttk_i31_add
#define cti_add_trunc              cttk_i31_add_trunc
#define cti_sub                    cttk_i31_sub
#define cti_sub_trunc              cttk_i31_sub_trunc
#define cti_neg                    cttk_i31_neg
#define cti_neg_trunc              cttk_i31_neg_trunc
#define cti_mul                    cttk_i31_mul
#define cti_mul_trunc              cttk_i31_mul_trunc
#define cti_lsh                    cttk_i31_lsh
#define cti_lsh_prot               cttk_i31_lsh_prot
#define cti_lsh_trunc              cttk_i31_lsh_trunc
#define cti_lsh_trunc_prot         cttk_i31_lsh_trunc_prot
#define cti_rsh                    cttk_i31_rsh
#define cti_rsh_prot               cttk_i31_rsh_prot
#define cti_divrem                 cttk_i31_divrem
#define cti_div                    cttk_i31_div
#define cti_rem                    cttk_i31_rem
#define cti_mod                    cttk_i31_mod
#define cti_and                    cttk_i31_and
#define cti_or                     cttk_i31_or
#define cti_xor                    cttk_i31_xor
#define cti_eqv                    cttk_i31_eqv
#define cti_not                    cttk_i31_not

/*
 * The "i31" implementation of big integer represents values as
 * arrays of 32bit words (uint32_t). The first array encodes the
 * integer size, and contains the "NaN flag". Subsequent arrays
 * encode the value, with 31 bits per word (the top bit is kept
 * equal to 0 at all times, even for a NaN).
 */

#define cttk_i31_def(name, size)       uint32_t name[((size) + 61) / 31]
#define cttk_i31_definit(name, size)   cttk_i31_def(name, size) = { ((size) + ((size) / 31)) + 0x80000000 }
#define cttk_i31_elt   uint32_t
void cttk_i31_init(uint32_t *x, unsigned size);
void cttk_i31_set_u32(uint32_t *x, uint32_t v);
void cttk_i31_set_u32_trunc(uint32_t *x, uint32_t v);
void cttk_i31_set_u64(uint32_t *x, uint64_t v);
void cttk_i31_set_u64_trunc(uint32_t *x, uint64_t v);
void cttk_i31_set_s32(uint32_t *x, int32_t v);
void cttk_i31_set_s64(uint32_t *x, int64_t v);
void cttk_i31_set(uint32_t *d, const uint32_t *a);
void cttk_i31_set_trunc(uint32_t *d, const uint32_t *a);
static inline cttk_bool
cttk_i31_isnan(const uint32_t *x)
{
	return cttk_bool_of_u32(x[0] >> 31);
}
uint32_t cttk_i31_to_u32_trunc(const uint32_t *x);
int32_t cttk_i31_to_s32_trunc(const uint32_t *x);
uint64_t cttk_i31_to_u64_trunc(const uint32_t *x);
int64_t cttk_i31_to_s64_trunc(const uint32_t *x);
uint32_t cttk_i31_to_u32(const uint32_t *x);
int32_t cttk_i31_to_s32(const uint32_t *x);
uint64_t cttk_i31_to_u64(const uint32_t *x);
int64_t cttk_i31_to_s64(const uint32_t *x);
void cttk_i31_decbe_signed(uint32_t *x, const void *src, size_t len);
void cttk_i31_decbe_unsigned(uint32_t *x, const void *src, size_t len);
void cttk_i31_decbe_signed_trunc(uint32_t *x, const void *src, size_t len);
void cttk_i31_decbe_unsigned_trunc(uint32_t *x, const void *src, size_t len);
void cttk_i31_decle_signed(uint32_t *x, const void *src, size_t len);
void cttk_i31_decle_unsigned(uint32_t *x, const void *src, size_t len);
void cttk_i31_decle_signed_trunc(uint32_t *x, const void *src, size_t len);
void cttk_i31_decle_unsigned_trunc(uint32_t *x, const void *src, size_t len);
void cttk_i31_encbe(void *dst, size_t len, const uint32_t *x);
void cttk_i31_encle(void *dst, size_t len, const uint32_t *x);
cttk_bool cttk_i31_eq0(const uint32_t *x);
cttk_bool cttk_i31_neq0(const uint32_t *x);
cttk_bool cttk_i31_gt0(const uint32_t *x);
cttk_bool cttk_i31_lt0(const uint32_t *x);
cttk_bool cttk_i31_geq0(const uint32_t *x);
cttk_bool cttk_i31_leq0(const uint32_t *x);
cttk_bool cttk_i31_eq(const uint32_t *x, const uint32_t *y);
cttk_bool cttk_i31_neq(const uint32_t *x, const uint32_t *y);
cttk_bool cttk_i31_lt(const uint32_t *x, const uint32_t *y);
cttk_bool cttk_i31_leq(const uint32_t *x, const uint32_t *y);
cttk_bool cttk_i31_gt(const uint32_t *x, const uint32_t *y);
cttk_bool cttk_i31_geq(const uint32_t *x, const uint32_t *y);
int cttk_i31_sign(const uint32_t *x);
int32_t cttk_i31_cmp(const uint32_t *x, const uint32_t *y);
void cttk_i31_copy(uint32_t *d, const uint32_t *s);
void cttk_i31_cond_copy(cttk_bool ctl, uint32_t *d, const uint32_t *s);
void cttk_i31_swap(uint32_t *a, uint32_t *b);
void cttk_i31_cond_swap(cttk_bool ctl, uint32_t *a, uint32_t *b);
void cttk_i31_mux(cttk_bool ctl, uint32_t *d,
	const uint32_t *a, const uint32_t *b);
void cttk_i31_add(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_add_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_sub(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_sub_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_neg(uint32_t *d, const uint32_t *x);
void cttk_i31_neg_trunc(uint32_t *d, const uint32_t *x);
void cttk_i31_mul(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_mul_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_lsh(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_lsh_prot(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_lsh_trunc(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_lsh_trunc_prot(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_rsh(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_rsh_prot(uint32_t *d, const uint32_t *a, uint32_t n);
void cttk_i31_divrem(uint32_t *q, uint32_t *r,
	const uint32_t *a, const uint32_t *b);
static inline void
cttk_i31_div(uint32_t *q, const uint32_t *a, const uint32_t *b)
{
	cttk_i31_divrem(q, NULL, a, b);
}
static inline void
cttk_i31_rem(uint32_t *r, const uint32_t *a, const uint32_t *b)
{
	cttk_i31_divrem(NULL, r, a, b);
}
void cttk_i31_mod(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_and(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_or(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_xor(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_eqv(uint32_t *d, const uint32_t *a, const uint32_t *b);
void cttk_i31_not(uint32_t *d, const uint32_t *a);

#endif

/* ==================================================================== */

#ifdef __cplusplus
}
#endif

#endif
