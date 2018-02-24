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

/*
 * Convert a character to its numerical value (0 to 63). This returns
 * -1 if the character is not a Base64 character.
 */
static int
b64val(char c)
{
	uint32_t x, y, z, r;

	x = (uint32_t)c - 0x41;
	y = (uint32_t)c - 0x61;
	z = (uint32_t)c - 0x30;
	r = ((x + 1) & -cttk_u32_lt(x, 26).v)
		| ((y + 27) & -cttk_u32_lt(y, 26).v)
		| ((z + 53) & -cttk_u32_lt(z, 10).v)
		| (63 & -cttk_u32_eq((uint32_t)c, 0x2B).v)
		| (64 & -cttk_u32_eq((uint32_t)c, 0x2F).v);
	return r - 1;
}

/*
 * Get the Base64 character corresponding to the provided value (0 to 63).
 */
static char
tob64(int d)
{
	uint32_t x, r;

	x = (uint32_t)d;
	r = (0x41 + x) & -cttk_u32_lt(x, 26).v;
	x -= 26;
	r |= (0x61 + x) & -cttk_u32_lt(x, 26).v;
	x -= 26;
	r |= (0x30 + x) & -cttk_u32_lt(x, 10).v;
	r |= 0x2B & -cttk_u32_eq(x, 10).v;
	r |= 0x2F & -cttk_u32_eq(x, 11).v;
	return r;
}

/* see cttk.h */
size_t
cttk_b64tobin_gen(void *dst, size_t dst_len,
	const char *src, size_t src_len, const char **err, unsigned flags)
{
	FIXME
}

/* see cttk.h */
size_t
cttk_bintob64_gen(char *dst, size_t dst_len,
	const void *src, size_t src_len, unsigned flags)
{
	if (dst == NULL) {
		size_t len;

		if ((flags & CTTK_B64ENC_NEWLINE) != 0) {
			size_t llen, nlen;

			if ((flags & CTTK_B64ENC_LINE64) != 0) {
				llen = 48;
			} else {
				llen = 57;
			}
			len = (src_len + llen - 1) / llen;
			if ((flags & CTTK_B64ENC_CRLF) != 0) {
				len <<= 1;
			}
		} else {
			len = 0;
		}
		if ((flags & CTTK_B64ENC_NO_PAD) != 0) {
			len += ((src_len / 3) << 2);
			switch (src_len % 3) {
			case 1: len += 2; break;
			case 2: len += 3; break;
			}
		} else {
			len += ((src_len + 2) / 3) << 2;
		}
		return len;
	}

	FIXME
}
