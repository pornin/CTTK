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
 * Memory layout: a big integer is a sequence of 32 bit words.
 * First word (header) contains (least to most significant order):
 *
 *    size % 31      5 bits, value is 0 to 30
 *    size / 31      26 bits
 *    NaN flag       1 bit (1 = NaN, 0 = not NaN)
 *
 * Further words contain the value, 31 bits per word, little-endian.
 * The upper bit of each uint32_t is always 0. The sign bit is extended
 * over the complete last word (excluding bit 31).
 *
 * If the NaN flag is set, then words may contain any value, with
 * the following constraints:
 *   - The top bit of each word (except the header word) is still 0.
 *   - The header word is still fully defined.
 *
 * Size is not secret, so we can make conditional jumps based on that
 * size.
 *
 *
 * Let h be the value of the header word, with the NaN flag masked out.
 * Then:
 *
 *   - The number of value words is equal to: (h + 31) >> 5
 *   - The bit length is: h - (h >> 5)
 */

/*
 * Get index of the top bit (sign bit) given the encoded size (header
 * word, without the "NaN" flag). The returned index is relative to
 * the top word.
 */
static inline unsigned
top_index(uint32_t h)
{
	h = (h & 31) - 1;
	return h + (31 & (h >> 5));
}

/*
 * Sign-extend an n-bit value to 32 bits (1 <= n <= 32).
 */
static inline uint32_t
signext(uint32_t v, unsigned n)
{
	uint32_t hi, lo;

	hi = -(uint32_t)((v >> (n - 1)) & 1) << (n - 1);
	lo = v & ((uint32_t)-1 >> (32 - n));
	return hi | lo;
}

/* see cttk.h */
void
cttk_i31_init(uint32_t *x, unsigned size)
{
	uint32_t h;

	h = (uint32_t)size + ((uint32_t)size / 31);
	*x = h | 0x80000000;
	memset(x + 1, 0, ((h + 31) >> 5) * sizeof *x);
}

/* see cttk.h */
void
cttk_i31_set_u32(uint32_t *x, uint32_t v)
{
	uint32_t h, size;
	size_t len;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);

	/*
	 * If there is an overflow, then we will get a NaN. Otherwise,
	 * the value is positive, so the sign extends as a 0.
	 */
	memset(x + 2, 0, (len - 1) * sizeof(uint32_t));
	x[1] = v & 0x7FFFFFFF;
	if (size >= 32) {
		x[2] = v >> 31;
	}
	if (size <= 32) {
		x[0] |= cttk_u32_neq0(v >> (size - 1)).v << 31;
	}
}

/* see cttk.h */
void
cttk_i31_set_u32_trunc(uint32_t *x, uint32_t v)
{
	uint32_t h, size;
	size_t len;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);
	memset(x + 1, 0, len * sizeof(uint32_t));
	if (size > 32) {
		x[1] = v & 0x7FFFFFFF;
		x[2] = v >> 31;
	} else if (size == 32) {
		x[1] = v & 0x7FFFFFFF;
		x[2] = -(v >> 31) >> 1;
	} else {
		x[1] = signext(v, size) & 0x7FFFFFFF;
	}
}

/* see cttk.h */
void
cttk_i31_set_u64(uint32_t *x, uint64_t v)
{
	uint32_t h, size;
	size_t len;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);
	memset(x + 1, 0, len * sizeof(uint32_t));
	x[1] = (uint32_t)v & 0x7FFFFFFF;
	if (size > 31) {
		x[2] = (uint32_t)(v >> 31) & 0x7FFFFFFF;
	}
	if (size > 62) {
		x[3] = (uint32_t)(v >> 62);
	}
	if (size <= 64) {
		x[0] |= cttk_u64_neq0(v >> (size - 1)).v << 31;
	}
}

/* see cttk.h */
void
cttk_i31_set_u64_trunc(uint32_t *x, uint64_t v)
{
	uint32_t h, size;
	size_t len;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);
	memset(x + 1, 0, len * sizeof(uint32_t));
	if (size >= 65) {
		/*
		 * If size is 65 bits or more, then the 64-bit value fits
		 * unmodified (positive).
		 */
		x[1] = (uint32_t)v & 0x7FFFFFFF;
		x[2] = (uint32_t)(v >> 31) & 0x7FFFFFFF;
		x[3] = (uint32_t)(v >> 62);
	} else if (size >= 63) {
		/*
		 * If size is 63 or 64 bits, then the value uses three
		 * words; we must truncate and sign-extend the top word.
		 */
		x[1] = (uint32_t)v & 0x7FFFFFFF;
		x[2] = (uint32_t)(v >> 31) & 0x7FFFFFFF;
		x[3] = signext((uint32_t)(v >> 62), size - 62) & 0x7FFFFFFF;
	} else if (size >= 32) {
		/*
		 * If size is 32 to 62 bits, then the value uses two
		 * words; we must truncate and sign-extend the top word.
		 */
		x[1] = (uint32_t)v & 0x7FFFFFFF;
		x[2] = signext((uint32_t)(v >> 31), size - 31) & 0x7FFFFFFF;
	} else {
		x[1] = signext((uint32_t)v, size) & 0x7FFFFFFF;
	}
}

/* see cttk.h */
void
cttk_i31_set_s32(uint32_t *x, int32_t v)
{
	uint32_t h, size;
	size_t u, len;
	uint32_t w;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);
	w = (uint32_t)v;
	x[1] = w & 0x7FFFFFFF;

	/*
	 * If size is at least 32 bits, then there can be no overflow,
	 * but we must extend the sign bit over all remaining words.
	 * If size is 31 bits or less, then we check that all top bits
	 * of the source value are equal to each other.
	 */
	if (size >= 32) {
		w = -(w >> 31) >> 1;
		for (u = 1; u < len; u ++) {
			x[u + 1] = w;
		}
	} else {
		uint32_t m;

		m = (uint32_t)-1 << (size - 1);
		w &= m;
		x[0] |= (cttk_u32_neq0(w).v & cttk_u32_neq0(w ^ m).v) << 31;
	}
}

/* see cttk.h */
void
cttk_i31_set_s64(uint32_t *x, int64_t v)
{
	uint32_t h, size;
	size_t u, len;
	uint64_t w;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	size = h - (h >> 5);
	w = (uint64_t)v;
	x[1] = (uint32_t)w & 0x7FFFFFFF;
	if (size >= 63) {
		uint32_t h;

		x[2] = (uint32_t)(w >> 31) & 0x7FFFFFFF;
		x[3] = signext((uint32_t)(w >> 62), 2) & 0x7FFFFFFF;
		h = -(uint32_t)(w >> 63) >> 1;
		for (u = 3; u < len; u ++) {
			x[u + 1] = h;
		}
	} else if (size >= 32) {
		x[2] = (uint32_t)(w >> 31) & 0x7FFFFFFF;
	}

	/*
	 * Check on overflow: the top bits must be equal to each other.
	 */
	if (size < 64) {
		uint64_t m;

		m = (uint64_t)-1 << (size - 1);
		w &= m;
		x[0] |= (cttk_u64_neq0(w).v & cttk_u64_neq0(w ^ m).v) << 31;
	}
}

/* see cttk.h */
void
cttk_i31_set(uint32_t *d, const uint32_t *a)
{
	uint32_t h;
	size_t dlen, alen;

	/*
	 * Special case: when source and operands are identical, there
	 * is nothing more to do.
	 */
	if (a == d) {
		return;
	}

	/*
	 * We may now assume that operands do not overlap.
	 */
	h = a[0] & 0x7FFFFFFF;
	alen = (h + 31) >> 5;

	h = d[0] & 0x7FFFFFFF;
	dlen = (h + 31) >> 5;
	d[0] = h | (a[0] & 0x80000000);

	if (dlen > alen) {
		size_t u;
		uint32_t w;

		memcpy(d + 1, a + 1, alen * sizeof *a);
		w = -(a[alen] >> 30) >> 1;
		for (u = alen; u < dlen; u ++) {
			d[1 + u] = w;
		}
	} else {
		size_t u;
		uint32_t w, m;

		memcpy(d + 1, a + 1, dlen * sizeof *a);
		m = -(a[alen] >> 30) >> 1;
		w = (d[dlen] ^ m) & ((uint32_t)-1 << top_index(h));
		for (u = dlen; u < alen; u ++) {
			w |= a[u + 1] ^ m;
		}
		d[0] |= (w | -w) & 0x80000000;
	}
}

