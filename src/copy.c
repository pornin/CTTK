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
void
cttk_cond_copy(cttk_bool ctl, void *dst, const void *src, size_t len)
{
	/*
	 * TODO: make optimised versions for special cases:
	 *
	 *  - We can copy by full words, depending on alignment (and
	 *    architecture tolerance for unaligned accesses), even in
	 *    case of overlap (it suffices to do the copy in the
	 *    appropriate direction).
	 *
	 *  - We may use SSE2, AVX2, or similar vector operands on
	 *    platforms that support them.
	 *
	 * Code below is the basic, portable variant. It relies on the
	 * idea that if source and destination overlap, then converting
	 * the pointers to integers should preserve ordering.
	 */
	unsigned char *bdst;
	const unsigned char *bsrc;
	size_t u;

	/*
	 * In order to perform a constant-time copy, we need to read the
	 * current contents of the destination buffer, which may be, from
	 * the point of view of the C compiler, uninitialised. Note that
	 * reading uninitialised values of a character type is actually
	 * supported by the C standard (values are unspecified, but this
	 * shall incur no trap representation, and therefore no undefined
	 * behaviour).
	 */
	bdst = dst;
	bsrc = src;
	if ((uintptr_t)dst <= (uintptr_t)src) {
		u = 0;
		while (u < len) {
			bdst[u] ^= (bsrc[u] ^ bdst[u]) & -ctl.v;
			u ++;
		}
	} else {
		u = len;
		while (u > 0) {
			u --;
			bdst[u] ^= (bsrc[u] ^ bdst[u]) & -ctl.v;
		}
	}
}
