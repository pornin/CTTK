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

#ifndef INNER_H__
#define INNER_H__

#include <string.h>
#include <limits.h>

#include "config.h"
#include "cttk.h"

/* ==================================================================== */

#if !CTTK_NO_MALLOC
#include <stdlib.h>
#endif

/*
 * We allow stack-based temporaries to use up to 4 kB of stack. With the i31
 * implementation, this is good for one integer up to 31713 bits.
 */
#if !defined CTTK_MAX_INT_BUF
#define CTTK_MAX_INT_BUF   4096
#endif

#ifdef CTTK_CTMUL
#ifndef CTTK_CTMUL32
#define CTTK_CTMUL32     CTTK_CTMUL
#endif
#ifndef CTTK_CTMULU32W
#define CTTK_CTMULU32W   CTTK_CTMUL
#endif
#ifndef CTTK_CTMULS32W
#define CTTK_CTMULS32W   CTTK_CTMUL
#endif
#ifndef CTTK_CTMUL64
#define CTTK_CTMUL64     CTTK_CTMUL
#endif
#endif

/* ==================================================================== */

#if CTTK_CTMUL32
#define mulu32(x, y)   ((uint32_t)((uint32_t)(x) * (uint32_t)(y)))
#else
static inline uint32_t
mulu32(uint32_t x, uint32_t y)
{
	int i;
	uint32_t z;

	z = 0;
	for (i = 0; i < 32; i ++) {
		z += x & -(y & 1);
		x <<= 1;
		y >>= 1;
	}
	return z;
}
#endif

static inline int32_t
muls32(int32_t x, int32_t y)
{
	uint32_t r;

	r = mulu32((uint32_t)x, (uint32_t)y);
	return *(int32_t *)&r;
}

#if CTTK_CTMULU32W
#define mulu32w(x, y)   ((uint64_t)((uint64_t)(uint32_t)(x) \
                        * (uint64_t)(uint32_t)(y)))
#else
static inline uint64_t
mulu32w(uint32_t x, uint32_t y)
{
	int i;
	uint64_t xe, z;

	z = 0;
	xe = x;
	for (i = 0; i < 32; i ++) {
		z += xe & -(uint64_t)(y & 1);
		xe <<= 1;
		y >>= 1;
	}
	return z;
}
#endif

#if CTTK_CTMULS32W
#define muls32w(x, y)   ((int64_t)((int64_t)(int32_t)(x) \
                        * (int64_t)(int32_t)(y)))
#else
static inline int64_t
muls32w(int32_t x, int32_t y)
{
	uint32_t xu, yu, xh, yh;
	uint64_t z;

	xu = (uint32_t)x;
	yu = (uint32_t)y;
	xh = xu >> 31;
	yh = yu >> 31;
	xu &= 0x7FFFFFFF;
	yu &= 0x7FFFFFFF;
	z = mulu32w(xu, yu);
	z -= ((uint64_t)(xu & -yh) + (uint64_t)(yu & -xh)) << 31;
	z += (uint64_t)(xh & yh) << 62;
	return *(int64_t *)&z;
}
#endif

#if CTTK_CTMUL64
#define mulu64(x, y)   ((uint64_t)((uint64_t)(x) * (uint64_t)(y)))
#else
static inline uint64_t
mulu64(uint64_t x, uint64_t y)
{
	uint64_t z;
	int i;

	z = 0;
	for (i = 0; i < 64; i ++) {
		z += x & -(y & 1);
		x <<= 1;
		y >>= 1;
	}
	return z;
}
#endif

static inline int64_t
muls64(int64_t x, int64_t y)
{
	uint64_t r;

	r = mulu64((uint64_t)x, (uint64_t)y);
	return *(int64_t *)&r;
}

/* ==================================================================== */

#endif