/* see cttk.h */
void
cttk_i31_set_trunc(uint32_t *d, const uint32_t *a)
{
	uint32_t h;
	size_t dlen, alen;

	/*
	 * Special case: when source and operands are identical, there
	 * is nothing more to do.
	 */
	if (a == d) {
		return;
	}

	/*
	 * We may now assume that operands do not overlap.
	 */
	h = a[0] & 0x7FFFFFFF;
	alen = (h + 31) >> 5;

	h = d[0] & 0x7FFFFFFF;
	dlen = (h + 31) >> 5;
	d[0] = h | (a[0] & 0x80000000);

	if (dlen > alen) {
		size_t u;
		uint32_t w;

		memcpy(d + 1, a + 1, alen * sizeof *a);
		w = -(a[alen] >> 30) >> 1;
		for (u = alen; u < dlen; u ++) {
			d[1 + u] = w;
		}
	} else {
		uint32_t m, sb;

		memcpy(d + 1, a + 1, dlen * sizeof *a);
		m = (uint32_t)1 << top_index(h);
		sb = d[dlen] & m;
		d[dlen] &= m - 1;
		d[dlen] |= -sb & 0x7FFFFFFF;
	}
}

/* see cttk.h */
uint32_t
cttk_i31_to_u32_trunc(const uint32_t *x)
{
	uint32_t r;

	r = x[1];
	if ((x[0] & 0x7FFFFFFF) > 32) {
		r |= x[2] << 31;
	} else {
		r |= (r & 0x40000000) << 1;
	}
	return r & (uint32_t)((x[0] >> 31) - 1);
}

/* see cttk.h */
int32_t
cttk_i31_to_s32_trunc(const uint32_t *x)
{
	uint32_t r;

	r = cttk_i31_to_u32_trunc(x);
	return *(int32_t *)&r;
}

/* see cttk.h */
uint64_t
cttk_i31_to_u64_trunc(const uint32_t *x)
{
	uint32_t h;
	uint64_t r;

	h = x[0] & 0x7FFFFFFF;
	r = x[1];
	if (h > 64) {
		r |= (uint64_t)x[2] << 31 | (uint64_t)x[3] << 62;
	} else if (h > 32) {
		r |= (uint64_t)x[2] << 31;
		r |= -(r & ((uint64_t)1 << 61));
	} else {
		r |= -(r & ((uint64_t)1 << 30));
	}
	return r & ((uint64_t)(x[0] >> 31) - 1);
}

/* see cttk.h */
int64_t
cttk_i31_to_s64_trunc(const uint32_t *x)
{
	uint64_t r;

	r = cttk_i31_to_u64_trunc(x);
	return *(int64_t *)&r;
}

/*
 * Generic decoding routine.
 */
static void
gendec(uint32_t *x, const void *src, size_t src_len, int be, int sig, int trunc)
{
	uint32_t h, top, top2;
	const unsigned char *buf;
	size_t u, v, len;
	unsigned ssb, ssx, k, hk, extra_bits, extra_bits_len;
	cttk_bool in_range;

	x[0] &= 0x7FFFFFFF;
	h = x[0];
	len = (h + 31) >> 5;
	memset(x + 1, 0, len * sizeof *x);
	if (src_len == 0) {
		if (sig) {
			x[0] |= 0x80000000;
		}
		return;
	}
	buf = src;
	hk = top_index(h);

	/*
	 * 'ssb' is the value used for bytes beyond the source buffer.
	 */
	if (sig) {
		if (be) {
			ssb = -(unsigned)(buf[0] >> 7) & 0xFF;
		} else {
			ssb = -(unsigned)(buf[src_len - 1] >> 7) & 0xFF;
		}
	} else {
		ssb = 0;
	}

	/*
	 * u:k points to the next bits to fill in x (u is word index, k
	 * is bit index).
	 * v is source byte index (counting from 0 for least significant).
	 */
	u = 0;
	k = 0;
	v = 0;

	/*
	 * in_range is set to false if the value turns out to be out of
	 * range (this is ignored if truncating). ssx is set to 0x00 or
	 * 0xFF when the sign bit of x is reached.
	 */
	in_range = cttk_true;
	ssx = 0;

	/*
	 * extra_bits / extra_bits_len will be set if there are extra bits
	 * that must be checked against the final value sign.
	 */
	extra_bits = 0;
	extra_bits_len = 0;

	while (u < len || v < src_len) {
		unsigned b;

		/*
		 * Get next byte of input in b.
		 */
		if (v < src_len) {
			b = be ? buf[src_len - 1 - v] : buf[v];
		} else {
			b = ssb;
		}
		v ++;

		if (u < len) {
			if (k <= 23) {
				x[1 + u] |= (uint32_t)b << k;
			} else {
				/*
				 * If we get beyond the last word boundary
				 * then we may have some extra bits which
				 * will have to be checked against the
				 * value sign.
				 */
				x[1 + u] |= ((uint32_t)b << k) & 0x7FFFFFFF;
				if ((u + 1) < len) {
					x[2 + u] |= (uint32_t)b >> (31 - k);
				} else {
					extra_bits = (uint32_t)b >> (31 - k);
					extra_bits_len = k - 23;
				}
			}

			k += 8;
			if (k >= 31) {
				k -= 31;
				u ++;
				if (u == len) {
					ssx = -(unsigned)((x[len] >> hk) & 1)
						& 0xFF;
				}
			}
		} else {
			/*
			 * If all words are filled, then we merely check
			 * that extra bytes have a value compatible with
			 * the range.
			 */
			in_range = cttk_and(in_range, cttk_u32_eq(b, ssx));
		}
	}

	/*
	 * We reach this point only when we filled all value words, and
	 * read all source bytes. ssx has been set. We still need to do
	 * some cleanup actions:
	 *
	 *  - If truncating, then there may be some extra bits in the top
	 *    word that must be replaced with a sign extension.
	 *
	 *  - If not truncating, then we must check that the extra bits
	 *    in the top word, and also the "extra bits" (if applicable),
	 *    have the proper value. Moreover, if source is unsigned, then
	 *    we must also check that we got a positive value.
	 */
	top = x[len];
	top2 = signext(top, hk + 1) & 0x7FFFFFFF;
	if (trunc) {
		x[len] = top2;
	} else {
		in_range = cttk_and(in_range, cttk_u32_eq(top, top2));
		if (extra_bits_len > 0) {
			in_range = cttk_and(in_range, cttk_u32_eq(extra_bits,
				ssx >> (8 - extra_bits_len)));
		}
		if (!sig) {
			in_range = cttk_and(in_range, cttk_u32_eq0(ssx));
		}
		x[0] |= cttk_not(in_range).v << 31;
	}
}

/*
 * Generic encoding routine.
 */
static void
genenc(void *dst, size_t dst_len, const uint32_t *x, int be)
{
	unsigned char *buf;
	uint32_t h, acc, ssx;
	unsigned mask, acc_len;
	size_t u, len, v;

	h = x[0];
	mask = (h >> 31) - 1;
	h &= 0x7FFFFFFF;
	len = (h + 31) >> 5;

	ssx = -(uint32_t)((x[len] >> top_index(h)) & 1) >> 1;
	acc = x[1];
	acc_len = 31;
	u = 1;
	buf = dst;
	for (v = 0; v < dst_len; v ++) {
		unsigned b;

		if (acc_len >= 8) {
			b = acc & 0xFF;
			acc >>= 8;
			acc_len -= 8;
		} else {
			b = acc;
			if (u < len) {
				acc = x[1 + u];
				u ++;
			} else {
				acc = ssx;
			}
			b |= acc << acc_len;
			acc >>= (8 - acc_len);
			acc_len += 23;
		}
		b &= mask;
		if (be) {
			buf[dst_len - 1 - v] = b;
		} else {
			buf[v] = b;
		}
	}
}

