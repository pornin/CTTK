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
 * Return the value of a Base64 character. This is -1 if the character
 * is not a Base64 character; otherwise, the value ranges from 0 to 63.
 */
static int
b64val(char c)
{
	uint32_t p, q, r, z;

	p = (uint32_t)c - 0x41;
	q = (uint32_t)c - 0x61;
	r = (uint32_t)c - 0x30;

	z = ((p + 1) & -cttk_u32_lt(p, 26).v)
		| ((q + 27) & -cttk_u32_lt(q, 26).v)
		| ((r + 53) & -cttk_u32_lt(r, 10).v)
		| (63 & -cttk_u32_eq((uint32_t)c, 0x2B).v)
		| (64 & -cttk_u32_eq((uint32_t)c, 0x2F).v);
	return (int)z - 1;
}

/* see cttk.h */
size_t
cttk_b64tobin_gen(void *dst, size_t dst_len,
	const char *src, size_t src_len, const char **err, unsigned flags)
{
	unsigned char *buf;
	unsigned acc;
	size_t u, v;
	int lc;
	int nows;

	buf = dst;
	acc = 0;
	lc = 0;
	nows = (flags & CTTK_B64DEC_NO_WS) != 0;
	for (u = 0, v = 0; u < src_len; u ++) {
		int c, d;

		/*
		 * We want source characters in the 0x00..0xFF range,
		 * hence the cast to unsigned char.
		 */
		c = *((const unsigned char *)src + u);
		d = b64val(c);

		/*
		 * If the character is not an hex digit, then it may be
		 * whitespace to be ignored.
		 */
		if (d < 0) {
			if (!nows && c <= 32) {
				continue;
			}
			if (c == 0x3D && !(flags & CTTK_B64DEC_NO_PAD)) {
				/*
				 * We found an '=' sign.
				 *
				 * We report an error if there are accumulated
				 * non-zero bits. Note that these bits are
				 * erroneous, hence we allow them to leak.
				 */
				if (lc < 2 || acc != 0) {
					goto b64dec_err;
				}
				for (u ++; u < src_len; u ++) {
					c = *((const unsigned char *)src + u);
					if (lc == 2 && c == 0x3D) {
						lc ++;
						continue;
					}
					if (c > 32 || nows) {
						goto b64dec_err;
					}
				}
				if (lc == 2) {
					goto b64dec_err;
				}
				if (err != NULL) {
					*err = NULL;
				}
				return v;
			}
			goto b64dec_err;
		}

		/*
		 * Accumulate extra character.
		 */
		acc = (acc << 6) | (unsigned)d;

		/*
		 * Output a byte, if possible.
		 *
		 * Some extra explanations are required about the management
		 * of a short output buffer. When there is no room left
		 * in the output buffer, the first character on which the
		 * problem is detected depends on the position in the current
		 * 4-character chunk:
		 *
		 *  - When reading the first character of a chunk, if the
		 *    output buffer is full, then we know there is a problem.
		 *    Note that we don't have a full byte to store at this
		 *    point.
		 *
		 *  - When reading the second character of a chunk, and
		 *    (after producing the first byte) we still have some
		 *    buffered non-zero bits, and the output buffer is now
		 *    full, then again a failure is inevitable.
		 *
		 *  - Similarly when reading the third character of a chunk,
		 *    two bytes have been produced, and there are buffered
		 *    non-zero bits, which implies that a third byte will be
		 *    produced.
		 *
		 * However, the above implies that in some cases, the
		 * error will be reported on a source character that
		 * depends on the value of some data bits -- which are a
		 * priori perfectly normal data bits, not erroneous
		 * bits. We are talking here about an output buffer
		 * being too short for a given input string. Constant-time
		 * processing requires that we do not indulge in
		 * memory activity that depends on secret data bits, and
		 * this includes conditional jumps. Therefore, we MUST NOT
		 * report an error based on the value of these buffered
		 * bits. In these cases, the error reporting will be delayed
		 * to the processing of the next input character.
		 *
		 * Thus, the test on a full output buffer shall be done
		 * when reading characters 0, 2 and 3. We _can_ do it when
		 * reading character 1, since it would have been trapped
		 * at character 0 anyway. We may thus do the test
		 * systematically.
		 */
		if (buf != NULL && v >= dst_len) {
			goto b64dec_err;
		}
		if (lc != 0) {
			unsigned out;

			if (lc == 1) {
				out = acc >> 4;
				acc &= 0x0F;
			} else if (lc == 2) {
				out = acc >> 2;
				acc &= 0x03;
			} else {
				out = acc;
				acc = 0;
			}
			if (buf != NULL) {
				buf[v] = out;
			}
			v ++;
		}
		lc = (lc + 1) & 3;
	}

	/*
	 * Reached end of source string. Whether this is OK depends on
	 * the state and flags:
	 *
	 *  - If padding is expected, then this is correct only if we
	 *    processed an integral number of chunks.
	 *
	 *  - If padding is not expected, then this is correct only if
	 *    the current chunk is not 1 lone character, and there are
	 *    no non-zero extra bits.
	 *
	 * In that case, we can test for the value of the extra bits,
	 * since they are erroenous, thus not part of the actually secret
	 * data.
	 */
	if (flags & CTTK_B64DEC_NO_PAD) {
		if (lc == 1 || acc != 0) {
			goto b64dec_err;
		}
	} else {
		if (lc != 0) {
			goto b64dec_err;
		}
	}
	if (err != NULL) {
		*err = NULL;
	}
	return v;

b64dec_err:
	if (err != NULL) {
		*err = src + u;
	}
	return v;
}

