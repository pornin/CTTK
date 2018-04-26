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
int
cttk_hexval(char c)
{
	/*
	 * Code below assumes an ASCII compatible charset.
	 *
	 * Decimal digits are 0x30 to 0x39.
	 * Uppercase letters 'A' to 'F' are 0x41 to 0x46.
	 * Lowercase letters 'a' to 'f' are 0x61 to 0x66.
	 *
	 * Only one of the three clauses (at most) will yield a non-zero
	 * value; we increment it by 1 to get a value from 1 to 16, or 0
	 * if the source character is not an hexadecimal digit. A final
	 * subtraction ensures that we get -1 on an invalid source digit.
	 */
	uint32_t x, y, z, r;

	x = (uint32_t)c - 0x30;
	y = (uint32_t)c - 0x41;
	z = (uint32_t)c - 0x61;
	r = ((x + 1) & -cttk_u32_lt(x, 10).v)
		| ((y + 11) & -cttk_u32_lt(y, 6).v)
		| ((z + 11) & -cttk_u32_lt(z, 6).v);
	return (int)r - 1;
}

/* see cttk.h */
char
cttk_hexdigit(int x, int uppercase)
{
	unsigned off;

	/*
	 * 'off' is the offset to add to the character value if going
	 * beyond '9' (0x39) so that a letter is obtained ('A' is 0x41,
	 * 'a' is 0x61).
	 */
	off = (0x41 - 0x3A);
	if (!uppercase) {
		off += 0x20;
	}

	/*
	 * If x >= 10, then the subtraction will yield upper bits of
	 * value 0.
	 */
	return (char)(0x30 + x + (off & ~(((unsigned)x - 10) >> 8)));
}

/* see cttk.h */
size_t
cttk_hexscan(const char *src, size_t src_len, int skipws)
{
	size_t u;

	for (u = 0; u < src_len; u ++) {
		int c;

		c = src[u];
		if (skipws && c <= 0x20) {
			continue;
		}
		if (cttk_hexval(c) < 0) {
			break;
		}
	}
	return u;
}

/* see cttk.h */
size_t
cttk_hextobin_gen(void *dst, size_t dst_len,
	const char *src, size_t src_len, const char **err, unsigned flags)
{
	size_t u, v;
	unsigned char *buf;
	int halfbyte;
	int acc;

	buf = dst;
	halfbyte = 0;
	acc = 0;
	v = 0;
	for (u = 0; u < src_len; u ++) {
		int c, d;

		/*
		 * We read source characters as unsigned bytes, so that
		 * values are positive and bytes beyond 0x7F are not
		 * considered whitespace.
		 */
		c = *((const unsigned char *)src + u);
		d = cttk_hexval(c);

		/*
		 * If the character is not an hex digit, it may be
		 * whitespace to be ignored; otherwise, this is an
		 * error to report. Padding of half-bytes must be applied
		 * where necessary (in such a case, output buffer capacity
		 * was already checked).
		 */
		if (d < 0) {
			if ((flags & CTTK_HEX_SKIP_WS) != 0 && c <= 32) {
				continue;
			}
			if (err != NULL) {
				*err = src + u;
			}
			if (halfbyte && (flags & CTTK_HEX_PAD_ODD) != 0) {
				if (buf != NULL) {
					buf[v] = acc;
				}
				v ++;
			}
			return v;
		}

		/*
		 * We have a new digit. We either keep it in the accumulator
		 * (first digit of the next byte) or store it into the output
		 * buffer.
		 *
		 * Output buffer overflow is detected on the first digit,
		 * not the second.
		 */
		if (halfbyte) {
			if (buf != NULL) {
				buf[v] = acc + d;
			}
			v ++;
		} else {
			if (buf != NULL && v == dst_len) {
				if (err != NULL) {
					*err = src + u;
				}
				return v;
			}
			acc = d << 4;
		}
		halfbyte = !halfbyte;
	}

	/*
	 * This part is reached when the source string is finished. We
	 * encountered no error so far. If there is a pending digit,
	 * then we either pad it into a full byte, or report an error,
	 * depending on the relevant flag. Note that output buffer
	 * capacity was already checked when that first digit was read.
	 */
	if (halfbyte) {
		if ((flags & CTTK_HEX_PAD_ODD) != 0) {
			if (buf != NULL) {
				buf[v] = acc;
			}
			v ++;
		} else {
			if (err != NULL) {
				*err = src + src_len;
			}
			return v;
		}
	}
	if (err != NULL) {
		*err = NULL;
	}
	return v;
}

/* see cttk.h */
size_t
cttk_bintohex_gen(char *dst, size_t dst_len,
	const void *src, size_t src_len, unsigned flags)
{
	const unsigned char *buf;
	size_t u, v;
	int halflast, uppercase;

	if (dst == NULL) {
		return (src_len << 1);
	}

	/*
	 * Special case: destination buffer has size 0; we cannot even
	 * write the terminating null byte.
	 */
	if (dst_len == 0) {
		return 0;
	}

	/*
	 * Adjust lengths. Three situations:
	 *
	 *  - Destination is large enough for all characters.
	 *  - Destination is too short, but may accommodate an even number
	 *    of digits.
	 *  - Destination is too short, but may accommodate an odd number
	 *    of digits.
	 */
	halflast = 0;
	if (dst_len < (1 + (src_len << 1))) {
		if ((dst_len & 1) == 0) {
			src_len = (dst_len - 1) >> 1;
			halflast = 1;
		} else {
			src_len = dst_len >> 1;
		}
	}

	buf = src;
	v = 0;
	uppercase = (flags & CTTK_HEX_UPPERCASE) != 0;
	for (u = 0; u < src_len; u ++) {
		int x;

		x = buf[u];
		dst[v ++] = cttk_hexdigit(x >> 4, uppercase);
		dst[v ++] = cttk_hexdigit(x & 15, uppercase);
	}
	if (halflast) {
		int x;

		x = buf[u];
		dst[v ++] = cttk_hexdigit(x >> 4, uppercase);
	}
	dst[v] = 0;
	return v;
}