/* see cttk.h */
void
cttk_i31_decbe_signed(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 1, 1, 0);
}

/* see cttk.h */
void
cttk_i31_decbe_unsigned(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 1, 0, 0);
}

/* see cttk.h */
void
cttk_i31_decbe_signed_trunc(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 1, 1, 1);
}

/* see cttk.h */
void
cttk_i31_decbe_unsigned_trunc(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 1, 0, 1);
}

/* see cttk.h */
void
cttk_i31_decle_signed(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 0, 1, 0);
}

/* see cttk.h */
void
cttk_i31_decle_unsigned(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 0, 0, 0);
}

/* see cttk.h */
void
cttk_i31_decle_signed_trunc(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 0, 1, 1);
}

/* see cttk.h */
void
cttk_i31_decle_unsigned_trunc(uint32_t *x, const void *src, size_t len)
{
	gendec(x, src, len, 0, 0, 1);
}

/* see cttk.h */
void
cttk_i31_encbe(void *dst, size_t len, const uint32_t *x)
{
	genenc(dst, len, x, 1);
}

/* see cttk.h */
void
cttk_i31_encle(void *dst, size_t len, const uint32_t *x)
{
	genenc(dst, len, x, 0);
}

/*
 * Compare x with zero. This function ignores the NaN flag.
 */
static cttk_bool
val_eq0(const uint32_t *x)
{
	uint32_t h, r;
	size_t len, u;

	h = x[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	r = 0;
	for (u = 0; u < len; u ++) {
		r |= x[u + 1];
	}
	return cttk_u32_eq0(r);
}

/*
 * Test whether x is lower than zero. This function ignores the NaN
 * flag. Since it only grabs the sign bit, it is efficient even for
 * large integers.
 */
static cttk_bool
val_lt0(const uint32_t *x)
{
	uint32_t h;
	size_t len;

	h = x[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	return cttk_bool_of_u32((x[len] >> 30) & 1);
}

/*
 * Get actual bitlength, i.e. minimal number of bits to hold the value,
 * excluding the sign bit (hence, -1 has bitlength 0). This function
 * ignores the NaN flag.
 */
static uint32_t
real_bitlength(const uint32_t *x)
{
	uint32_t h, mx, t, g;
	size_t len, u;
	unsigned k;

	h = x[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	k = top_index(h);
	mx = -(uint32_t)((x[len] >> k) & 1) >> 1;

	/*
	 * mx is an all-zero or all-one pattern (31 bits), depending on
	 * sign bit value. We XOR it with the words, to normalize on the
	 * positive case. We look for the index (g) and value (t) of the
	 * topmost non-zero word.
	 */
	t = x[1];
	g = 0;
	for (u = 1; u < len; u ++) {
		uint32_t w;
		cttk_bool nz;

		w = x[u + 1] ^ mx;
		nz = cttk_u32_neq0(w);
		t = cttk_u32_mux(nz, w, t);
		g = cttk_u32_mux(nz, (uint32_t)u, g);
	}

	return cttk_u32_bitlength(t) + (g << 5) - g;
}

/* see cttk.h */
uint32_t
cttk_i31_to_u32(const uint32_t *x)
{
	uint32_t r;

	r = cttk_i31_to_u32_trunc(x);
	r &= -cttk_u32_lt(real_bitlength(x), 33).v;
	r &= val_lt0(x).v - 1;
	return r;
}

/* see cttk.h */
int32_t
cttk_i31_to_s32(const uint32_t *x)
{
	uint32_t r;

	r = cttk_i31_to_u32_trunc(x);
	r &= -cttk_u32_lt(real_bitlength(x), 32).v;
	return *(int32_t *)&r;
}

/* see cttk.h */
uint64_t
cttk_i31_to_u64(const uint32_t *x)
{
	uint64_t r;

	r = cttk_i31_to_u64_trunc(x);
	r &= -(uint64_t)cttk_u32_lt(real_bitlength(x), 65).v;
	r &= (uint64_t)val_lt0(x).v - 1;
	return r;
}

/* see cttk.h */
int64_t
cttk_i31_to_s64(const uint32_t *x)
{
	uint64_t r;

	r = cttk_i31_to_u64_trunc(x);
	r &= -(uint64_t)cttk_u64_lt(real_bitlength(x), 64).v;
	return *(int64_t *)&r;
}

/* see cttk.h */
cttk_bool
cttk_i31_eq0(const uint32_t *x)
{
	return cttk_and(val_eq0(x), cttk_not(cttk_i31_isnan(x)));
}

/* see cttk.h */
cttk_bool
cttk_i31_neq0(const uint32_t *x)
{
	return cttk_not(cttk_or(val_eq0(x), cttk_i31_isnan(x)));
}

/* see cttk.h */
cttk_bool
cttk_i31_gt0(const uint32_t *x)
{
	return cttk_not(cttk_or(cttk_or(val_eq0(x), val_lt0(x)),
		cttk_i31_isnan(x)));
}

/* see cttk.h */
cttk_bool
cttk_i31_lt0(const uint32_t *x)
{
	return cttk_and(val_lt0(x), cttk_not(cttk_i31_isnan(x)));
}

/* see cttk.h */
cttk_bool
cttk_i31_geq0(const uint32_t *x)
{
	return cttk_not(cttk_or(val_lt0(x), cttk_i31_isnan(x)));
}

/* see cttk.h */
cttk_bool
cttk_i31_leq0(const uint32_t *x)
{
	return cttk_and(cttk_or(val_eq0(x), val_lt0(x)),
		cttk_not(cttk_i31_isnan(x)));
}

/*
 * Test two integers for NaN. This function returns true is either or
 * both are NaN.
 */
static inline cttk_bool
tst_nan2(const uint32_t *x, const uint32_t *y)
{
	return cttk_bool_of_u32((x[0] | y[0]) >> 31);
}

/*
 * Compare integers; this function assumes that both operands have the
 * same size. The common length (in words) is provided. The NaN flag
 * of each value is ignored.
 */
static cttk_bool
val_eq(const uint32_t *x, const uint32_t *y)
{
	size_t u, len;
	uint32_t r;

	len = ((x[0] & 0x7FFFFFFF) + 31) >> 5;
	r = 0;
	for (u = 0; u < len; u ++) {
		r |= x[1 + u] ^ y[1 + u];
	}
	return cttk_u32_eq0(r);
}

/*
 * Compare integers; this function assumes that both operands have the
 * same size. The common length (in words) is provided. The header word
 * of each value is ignored.
 */
static cttk_bool
val_lt(const uint32_t *x, const uint32_t *y)
{
	size_t u, len;
	uint32_t cc;

	len = ((x[0] & 0x7FFFFFFF) + 31) >> 5;
	cc = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wx, wy, wz;

		wx = x[u + 1];
		wy = y[u + 1];
		wz = wx - wy - cc;
		cc = (wz >> 31);
	}

	/*
	 * Mathematical sign of the result (r), depending on the operand
	 * signs and carry value:
	 *
	 *   x   y  cc   r
	 *
	 *   0   0   0   0
	 *   0   0   1   1
	 *   0   1   0   not possible
	 *   0   1   1   0
	 *   1   0   0   1
	 *   1   0   1   not possible
	 *   1   1   0   0
	 *   1   1   1   1
	 *
	 * Thus, the XOR of the sign bits of x and y, and of the carry,
	 * yields the sign of the result.
	 */
	cc ^= (x[len] ^ y[len]) >> 30;
	return cttk_bool_of_u32(cc);
}

/*
 * Generic integer comparison. This function assumes that both operands
 * have the same size, and ignores the NaN flags. Returned value is
 * -1, 0 or 1, converted to uint32_t.
 */
static uint32_t
val_cmp(const uint32_t *x, const uint32_t *y)
{
	size_t u, len;
	uint32_t cc, t;

	len = ((x[0] & 0x7FFFFFFF) + 31) >> 5;
	cc = 0;
	t = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wx, wy, wz;

		wx = x[u + 1];
		wy = y[u + 1];
		wz = wx - wy - cc;
		cc = (wz >> 31);
		t |= wz;
	}

	/*
	 * See val_lt0() for details.
	 */
	cc ^= (x[len] ^ y[len]) >> 30;
	return cttk_u32_neq0(t).v | -cc;
}

/* see cttk.h */
cttk_bool
cttk_i31_eq(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_and(val_eq(x, y), cttk_not(tst_nan2(x, y)));
}

/* see cttk.h */
cttk_bool
cttk_i31_neq(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_not(cttk_or(val_eq(x, y), tst_nan2(x, y)));
}

/* see cttk.h */
cttk_bool
cttk_i31_lt(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_and(val_lt(x, y), cttk_not(tst_nan2(x, y)));
}

/* see cttk.h */
cttk_bool
cttk_i31_leq(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_not(cttk_or(val_lt(y, x), tst_nan2(x, y)));
}

/* see cttk.h */
cttk_bool
cttk_i31_gt(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_and(val_lt(y, x), cttk_not(tst_nan2(x, y)));
}

/* see cttk.h */
cttk_bool
cttk_i31_geq(const uint32_t *x, const uint32_t *y)
{
	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return cttk_false;
	}
	return cttk_not(cttk_or(val_lt(x, y), tst_nan2(x, y)));
}