static char
b64char(uint32_t x)
{
	/*
	 * Values 0 to 25 map to 0x41..0x5A ('A' to 'Z')
	 * Values 26 to 51 map to 0x61..0x7A ('a' to 'z')
	 * Values 52 to 61 map to 0x30..0x39 ('0' to '9')
	 * Value 62 maps to 0x2B ('+')
	 * Value 63 maps to 0x2F ('/')
	 */
	uint32_t a, b, c;

	a = x - 26;
	b = x - 52;
	c = x - 62;

	/*
	 * Looking at bits 8..15 of values a, b and c:
	 *
	 *     x       a   b   c
	 *  ---------------------
	 *   0..25    FF  FF  FF
	 *   26..51   00  FF  FF
	 *   52..61   00  00  FF
	 *   62..63   00  00  00
	 */
	return (char)(((x + 0x41) & ((a & b & c) >> 8))
		| ((x + (0x61 - 26)) & ((~a & b & c) >> 8))
		| ((x - (52 - 0x30)) & ((~a & ~b & c) >> 8))
		| ((0x2B + ((x & 1) << 2)) & (~(a | b | c) >> 8)));
}

/* see cttk.h */
size_t
cttk_bintob64_gen(char *dst, size_t dst_len,
	const void *src, size_t src_len, unsigned flags)
{
	size_t num, lc, dlen, u, v, n, line_len;
	const unsigned char *buf;

	/*
	 * Compute total output length (not counting the terminating 0).
	 */
	num = src_len / 3;
	lc = src_len % 3;
	dlen = num << 2;
	if (lc != 0) {
		if (flags & CTTK_B64ENC_NO_PAD) {
			dlen += lc + 1;
		} else {
			dlen += 4;
		}
	}
	if (src_len != 0 && (flags & CTTK_B64ENC_NEWLINE) != 0) {
		size_t nl;

		if (flags & CTTK_B64ENC_LINE64) {
			nl = (dlen + 63) >> 6;
			line_len = 16;
		} else {
			nl = (dlen + 75) / 76;
			line_len = 19;
		}
		if (flags & CTTK_B64ENC_CRLF) {
			nl <<= 1;
		}
		dlen += nl;
	} else {
		line_len = 0;
	}

	/*
	 * Easy special cases.
	 */
	if (dst == NULL) {
		return dlen;
	}
	if (dst_len == 0) {
		return 0;
	}

	/*
	 * We reserve one character in the output for the terminating 0.
	 */
	dst_len --;

	/*
	 * Output characters.
	 *  u = index in source buffer
	 *  v = index in output buffer
	 *  n = size of current line (counted in groups of 3 bytes / 4 chars)
	 */
	buf = src;
	num *= 3;

#define OUTC(z)   do { \
		if (v == dst_len) { \
			goto b64enc_exit; \
		} \
		dst[v ++] = (z); \
	} while (0)

	for (u = 0, v = 0, n = 0; u < num; u += 3) {
		uint32_t x;

		x = ((uint32_t)buf[u] << 16)
			| ((uint32_t)buf[u + 1] << 8)
			| (uint32_t)buf[u + 2];
		OUTC(b64char(x >> 18));
		OUTC(b64char((x >> 12) & 0x3F));
		OUTC(b64char((x >> 6) & 0x3F));
		OUTC(b64char(x & 0x3F));
		if (++ n == line_len) {
			if (flags & CTTK_B64ENC_CRLF) {
				OUTC(0x0D);
			}
			OUTC(0x0A);
			n = 0;
		}
	}
	if (lc != 0 || n != 0) {
		if (lc == 1) {
			uint32_t x;

			x = buf[u];
			OUTC(b64char(x >> 2));
			OUTC(b64char((x << 4) & 0x3F));
			if (!(flags & CTTK_B64ENC_NO_PAD)) {
				OUTC(0x3D);
				OUTC(0x3D);
			}
		} else if (lc == 2) {
			uint32_t x;

			x = ((uint32_t)buf[u] << 8) | (uint32_t)buf[u + 1];
			OUTC(b64char(x >> 10));
			OUTC(b64char((x >> 4) & 0x3F));
			OUTC(b64char((x << 2) & 0x3F));
			if (!(flags & CTTK_B64ENC_NO_PAD)) {
				OUTC(0x3D);
			}
		}
		if (flags & CTTK_B64ENC_NEWLINE) {
			if (flags & CTTK_B64ENC_CRLF) {
				OUTC(0x0D);
			}
			OUTC(0x0A);
		}
	}

#undef OUTC

b64enc_exit:
	dst[v] = 0;
	return v;
}
