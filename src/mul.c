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

#include "inner.h"

/* see cttk.h */
uint32_t
cttk_mulu32(uint32_t x, uint32_t y)
{
	return mulu32(x, y);
}

/* see cttk.h */
int32_t
cttk_muls32(int32_t x, int32_t y)
{
	return muls32(x, y);
}

/* see cttk.h */
uint64_t
cttk_mulu32w(uint32_t x, uint32_t y)
{
	return mulu32w(x, y);
}

/* see cttk.h */
int64_t
cttk_muls32w(int32_t x, int32_t y)
{
	return muls32w(x, y);
}

/* see cttk.h */
uint64_t
cttk_mulu64(uint64_t x, uint64_t y)
{
	return mulu64(x, y);
}

/* see cttk.h */
int64_t
cttk_muls64(int64_t x, int64_t y)
{
	return muls64(x, y);
}