/* see cttk.h */
int
cttk_i31_sign(const uint32_t *x)
{
	uint32_t w;

	w = (val_eq0(x).v ^ (uint32_t)1) | -val_lt0(x).v;
	w &= (x[0] >> 31) - 1;
	return *(int32_t *)&w;
}

/* see cttk.h */
int
cttk_i31_cmp(const uint32_t *x, const uint32_t *y)
{
	uint32_t w;

	if ((uint32_t)((x[0] ^ y[0]) << 1) != 0) {
		return 0;
	}
	w = val_cmp(x, y) & (uint32_t)(((x[0] | y[0]) >> 31) - 1);
	return *(int32_t *)&w;
}

/* see cttk.h */
void
cttk_i31_copy(uint32_t *d, const uint32_t *s)
{
	if (d != s) {
		if ((uint32_t)((d[0] ^ s[0]) << 1) != 0) {
			d[0] |= 0x80000000;
			return;
		}
		memcpy(d, s, (((s[0] & 0x7FFFFFFF) + 63) >> 5) * sizeof *s);
	}
}

/* see cttk.h */
void
cttk_i31_cond_copy(cttk_bool ctl, uint32_t *d, const uint32_t *s)
{
	cttk_i31_mux(ctl, d, s, d);
}

/* see cttk.h */
void
cttk_i31_swap(uint32_t *a, uint32_t *b)
{
	size_t u, len;

	if (a == b) {
		return;
	}
	if ((uint32_t)((a[0] ^ b[0]) << 1) != 0) {
		a[0] |= 0x80000000;
		b[0] |= 0x80000000;
		return;
	}
	len = ((a[0] & 0x7FFFFFFF) + 63) >> 5;
	for (u = 0; u < len; u ++) {
		uint32_t w;

		w = a[u];
		a[u] = b[u];
		b[u] = w;
	}
}

/* see cttk.h */
void
cttk_i31_cond_swap(cttk_bool ctl, uint32_t *a, uint32_t *b)
{
	size_t u, len;

	if (a == b) {
		return;
	}
	if ((uint32_t)((a[0] ^ b[0]) << 1) != 0) {
		a[0] |= 0x80000000;
		b[0] |= 0x80000000;
		return;
	}
	len = ((a[0] & 0x7FFFFFFF) + 63) >> 5;
	for (u = 0; u < len; u ++) {
		uint32_t wa, wb, wt;

		wa = a[u];
		wb = b[u];
		wt = (wa ^ wb) & -ctl.v;
		a[u] = wa ^ wt;
		b[u] = wb ^ wt;
	}
}

/* see cttk.h */
void
cttk_i31_mux(cttk_bool ctl, uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t u, len;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 63) >> 5;
	for (u = 0; u < len; u ++) {
		d[u] = cttk_u32_mux(ctl, a[u], b[u]);
	}
}

/* see cttk.h */
void
cttk_i31_add(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h, cc, tt;
	size_t len, u;

	/*
	 * Compare sizes. This needs not be constant-time, but take
	 * care to mask out the NaN bit (since that one may be secret).
	 */
	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;

	/*
	 * Since sizes are equal, we can simply OR together the header
	 * words, which will propagate any NaN.
	 */
	d[0] = a[0] | b[0];

	/*
	 * Get the XOR of the top words of a[] and b[]. This must be
	 * done now because either could be used as recipient.
	 */
	tt = a[len] ^ b[len];

	/*
	 * Compute addition.
	 */
	cc = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wa, wb, wd;

		wa = a[u + 1];
		wb = b[u + 1];
		wd = wa + wb + cc;
		d[u + 1] = wd & 0x7FFFFFFF;
		cc = wd >> 31;
	}

	/*
	 * Overflow/underflow:
	 *
	 * Since source values are signed, they extend to infinity
	 * with copies of their sign bit. Depending on the signs of
	 * a and b, we get the following sign for the result (r):
	 *
	 *   a   b  cc   r
	 *
	 *   0   0   0   0
	 *   0   0   1   not possible
	 *   0   1   0   1
	 *   0   1   1   0
	 *   1   0   0   1
	 *   1   0   1   0
	 *   1   1   0   not possible
	 *   1   1   1   1
	 *
	 * Note that some combinations are not possible: when adding two
	 * zeros, even with a carry of 1, you cannot obtain a carry;
	 * similarly, when adding two ones, you cannot not get a carry.
	 * This hinges on the fact that source values are signed, and
	 * the carry we observe is necessarily _after_ processing the
	 * addition on their top (sign) bits.
	 *
	 * Note that in all cases, r is the XOR of the sign bits of a
	 * and b, and of the carry. Result is an overflow or underflow
	 * if and only if the obtained sign bit of d does not match that
	 * value.
	 */
	d[0] |= (((tt ^ d[len]) >> top_index(h)) ^ cc) << 31;
}

/* see cttk.h */
void
cttk_i31_add_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h, cc;
	size_t len, u;

	/*
	 * Compare sizes. This needs not be constant-time, but take
	 * care to mask out the NaN bit (since that one may be secret).
	 */
	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;

	/*
	 * Since sizes are equal, we can simply OR together the header
	 * words, which will propagate any NaN.
	 */
	d[0] = a[0] | b[0];

	/*
	 * Compute addition.
	 */
	cc = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wa, wb, wd;

		wa = a[u + 1];
		wb = b[u + 1];
		wd = wa + wb + cc;
		d[u + 1] = wd & 0x7FFFFFFF;
		cc = wd >> 31;
	}

	/*
	 * Apply truncation to the proper size.
	 */
	d[len] = signext(d[len], top_index(h) + 1) & 0x7FFFFFFF;
}

/* see cttk.h */
void
cttk_i31_sub(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h, cc, tt;
	size_t len, u;

	/*
	 * Compare sizes. This needs not be constant-time, but take
	 * care to mask out the NaN bit (since that one may be secret).
	 */
	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;

	/*
	 * Since sizes are equal, we can simply OR together the header
	 * words, which will propagate any NaN.
	 */
	d[0] = a[0] | b[0];

	/*
	 * Get the XOR of the top words of a[] and b[]. This must be
	 * done now because either could be used as recipient.
	 */
	tt = a[len] ^ b[len];

	/*
	 * Compute subtraction.
	 */
	cc = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wa, wb, wd;

		wa = a[u + 1];
		wb = b[u + 1];
		wd = wa - wb - cc;
		d[u + 1] = wd & 0x7FFFFFFF;
		cc = wd >> 31;
	}

	/*
	 * Overflow/underflow:
	 *
	 *   a   b  cc   r
	 *
	 *   0   0   0   0
	 *   0   0   1   1
	 *   0   1   0   not possible
	 *   0   1   1   0
	 *   1   0   0   1
	 *   1   0   1   not possible
	 *   1   1   0   0
	 *   1   1   1   1
	 *
	 * This yields the same expression as for addition: the
	 * mathematical sign of the result is the XOR of the sign bits
	 * of the source and the carry.
	 */
	d[0] |= (((tt ^ d[len]) >> top_index(h)) ^ cc) << 31;
}

/* see cttk.h */
void
cttk_i31_sub_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h, cc;
	size_t len, u;

	/*
	 * Compare sizes. This needs not be constant-time, but take
	 * care to mask out the NaN bit (since that one may be secret).
	 */
	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;

	/*
	 * Since sizes are equal, we can simply OR together the header
	 * words, which will propagate any NaN.
	 */
	d[0] = a[0] | b[0];

	/*
	 * Compute subtraction.
	 */
	cc = 0;
	for (u = 0; u < len; u ++) {
		uint32_t wa, wb, wd;

		wa = a[u + 1];
		wb = b[u + 1];
		wd = wa - wb - cc;
		d[u + 1] = wd & 0x7FFFFFFF;
		cc = wd >> 31;
	}

	/*
	 * Apply truncation to the proper size.
	 */
	d[len] = signext(d[len], top_index(h) + 1) & 0x7FFFFFFF;
}

/* see cttk.h */
void
cttk_i31_neg(uint32_t *d, const uint32_t *x)
{
	uint32_t h, cc, tt;
	size_t u, len;

	h = x[0] & 0x7FFFFFFF;
	if ((uint32_t)((h ^ d[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}

	d[0] = x[0];
	len = (h + 31) >> 5;
	cc = 1;
	tt = x[len];
	for (u = 0; u < len; u ++) {
		uint32_t w;

		w = x[u + 1];
		w = ~w + cc;
		d[u + 1] = w & 0x7FFFFFFF;
		cc = (w >> 31) ^ 1;
	}

	/*
	 * We get an overflow if the source operand is equal to the
	 * minimum value in the representable range. This is the
	 * only situation where the sign bit of the source and of
	 * the result are both 1.
	 */
	d[0] |= (uint32_t)(((d[len] & tt) >> top_index(h)) & 1) << 31;
}

/* see cttk.h */
void
cttk_i31_neg_trunc(uint32_t *d, const uint32_t *x)
{
	uint32_t h, cc;
	size_t u, len;

	h = x[0] & 0x7FFFFFFF;
	if ((uint32_t)((h ^ d[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}

	d[0] = x[0];
	len = (h + 31) >> 5;
	cc = 1;
	for (u = 0; u < len; u ++) {
		uint32_t w;

		w = x[u + 1];
		w = ~w + cc;
		d[u + 1] = w & 0x7FFFFFFF;
		cc = (w >> 31) ^ 1;
	}
	d[len] = signext(d[len], top_index(h) + 1) & 0x7FFFFFFF;
}

/*
 * Generic multiplication routine. It computes a truncated multiplication,
 * but returns true if and only if the truncation changed the value. This
 * function:
 *  - ignores the NaN flag;
 *  - assumes that source and destination operands have the same size;
 *  - assumes that the destination array is distinct from the source arrays.
 *
 * TODO: use Karatsuba for large inputs.
 */
static cttk_bool
genmul_separate(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h, ssa, ssb, ssd;
	size_t u, v, len;
	uint64_t cc;
	cttk_bool only0, only1, opz;

	h = d[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	ssa = -(uint32_t)(a[len] >> 30) >> 1;
	ssb = -(uint32_t)(b[len] >> 30) >> 1;
	only0 = cttk_true;
	only1 = cttk_true;
	opz = cttk_or(cttk_i31_eq0(a), cttk_i31_eq0(b));

	cc = 0;
	for (u = 0; u < (len << 1); u ++) {
		uint32_t wd;
		uint64_t zd;

		zd = cc;
		cc = 0;
		for (v = 0; v <= u; v ++) {
			uint32_t wa, wb;
			uint64_t zr;

			wa = v < len ? a[1 + v] : ssa;
			wb = (v + len) > u ? b[1 + u - v] : ssb;
			zr = mulu32w(wa, wb);
			zd += zr & 0x7FFFFFFF;
			cc += zr >> 31;
		}
		cc += zd >> 31;
		wd = (uint32_t)zd & 0x7FFFFFFF;
		if (u < len) {
			d[1 + u] = wd;
		} else {
			only0 = cttk_and(only0, cttk_u32_eq0(wd));
			only1 = cttk_and(only1, cttk_u32_eq0(wd ^ 0x7FFFFFFF));
		}
	}

	/*
	 * We check that all upper bits have a value compatible with the
	 * expected result sign.
	 */
	ssd = ssa ^ ssb;
	ssd &= (uint32_t)(opz.v - 1);
	return cttk_and(
		cttk_bool_of_u32(cttk_u32_mux(
			cttk_bool_of_u32(ssd & 1), only1.v, only0.v)),
		cttk_u32_eq0((d[len] ^ ssd) >> top_index(h)));
}

/*
 * Multiplication function with a stack-based temporary. Assumptions:
 *
 *  - d is equal (same pointer value) to a or b or both;
 *  - a, b and d have the same size;
 *  - the common size does not exceed CTTK_MAX_INT_BUF bytes.
 */
static cttk_bool
genmul_stack(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t t[CTTK_MAX_INT_BUF / sizeof(uint32_t)];
	uint32_t h;
	size_t len;
	cttk_bool r;

	h = d[0] & 0x7FFFFFFF;
	t[0] = h;
	len = (h + 31) >> 5;
	memset(t + 1, 0, len * sizeof t[0]);
	r = genmul_separate(t, a, b);
	memcpy(d + 1, t + 1, len * sizeof *d);
	return r;
}

static cttk_bool
genmul(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t blen;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return cttk_false;
	}
	d[0] = a[0] | b[0];

	if (d != a && d != b) {
		return genmul_separate(d, a, b);
	}
	blen = ((h + 63) >> 5) * sizeof *d;
	if (blen <= CTTK_MAX_INT_BUF) {
		return genmul_stack(d, a, b);
	}
#if !CTTK_NO_MALLOC
	{
		uint32_t *t;

		t = malloc(blen);
		if (t != NULL) {
			cttk_bool r;
			size_t len;

			t[0] = h;
			len = (h + 31) >> 5;
			memset(t + 1, 0, len * sizeof *t);
			r = genmul_separate(t, a, b);
			memcpy(d + 1, t + 1, len * sizeof *t);
			free(t);
			return r;
		}
	}
#endif
	d[0] |= 0x80000000;
	return cttk_false;
}

/* see cttk.h */
void
cttk_i31_mul(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	cttk_bool r;

	r = genmul(d, a, b);
	d[0] |= (r.v ^ 1) << 31;
}

/* see cttk.h */
void
cttk_i31_mul_trunc(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t len;

	genmul(d, a, b);
	h = d[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	d[len] = signext(d[len], top_index(h) + 1) & 0x7FFFFFFF;
}

/*
 * Generic left-shift function:
 *
 *  - d and a must have been already verified to have the same size.
 *  - Shift count is n = nd*31+nm, with 0 <= nm < 31, and n fits on 32 bits.
 *  - If ctl is false, then the shift is not actually done.
 *
 * Returned value is false if the value overflows/underflows.
 *
 * nd and nm may leak.
 */
static cttk_bool
genlsh(uint32_t *d, const uint32_t *a, uint32_t nd, unsigned nm, cttk_bool ctl)
{
	uint32_t n, h, hk, bl, ssa, tt;
	size_t len, u;
	cttk_bool r;

	d[0] = a[0];
	h = d[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	bl = h - (h >> 5);
	n = 31 * nd + nm;
	ssa = -(a[len] >> 30) & 0x7FFFFFFF;

	/*
	 * If the shift count is greater than or equal to the type size,
	 * then we can only get zero. This is an overflow/underflow if
	 * the source value is not 0.
	 */
	if (n >= bl) {
		r = cttk_true;
		for (u = 0; u < len; u ++) {
			uint32_t wa;

			wa = a[1 + u];
			r = cttk_and(r, cttk_u32_eq0(wa));
			d[1 + u] = wa & (uint32_t)(ctl.v - 1);
		}
		return cttk_or(r, cttk_not(ctl));
	}

	/*
	 * We have:
	 *    len * 31 >= bl
	 *    n >= 31 * nd
	 * We reach that point only if n < bl, which implies nd < len.
	 */

	/*
	 * Since source and destination may be the same array, we need
	 * to do the shift in high to low order.
	 */
	r = cttk_true;
	for (u = len; u > len - nd; u --) {
		r = cttk_and(r, cttk_u32_eq(ssa, a[u]));
	}
	if (nm == 0) {
		for (u = len; u > nd; u --) {
			d[u] = cttk_u32_mux(ctl, a[u - nd], a[u]);
		}
	} else {
		r = cttk_and(r,
			cttk_u32_eq0((a[len - nd] ^ ssa) >> (31 - nm)));
		for (u = len; u > nd; u --) {
			uint32_t wa, wd;

			wa = a[u - nd];
			wd = (wa << nm) & 0x7FFFFFFF;
			if ((u - nd) > 1) {
				wd |= a[u - nd - 1] >> (31 - nm);
			}
			d[u] = cttk_u32_mux(ctl, wd, a[u]);
		}
	}
	for (u = nd; u > 0; u --) {
		d[u] = a[u] & (ctl.v - 1);
	}

	/*
	 * 'r' contains the overflow/underflow check for all the dropped
	 * bits, but we must still check the top bits in the high word
	 * are all equal to the expected sign (and we should also adjust
	 * them, for truncation support).
	 */
	hk = top_index(h);
	tt = signext(d[len], hk + 1) & 0x7FFFFFFF;
	r = cttk_and(r, cttk_u32_eq(d[len], tt));
	d[len] = tt;
	r = cttk_and(r, cttk_u32_eq0((tt ^ ssa) >> hk));

	/*
	 * We must also check that the final sign matches the source sign;
	 * otherwise, the checks ab
	 */
	return cttk_or(r, cttk_not(ctl));
}

/*
 * Generic right-shift function:
 *
 *  - d and a must have been already verified to have the same size.
 *  - Shift count is n = nd*31+nm, with 0 <= nm < 31, and n fits on 32 bits.
 *  - If ctl is false, then the shift is not actually done.
 *
 * nd and nm may leak.
 */
static void
genrsh(uint32_t *d, const uint32_t *a, uint32_t nd, unsigned nm, cttk_bool ctl)
{
	uint32_t h, ssa, n, bl;
	size_t u, len;

	d[0] = a[0];
	h = d[0] & 0x7FFFFFFF;
	len = (h + 31) >> 5;
	bl = h - (h >> 5);
	n = 31 * nd + nm;
	ssa = -(a[len] >> 30) & 0x7FFFFFFF;

	/*
	 * If right-shifting by at least bl-1 bits, then the result is
	 * either 0 or -1, depending on source sign.
	 */
	if ((n + 1) >= bl) {
		for (u = 0; u < len; u ++) {
			d[1 + u] = cttk_u32_mux(ctl, ssa, a[1 + u]);
		}
		return;
	}

	/*
	 * We have:
	 *    len * 31 >= bl
	 *    n >= 31 * nd
	 * We reach that point only if n < bl, which implies nd < len.
	 */

	if (nm == 0) {
		for (u = 0; u < (len - nd); u ++) {
			d[1 + u] = cttk_u32_mux(ctl, a[1 + u + nd], a[1 + u]);
		}
	} else {
		for (u = 0; u < (len - nd - 1); u ++) {
			uint32_t wa;

			wa = ((a[1 + u + nd] >> nm)
				| (a[2 + u + nd] << (31 - nm))) & 0x7FFFFFFF;
			d[1 + u] = cttk_u32_mux(ctl, wa, a[1 + u]);
		}
		d[len - nd] = cttk_u32_mux(ctl,
			((a[len] >> nm) | (ssa << (31 - nm))) & 0x7FFFFFFF,
			a[len - nd]);
	}
	for (u = len - nd; u < len; u ++) {
		d[1 + u] = cttk_u32_mux(ctl, ssa, a[1 + u]);
	}
}

/*
 * Precomputed powers of two divided by 31 (quotient and remainder).
 */
static const uint32_t p2m31[] = {
	((uint32_t)1 <<  0) / 31, ((uint32_t)1 <<  0) % 31,
	((uint32_t)1 <<  1) / 31, ((uint32_t)1 <<  1) % 31,
	((uint32_t)1 <<  2) / 31, ((uint32_t)1 <<  2) % 31,
	((uint32_t)1 <<  3) / 31, ((uint32_t)1 <<  3) % 31,
	((uint32_t)1 <<  4) / 31, ((uint32_t)1 <<  4) % 31,
	((uint32_t)1 <<  5) / 31, ((uint32_t)1 <<  5) % 31,
	((uint32_t)1 <<  6) / 31, ((uint32_t)1 <<  6) % 31,
	((uint32_t)1 <<  7) / 31, ((uint32_t)1 <<  7) % 31,
	((uint32_t)1 <<  8) / 31, ((uint32_t)1 <<  8) % 31,
	((uint32_t)1 <<  9) / 31, ((uint32_t)1 <<  9) % 31,
	((uint32_t)1 << 10) / 31, ((uint32_t)1 << 10) % 31,
	((uint32_t)1 << 11) / 31, ((uint32_t)1 << 11) % 31,
	((uint32_t)1 << 12) / 31, ((uint32_t)1 << 12) % 31,
	((uint32_t)1 << 13) / 31, ((uint32_t)1 << 13) % 31,
	((uint32_t)1 << 14) / 31, ((uint32_t)1 << 14) % 31,
	((uint32_t)1 << 15) / 31, ((uint32_t)1 << 15) % 31,
	((uint32_t)1 << 16) / 31, ((uint32_t)1 << 16) % 31,
	((uint32_t)1 << 17) / 31, ((uint32_t)1 << 17) % 31,
	((uint32_t)1 << 18) / 31, ((uint32_t)1 << 18) % 31,
	((uint32_t)1 << 19) / 31, ((uint32_t)1 << 19) % 31,
	((uint32_t)1 << 20) / 31, ((uint32_t)1 << 20) % 31,
	((uint32_t)1 << 21) / 31, ((uint32_t)1 << 21) % 31,
	((uint32_t)1 << 22) / 31, ((uint32_t)1 << 22) % 31,
	((uint32_t)1 << 23) / 31, ((uint32_t)1 << 23) % 31,
	((uint32_t)1 << 24) / 31, ((uint32_t)1 << 24) % 31,
	((uint32_t)1 << 25) / 31, ((uint32_t)1 << 25) % 31,
	((uint32_t)1 << 26) / 31, ((uint32_t)1 << 26) % 31,
	((uint32_t)1 << 27) / 31, ((uint32_t)1 << 27) % 31,
	((uint32_t)1 << 28) / 31, ((uint32_t)1 << 28) % 31,
	((uint32_t)1 << 29) / 31, ((uint32_t)1 << 29) % 31,
	((uint32_t)1 << 30) / 31, ((uint32_t)1 << 30) % 31,
	((uint32_t)1 << 31) / 31, ((uint32_t)1 << 31) % 31,
};

/* see cttk.h */
void
cttk_i31_lsh(uint32_t *d, const uint32_t *a, uint32_t n)
{
	cttk_bool r;

	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	r = genlsh(d, a, n / 31, n % 31, cttk_true);
	d[0] |= (r.v ^ 1) << 31;
}

/* see cttk.h */
void
cttk_i31_lsh_prot(uint32_t *d, const uint32_t *a, uint32_t n)
{
	int i;

	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	for (i = 0; i < 32; i ++) {
		cttk_bool r;

		r = genlsh(d, a, p2m31[i << 1], p2m31[(i << 1) + 1],
			cttk_u32_neq0(n & ((uint32_t)1 << i)));
		d[0] |= (r.v ^ 1) << 31;
		a = d;
	}
}

/* see cttk.h */
void
cttk_i31_lsh_trunc(uint32_t *d, const uint32_t *a, uint32_t n)
{
	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	genlsh(d, a, n / 31, n % 31, cttk_true);
}

/* see cttk.h */
void
cttk_i31_lsh_trunc_prot(uint32_t *d, const uint32_t *a, uint32_t n)
{
	int i;

	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	for (i = 0; i < 32; i ++) {
		genlsh(d, a, p2m31[i << 1], p2m31[(i << 1) + 1],
			cttk_u32_neq0(n & ((uint32_t)1 << i)));
		a = d;
	}
}

/* see cttk.h */
void
cttk_i31_rsh(uint32_t *d, const uint32_t *a, uint32_t n)
{
	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	genrsh(d, a, n / 31, n % 31, cttk_true);
}

/* see cttk.h */
void
cttk_i31_rsh_prot(uint32_t *d, const uint32_t *a, uint32_t n)
{
	int i;

	if (((d[0] ^ a[0]) << 1) != 0) {
		d[0] |= 0x80000000;
		return;
	}
	for (i = 0; i < 32; i ++) {
		genrsh(d, a, p2m31[i << 1], p2m31[(i << 1) + 1],
			cttk_u32_neq0(n & ((uint32_t)1 << i)));
		a = d;
	}
}

/*
 * Internal division routine:
 *
 *   - r is non-NULL.
 *   - q, r, t1 and t2 are distinct from each other. Only q may be NULL.
 *   - t1 and t2 are distinct from a and b.
 *   - All non-NULL arrays have the same size.
 *
 * Note that q and r may be aliases on a or b.
 */
static void
gendiv_inner(uint32_t *q, uint32_t *r, const uint32_t *a,
	const uint32_t *b, uint32_t *t1, uint32_t *t2, int mod)
{
	uint32_t h, n, hk, sa, sb;
	size_t ud, len, u;
	unsigned um;
	cttk_bool a_isnan, a_isminv, b_isnan, b_isminv, b_iszero, b_ismone;
	cttk_bool both_nan, half_nan;

	h = b[0] & 0x7FFFFFFF;
	hk = top_index(h);
	n = h - (h >> 5);
	ud = h >> 5;
	um = h & 31;
	len = (h + 31) >> 5;

	/*
	 * We use absolute values, and then adjust signs.
	 * Special cases are the following:
	 *
	 *   - If a is NaN and/or b is NaN, then q and r are set to NaN.
	 *   - If b is zero, then q and r are set to NaN.
	 *   - If a and/or b is MinValue, then the corresponding absolute
	 *     value cannot be computed. Instead, we have to perform the
	 *     following adjustments:
	 *
	 *       If a == MinValue and b < 0, subtract b from a, and add 1
	 *       to q before adjusting its sign.
	 *
	 *       If a == MinValue and b > 0, add b to a, and add 1 to q
	 *       before adjusting its sign.
	 *
	 *       If b == MinValue and a == MinValue, then set q to 1 and
	 *       r to 0.
	 *
	 *       If b == MinValue and a != MinValue, then set q to 0 and
	 *       r to a.
	 *
	 *     The two first rules may be merged into the following:
	 *
	 *       If a == MinValue, add |b| to a, and add 1 to q before
	 *       adjusting its sign.
	 */

	/*
	 * If a or b is NaN, or b is zero, then both q and r will be NaN.
	 * If a is MinValue and b is -1, then q = NaN and r = 0.
	 * We obtain the relevant values here:
	 *    a_isnan      a is NaN
	 *    b_isnan      b is NaN
	 *    a_isminv     a == MinValue
	 *    b_isminv     b == MinValue
	 *    b_iszero     b == 0
	 *    b_ismone     b == -1
	 */
	a_isnan = cttk_i31_isnan(a);
	b_isnan = cttk_i31_isnan(b);
	a_isminv = cttk_true;
	b_isminv = cttk_true;
	b_iszero = cttk_true;
	b_ismone = cttk_true;
	for (u = 0; (u + 1) < len; u ++) {
		a_isminv = cttk_and(a_isminv, cttk_u32_eq0(a[1 + u]));
		b_isminv = cttk_and(b_isminv, cttk_u32_eq0(b[1 + u]));
		b_iszero = cttk_and(b_iszero, cttk_u32_eq0(b[1 + u]));
		b_ismone = cttk_and(b_iszero,
			cttk_u32_eq(b[1 + u], 0x7FFFFFFF));
	}
	a_isminv = cttk_and(a_isminv,
		cttk_u32_eq(a[len], ((uint32_t)-1 << hk) & 0x7FFFFFFF));
	b_isminv = cttk_and(b_isminv,
		cttk_u32_eq(b[len], ((uint32_t)-1 << hk) & 0x7FFFFFFF));
	b_iszero = cttk_and(b_iszero, cttk_u32_eq0(b[len]));
	b_ismone = cttk_and(b_ismone, cttk_u32_eq(b[len], 0x7FFFFFFF));

	/*
	 * Get signs.
	 */
	sa = a[len] >> 30;
	sb = b[len] >> 30;

	/*
	 * Compute |b| into t2.
	 */
	cttk_i31_neg(t2, b);
	cttk_i31_cond_copy(cttk_u32_eq0(sb), t2, b);

	/*
	 * Set r to |a| or |a+|b||. t1 is free at that point. r may be
	 * aliased on a or b, but not on t1.
	 */
	cttk_i31_add(t1, a, t2);
	cttk_i31_cond_copy(cttk_not(a_isminv), t1, a);
	cttk_i31_neg(r, t1);
	cttk_i31_cond_copy(cttk_not(cttk_bool_of_u32(t1[len] >> 30)), r, t1);

	/*
	 * Now r is set, and |b|. We "forget" about the true b, and instead
	 * use |b|.
	 */
	b = t2;

	/*
	 * Set q to 0 (if defined).
	 */
	if (q != NULL) {
		q[0] &= 0x7FFFFFFF;
		memset(q + 1, 0, ((h + 31) >> 5) * sizeof *q);
	}

	/*
	 * Compute the division on the positive values.
	 */
	while (n -- > 0) {
		cttk_bool e;

		/*
		 * The shift on b may yield a NaN, but in that case, the
		 * comparison will be false, and the cond_copy will put
		 * back the non-NaN value of r.
		 */
		cttk_i31_lsh(t1, b, n);
		e = cttk_i31_leq(t1, r);
		cttk_i31_sub(t1, r, t1);
		cttk_i31_cond_copy(e, r, t1);
		if (q != NULL) {
			if (um == 0) {
				um = 30;
				ud --;
			} else {
				um --;
			}
			q[1 + ud] |= e.v << um;
		}
	}

	/*
	 * Adjust values and signs. t1 is free.
	 */
	if (q != NULL) {
		int32_t p;

		/*
		 * If b == MinValue, then we must set q to 0; if
		 * a == MinValue too, we will add 1 afterwards.
		 */
		cttk_i31_set_u32_trunc(t1, 0);
		cttk_i31_cond_copy(b_isminv, q, t1);

		/*
		 * We adjust the sign of q: it is negative if the
		 * signs of a and b differ.
		 */
		cttk_i31_neg(t1, q);
		cttk_i31_cond_copy(cttk_bool_of_u32(sa ^ sb), q, t1);

		/*
		 * If a == MinValue, then there is a +1 or -1 to add.
		 * Normally we would do the +1 before adjusting the
		 * sign of q, but there is an edge case where the +1
		 * would set the value to MaxValue+1 (i.e. NaN),
		 * whereas setting the sign first, then subtracting 1,
		 * yields MinValue (which is correct and not NaN).
		 */
		p = cttk_bool_to_int(a_isminv);
		cttk_i31_set_s32(t1,
			cttk_s32_mux(cttk_bool_of_u32(sa ^ sb), -p, p));
		cttk_i31_add(q, q, t1);
	}
	cttk_i31_neg(t1, r);
	cttk_i31_cond_copy(cttk_bool_of_u32(sa), r, t1);

	/*
	 * Handle the special cases for b == MinValue. In that case,
	 * when we computed |b|, we got a NaN, and all checks in the
	 * loop were false. Thus, if a != MinValue, r contains a copy
	 * of a at this point (which is correct) and we just have to
	 * set the quotient to 0.
	 */
	cttk_i31_set_u32_trunc(t1, 0);
	if (q != NULL) {
		cttk_i31_cond_copy(
			cttk_and(b_isminv, cttk_not(a_isminv)), q, t1);
	}
	cttk_i31_cond_copy(cttk_and(b_isminv, a_isminv), r, t1);
	if (q != NULL) {
		cttk_i31_set_u32(t1, 1);
		cttk_i31_cond_copy(cttk_and(b_isminv, a_isminv), q, t1);
	}

	/*
	 * Apply NaN conditions.
	 */
	both_nan = cttk_or(cttk_or(a_isnan, b_isnan), b_iszero);
	half_nan = cttk_and(a_isminv, b_ismone);
	if (q != NULL) {
		q[0] |= cttk_or(both_nan, half_nan).v << 31;
	}
	r[0] |= both_nan.v << 31;
	cttk_i31_set_u32_trunc(t1, 0);
	cttk_i31_cond_copy(half_nan, r, t1);

	/*
	 * Extra step if doing modular reduction:
	 *  - If r >= 0, it is already fine.
	 *  - If r < 0 and b < 0, then we must subtract b.
	 *  - If r < 0 and b > 0, then we must add b.
	 *
	 * Since |r| < |b|, then we have the following properties:
	 *  - If the generic division hit a NaN, then the result must
	 *    be a NaN as well.
	 *  - If r and b are both non-NaN, then the extra operation
	 *    cannot overflow or underflow.
	 *
	 * We may not have b anymore at that point, though (since r
	 * might be aliased to b). We have |b| in t2 (and we set the 'b'
	 * pointer to t2). We can add |b| conditionally to r being
	 * negative. However, |b| might be a NaN if b == MinValue; we
	 * must isolate that case.
	 */
	if (mod) {
		uint32_t sr;

		sr = r[len] >> 30;
		cttk_i31_add(t1, r, b);
		cttk_i31_cond_copy(cttk_and(cttk_bool_of_u32(sr),
			cttk_not(b_isminv)), r, t1);

		/*
		 * When b == MinValue, we want to subtract MinValue from
		 * r, which is equivalent to flipping the sign bit.
		 */
		r[len] ^= ((-(sr & b_isminv.v) << hk) & 0x7FFFFFFF);
	}
}

static void
gendiv_stack3(uint32_t *q, const uint32_t *a, const uint32_t *b, int mod)
{
	uint32_t tr[CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t))];
	uint32_t t1[CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t))];
	uint32_t t2[CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t))];

	memset(tr, 0, sizeof tr);
	memset(t1, 0, sizeof t1);
	memset(t2, 0, sizeof t2);
	tr[0] = t1[0] = t2[0] = a[0];
	gendiv_inner(q, tr, a, b, t1, t2, mod);
}

static void
gendiv_stack2(uint32_t *q, uint32_t *r,
	const uint32_t *a, const uint32_t *b, int mod)
{
	uint32_t t1[CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t))];
	uint32_t t2[CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t))];

	memset(t1, 0, sizeof t1);
	memset(t2, 0, sizeof t2);
	t1[0] = t2[0] = a[0];
	gendiv_inner(q, r, a, b, t1, t2, mod);
}

/*
 * Generic division routine. This function assumes that sizes have been
 * verified to be equal to each other. Either q and r may be NULL, but
 * not both. Also, q != r.
 *
 * If mod is non-zero, then an extra step is applied to ensure a nonnegative
 * remainder.
 */
static void
gendiv(uint32_t *q, uint32_t *r, const uint32_t *a, const uint32_t *b, int mod)
{
	uint32_t h;
	size_t wlen;

	/*
	 * The inner function requires the following:
	 *  - A non-NULL r; we need a temporary if r is NULL.
	 *  - A nonnegative divisor, which must be a temporary since b
	 *    might be NULL.
	 *  - An extra temporary.
	 *
	 * We thus need either two (if r != NULL) or three (if r == NULL)
	 * temporaries.
	 */
	h = a[0] & 0x7FFFFFFF;
	wlen = (h + 63) >> 5;
	if (r == NULL) {
		if (wlen <= (CTTK_MAX_INT_BUF / (3 * sizeof(uint32_t)))) {
			gendiv_stack3(q, a, b, mod);
			return;
		}
#if !CTTK_NO_MALLOC
		{
			uint32_t *tr, *t1, *t2;

			tr = malloc(wlen * sizeof(uint32_t));
			t1 = malloc(wlen * sizeof(uint32_t));
			t2 = malloc(wlen * sizeof(uint32_t));
			if (tr != NULL && t1 != NULL && t2 != NULL) {
				gendiv_inner(q, tr, a, b, t1, t2, mod);
				free(tr);
				free(t1);
				free(t2);
				return;
			}
			if (tr != NULL) {
				free(tr);
			}
			if (t1 != NULL) {
				free(t1);
			}
			if (t2 != NULL) {
				free(t2);
			}
		}
#endif
	} else {
		if (wlen <= (CTTK_MAX_INT_BUF / (2 * sizeof(uint32_t)))) {
			gendiv_stack2(q, r, a, b, mod);
			return;
		}
#if !CTTK_NO_MALLOC
		{
			uint32_t *t1, *t2;

			t1 = malloc(wlen * sizeof(uint32_t));
			t2 = malloc(wlen * sizeof(uint32_t));
			if (t1 != NULL && t2 != NULL) {
				gendiv_inner(q, r, a, b, t1, t2, mod);
				free(t1);
				free(t2);
				return;
			}
			if (t1 != NULL) {
				free(t1);
			}
			if (t2 != NULL) {
				free(t2);
			}
		}
#endif
	}

	/*
	 * Could not find enough memory for temporaries...
	 */
	if (q != NULL) {
		q[0] |= 0x80000000;
	}
	if (r != NULL) {
		r[0] |= 0x80000000;
	}
}

/* see cttk.h */
void
cttk_i31_divrem(uint32_t *q, uint32_t *r, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;

	h = a[0] & 0x7FFFFFFF;
	if (h != (b[0] & 0x7FFFFFFF)) {
		if (q != NULL) {
			q[0] |= 0x80000000;
		}
		if (r != NULL) {
			r[0] |= 0x80000000;
		}
		return;
	}
	if (q != NULL && h != (q[0] & 0x7FFFFFFF)) {
		q[0] |= 0x80000000;
		q = NULL;
	}
	if (r != NULL && h != (r[0] & 0x7FFFFFFF)) {
		r[0] |= 0x80000000;
		r = NULL;
	}
	if (q == NULL && r == NULL) {
		return;
	}
	if (q == r) {
		q[0] |= 0x80000000;
		r[0] |= 0x80000000;
		return;
	}

	gendiv(q, r, a, b, 0);
}

/* see cttk.h */
void
cttk_i31_mod(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	gendiv(NULL, d, a, b, 1);
}

/* see cttk.h */
void
cttk_i31_and(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t len, u;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;
	d[0] = a[0] | b[0];
	for (u = 1; u <= len; u ++) {
		d[u] = a[u] & b[u];
	}
}

/* see cttk.h */
void
cttk_i31_or(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t len, u;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;
	d[0] = a[0] | b[0];
	for (u = 1; u <= len; u ++) {
		d[u] = a[u] | b[u];
	}
}

/* see cttk.h */
void
cttk_i31_xor(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t len, u;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;
	d[0] = a[0] | b[0];
	for (u = 1; u <= len; u ++) {
		d[u] = a[u] ^ b[u];
	}
}

/* see cttk.h */
void
cttk_i31_eqv(uint32_t *d, const uint32_t *a, const uint32_t *b)
{
	uint32_t h;
	size_t len, u;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF) || h != (b[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;
	d[0] = a[0] | b[0];
	for (u = 1; u <= len; u ++) {
		d[u] = a[u] ^ b[u] ^ 0x7FFFFFFF;
	}
}

/* see cttk.h */
void
cttk_i31_not(uint32_t *d, const uint32_t *a)
{
	uint32_t h;
	size_t len, u;

	h = d[0] & 0x7FFFFFFF;
	if (h != (a[0] & 0x7FFFFFFF)) {
		d[0] |= 0x80000000;
		return;
	}
	len = (h + 31) >> 5;
	d[0] = a[0];
	for (u = 1; u <= len; u ++) {
		d[u] = a[u] ^ 0x7FFFFFFF;
	}
}
