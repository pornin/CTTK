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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "cttk.h"

static void
check(int v, const char *fmt, ...)
{
	if (!v) {
		va_list ap;

		va_start(ap, fmt);
		fprintf(stderr, "CHECK FAILED: ");
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
		va_end(ap);
		abort();
	}
}

static void
test_comparisons_32(void)
{
	int i, j;

	printf("Test comparisons (32-bit): ");
	fflush(stdout);

	for (i = 0; i < 16 * 32; i ++) {
		for (j = 0; j < 16 * 32; j ++) {
			uint32_t xu, yu;
			int32_t xs, ys;
			int r;

			xu = ((uint32_t)1 << (i >> 4)) + (uint32_t)(i & 15) - 8;
			yu = ((uint32_t)1 << (j >> 4)) + (uint32_t)(j & 15) - 8;
			xs = *(int32_t *)&xu;
			ys = *(int32_t *)&yu;

			check(cttk_s32_neq0(xs).v == (uint32_t)(xs != 0),
				"s32_neq0: %ld", (long)xs);
			check(cttk_s32_neq0(ys).v == (uint32_t)(ys != 0),
				"s32_neq0: %ld", (long)ys);
			check(cttk_s32_eq0(xs).v == (uint32_t)(xs == 0),
				"s32_eq0: %ld", (long)xs);
			check(cttk_s32_eq0(ys).v == (uint32_t)(ys == 0),
				"s32_eq0: %ld", (long)ys);

			check(cttk_u32_neq0(xu).v == (uint32_t)(xu != 0),
				"u32_neq0: %lu", (unsigned long)xu);
			check(cttk_u32_neq0(yu).v == (uint32_t)(yu != 0),
				"u32_neq0: %lu", (unsigned long)yu);
			check(cttk_u32_eq0(xu).v == (uint32_t)(xu == 0),
				"u32_eq0: %lu", (unsigned long)xu);
			check(cttk_u32_eq0(yu).v == (uint32_t)(yu == 0),
				"u32_eq0: %lu", (unsigned long)yu);

			check(cttk_s32_eq(xs, ys).v == (uint32_t)(xs == ys),
				"s32_eq: %ld, %ld", (long)xs, (long)ys);
			check(cttk_s32_neq(xs, ys).v == (uint32_t)(xs != ys),
				"s32_neq: %ld, %ld", (long)xs, (long)ys);

			check(cttk_u32_eq(xu, yu).v == (uint32_t)(xu == yu),
				"u32_eq: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);
			check(cttk_u32_neq(xu, yu).v == (uint32_t)(xu != yu),
				"u32_neq: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);

			check(cttk_s32_gt(xs, ys).v == (uint32_t)(xs > ys),
				"s32_gt: %ld, %ld", (long)xs, (long)ys);
			check(cttk_s32_geq(xs, ys).v == (uint32_t)(xs >= ys),
				"s32_geq: %ld, %ld", (long)xs, (long)ys);
			check(cttk_s32_lt(xs, ys).v == (uint32_t)(xs < ys),
				"s32_lt: %ld, %ld", (long)xs, (long)ys);
			check(cttk_s32_leq(xs, ys).v == (uint32_t)(xs <= ys),
				"s32_leq: %ld, %ld", (long)xs, (long)ys);

			check(cttk_u32_gt(xu, yu).v == (uint32_t)(xu > yu),
				"u32_gt: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);
			check(cttk_u32_geq(xu, yu).v == (uint32_t)(xu >= yu),
				"u32_geq: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);
			check(cttk_u32_lt(xu, yu).v == (uint32_t)(xu < yu),
				"u32_lt: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);
			check(cttk_u32_leq(xu, yu).v == (uint32_t)(xu <= yu),
				"u32_leq: %lu, %lu",
				(unsigned long)xu, (unsigned long)yu);

			if (xs < ys) {
				r = -1;
			} else if (xs == ys) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s32_cmp(xs, ys) == r,
				"s32_cmp: %ld, %ld", (long)xs, (long)ys);

			if (xu < yu) {
				r = -1;
			} else if (xu == yu) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_u32_cmp(xu, yu) == r,
				"u32_cmp: %ld, %ld",
				(unsigned long)xu, (unsigned long)yu);

			check(cttk_s32_gt0(xs).v == (uint32_t)(xs > 0),
				"s32_gt0: %ld, %ld", (long)xs);
			check(cttk_s32_geq0(xs).v == (uint32_t)(xs >= 0),
				"s32_geq0: %ld, %ld", (long)xs);
			check(cttk_s32_lt0(xs).v == (uint32_t)(xs < 0),
				"s32_lt0: %ld, %ld", (long)xs);
			check(cttk_s32_leq0(xs).v == (uint32_t)(xs <= 0),
				"s32_leq0: %ld, %ld", (long)xs);
			check(cttk_s32_gt0(ys).v == (uint32_t)(ys > 0),
				"s32_gt0: %ld, %ld", (long)ys);
			check(cttk_s32_geq0(ys).v == (uint32_t)(ys >= 0),
				"s32_geq0: %ld, %ld", (long)ys);
			check(cttk_s32_lt0(ys).v == (uint32_t)(ys < 0),
				"s32_lt0: %ld, %ld", (long)ys);
			check(cttk_s32_leq0(ys).v == (uint32_t)(ys <= 0),
				"s32_leq0: %ld, %ld", (long)ys);

			if (xs < 0) {
				r = -1;
			} else if (xs == 0) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s32_sign(xs) == r,
				"s32_sign: %ld", (long)xs);
			if (ys < 0) {
				r = -1;
			} else if (ys == 0) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s32_sign(ys) == r,
				"s32_sign: %ld", (long)ys);
		}

		if ((i & 15) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_comparisons_64(void)
{
	int i, j;

	printf("Test comparisons (64-bit): ");
	fflush(stdout);

	for (i = 0; i < 16 * 64; i ++) {
		for (j = 0; j < 16 * 64; j ++) {
			uint64_t xu, yu;
			int64_t xs, ys;
			int r;

			xu = ((uint64_t)1 << (i >> 4)) + (uint64_t)(i & 15) - 8;
			yu = ((uint64_t)1 << (j >> 4)) + (uint64_t)(j & 15) - 8;
			xs = *(int64_t *)&xu;
			ys = *(int64_t *)&yu;

			check(cttk_s64_neq0(xs).v == (uint32_t)(xs != 0),
				"s64_neq0: %lld", (long long)xs);
			check(cttk_s64_neq0(ys).v == (uint32_t)(ys != 0),
				"s64_neq0: %lld", (long long)ys);
			check(cttk_s64_eq0(xs).v == (uint32_t)(xs == 0),
				"s64_eq0: %lld", (long long)xs);
			check(cttk_s64_eq0(ys).v == (uint32_t)(ys == 0),
				"s64_eq0: %lld", (long long)ys);

			check(cttk_u64_neq0(xu).v == (uint32_t)(xu != 0),
				"u64_neq0: %llu", (unsigned long long)xu);
			check(cttk_u64_neq0(yu).v == (uint32_t)(yu != 0),
				"u64_neq0: %llu", (unsigned long long)yu);
			check(cttk_u64_eq0(xu).v == (uint32_t)(xu == 0),
				"u64_eq0: %llu", (unsigned long long)xu);
			check(cttk_u64_eq0(yu).v == (uint32_t)(yu == 0),
				"u64_eq0: %llu", (unsigned long long)yu);

			check(cttk_s64_eq(xs, ys).v == (uint32_t)(xs == ys),
				"s64_eq: %lld, %lld",
				(long long)xs, (long long)ys);
			check(cttk_s64_neq(xs, ys).v == (uint32_t)(xs != ys),
				"s64_neq: %lld, %lld",
				(long long)xs, (long long)ys);

			check(cttk_u64_eq(xu, yu).v == (uint32_t)(xu == yu),
				"u64_eq: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);
			check(cttk_u64_neq(xu, yu).v == (uint32_t)(xu != yu),
				"u64_neq: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);

			check(cttk_s64_gt(xs, ys).v == (uint32_t)(xs > ys),
				"s64_gt: %lld, %lld",
				(long long)xs, (long long)ys);
			check(cttk_s64_geq(xs, ys).v == (uint32_t)(xs >= ys),
				"s64_geq: %lld, %lld",
				(long long)xs, (long long)ys);
			check(cttk_s64_lt(xs, ys).v == (uint32_t)(xs < ys),
				"s64_lt: %lld, %lld",
				(long long)xs, (long long)ys);
			check(cttk_s64_leq(xs, ys).v == (uint32_t)(xs <= ys),
				"s64_leq: %lld, %lld",
				(long long)xs, (long long)ys);

			check(cttk_u64_gt(xu, yu).v == (uint32_t)(xu > yu),
				"u64_gt: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);
			check(cttk_u64_geq(xu, yu).v == (uint32_t)(xu >= yu),
				"u64_geq: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);
			check(cttk_u64_lt(xu, yu).v == (uint32_t)(xu < yu),
				"u64_lt: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);
			check(cttk_u64_leq(xu, yu).v == (uint32_t)(xu <= yu),
				"u64_leq: %llu, %llu",
				(unsigned long long)xu, (unsigned long long)yu);

			if (xs < ys) {
				r = -1;
			} else if (xs == ys) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s64_cmp(xs, ys) == r,
				"s64_cmp: %lld, %lld",
				(long long)xs, (long long)ys);

			if (xu < yu) {
				r = -1;
			} else if (xu == yu) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_u64_cmp(xu, yu) == r,
				"u64_cmp: %lld, %lld",
				(unsigned long long)xu, (unsigned long long)yu);

			check(cttk_s64_gt0(xs).v == (uint32_t)(xs > 0),
				"s64_gt0: %lld, %lld", (long long)xs);
			check(cttk_s64_geq0(xs).v == (uint32_t)(xs >= 0),
				"s64_geq0: %lld, %lld", (long long)xs);
			check(cttk_s64_lt0(xs).v == (uint32_t)(xs < 0),
				"s64_lt0: %lld, %lld", (long long)xs);
			check(cttk_s64_leq0(xs).v == (uint32_t)(xs <= 0),
				"s64_leq0: %lld, %lld", (long long)xs);
			check(cttk_s64_gt0(ys).v == (uint32_t)(ys > 0),
				"s64_gt0: %lld, %lld", (long long)ys);
			check(cttk_s64_geq0(ys).v == (uint32_t)(ys >= 0),
				"s64_geq0: %lld, %lld", (long long)ys);
			check(cttk_s64_lt0(ys).v == (uint32_t)(ys < 0),
				"s64_lt0: %lld, %lld", (long long)ys);
			check(cttk_s64_leq0(ys).v == (uint32_t)(ys <= 0),
				"s64_leq0: %lld, %lld", (long long)ys);

			if (xs < 0) {
				r = -1;
			} else if (xs == 0) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s64_sign(xs) == r,
				"s64_sign: %lld", (long long)xs);
			if (ys < 0) {
				r = -1;
			} else if (ys == 0) {
				r = 0;
			} else {
				r = 1;
			}
			check(cttk_s64_sign(ys) == r,
				"s64_sign: %lld", (long long)ys);
		}

		if ((i & 15) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_hex(void)
{
	static const unsigned char databin[] = {
		0x38, 0xF2, 0x45, 0x86, 0x85, 0x82, 0xE3, 0x59,
		0x83, 0x3D, 0xE0, 0x0D, 0x90, 0x50, 0x5F, 0xA8,
		0x41, 0xBF, 0x0A, 0xE3, 0x01, 0x5B, 0x70, 0xCA,
		0x16, 0x0B, 0xDB, 0x63, 0x57, 0xF1, 0xB9, 0xE3,
		0x9A, 0x3F, 0xF6, 0x49, 0x28, 0xCA, 0xC9, 0x4F,
		0xCB, 0x6E, 0x44, 0x11, 0x92, 0x82, 0xC4, 0x72,
		0xF4, 0x31, 0xB1, 0xDF, 0x0C, 0xDE, 0xA5, 0x85,
		0x03, 0x26, 0x4C, 0x87, 0xC4, 0x76, 0x5E, 0x5C,
		0x6C, 0x61, 0x63, 0x7F, 0x93, 0xC1, 0xE1, 0x60,
		0xC3, 0x3C, 0x2C, 0x11, 0x4C, 0x83, 0x6C, 0x1A,
		0x57, 0x2C, 0x72, 0x1B, 0x63, 0x44, 0x0B, 0xB8,
		0x4A, 0x46, 0x7C, 0x42, 0xAA, 0xBA, 0x56, 0xF2,
		0x67, 0x5E, 0xA4, 0xA0, 0x0D, 0x7A, 0xF9, 0x59,
		0xE1, 0xF5, 0xCA, 0xE7, 0x0B, 0xA0, 0xA6, 0xD7,
		0x3E, 0xFE, 0xCF, 0x08, 0xDF, 0xE4, 0xA4, 0xBF,
		0x7D, 0x4B, 0x84, 0x91, 0x17, 0xF2, 0x0C, 0x3B,
		0x66, 0x58, 0xF3, 0x27, 0x4C, 0x88, 0x5C, 0x09,
		0x47, 0xD0, 0xE3, 0x00, 0x40, 0x16, 0xBC, 0xFC,
		0x56, 0x2C, 0x58, 0x3E, 0x95, 0xA2, 0x1C, 0x45,
		0x7D, 0x18, 0xA9, 0xBC, 0xD1, 0x45, 0x54, 0x43,
		0x5B, 0x46, 0x55, 0xBB, 0xA2, 0x23, 0xB1, 0x43,
		0x1D, 0x02, 0x8A, 0xD2, 0x14, 0x7A, 0x3C, 0x36,
		0x61, 0x21, 0x39, 0xA6, 0x7B, 0xC1, 0xB0, 0x13,
		0x00, 0xCC, 0xAB, 0x50, 0x9F, 0x1D, 0x40, 0x01,
		0xD2, 0x84, 0x55, 0x6C, 0xF5, 0x0E, 0x14, 0x74,
		0xBC, 0xF1, 0x1D, 0x17, 0xE4, 0x98, 0x70, 0x2C,
		0x1A, 0xD7, 0x66, 0x67, 0x33, 0xEE, 0x20, 0xE9,
		0x5D, 0x93, 0xD0, 0xF4, 0x0A, 0xD8, 0x6D, 0x23,
		0xF1, 0x08, 0x9D, 0x70, 0x1B, 0x97, 0xAC, 0x99,
		0xB0, 0xE5, 0x3A, 0x4E, 0x7E, 0x46, 0xB2, 0x81,
		0x24, 0xB9, 0x08, 0x23, 0xD2, 0x21, 0xF9, 0x3D,
		0xB6, 0x96, 0xF4, 0xF6, 0x06, 0x65, 0x66, 0x04
	};

	char datastr[1 + ((sizeof databin) << 1)];
	char tmp[sizeof datastr];
	unsigned char buf[(sizeof databin) + 1];
	size_t u, v, w;
	int i, j;
	const char *err;

	printf("Test hex: ");
	fflush(stdout);

	for (i = 0; i < 2; i ++) {
		unsigned flags;

		flags = (i == 0) ? 0 : CTTK_HEX_UPPERCASE;
		for (u = 0; u < sizeof databin; u ++) {
			sprintf(datastr + (u << 1),
				(i == 0) ? "%02x" : "%02X", databin[u]);
		}
		for (u = 0; u <= sizeof datastr; u ++) {
			memset(tmp, 'T', sizeof tmp);
			v = cttk_bintohex_gen(tmp, u,
				databin, sizeof databin, flags);
			if (u == 0) {
				check(v == 0, "bintohex 1");
				check(tmp[0] == 'T', "bintohex 2");
			} else {
				check(v == u - 1, "bintohex 3");
				check(tmp[v] == 0, "bintohex 4");
				check(memcmp(datastr, tmp, v) == 0,
					"bintohex 5");
			}
		}
		for (u = 0; u <= sizeof databin; u ++) {
			memset(tmp, 'U', sizeof tmp);
			v = cttk_bintohex_gen(tmp, sizeof tmp,
				databin, u, flags);
			check(v == (u << 1), "bintohex 6");
			check(tmp[v] == 0, "bintohex 7");
			check(memcmp(datastr, tmp, v) == 0, "bintohex 8");
			v = cttk_bintohex_gen(NULL, 0, databin, u, flags);
			check(v == (u << 1), "bintohex 9");
		}
		printf(".");
		fflush(stdout);
	}

	for (i = 0; i < 2; i ++) {
		for (u = 0; u < sizeof databin; u ++) {
			sprintf(datastr + (u << 1),
				(i == 0) ? "%02x" : "%02X", databin[u]);
		}
		for (u = 0; u <= sizeof datastr; u ++) {
			memset(buf, 0xFF, sizeof buf);
			v = cttk_hextobin_gen(buf, sizeof buf,
				datastr, u, &err, 0);
			check(v == (u >> 1), "hextobin 1");
			if (u == sizeof datastr) {
				check(err == datastr + u - 1, "hextobin 2");
			} else if ((u & 1) == 1) {
				check(err == datastr + u, "hextobin 3");
			} else {
				check(err == NULL, "hextobin 4");
			}
			check(memcmp(buf, databin, v) == 0, "hextobin 5");
			check(buf[v] == 0xFF, "hextobin 6");

			v = cttk_hextobin_gen(NULL, 0, datastr, u, &err, 0);
			check(v == (u >> 1), "hextobin 7");
			v = cttk_hextobin_gen(NULL, 0, datastr, u, NULL, 0);
			check(v == (u >> 1), "hextobin 8");

			memset(buf, 0xFF, sizeof buf);
			v = cttk_hextobin_gen(buf, sizeof buf,
				datastr, u, &err, CTTK_HEX_PAD_ODD);
			if (u == sizeof datastr) {
				check(v == (u >> 1), "hextobin 9");
				check(err == datastr + u - 1, "hextobin 10");
			} else if ((u & 1) == 1) {
				check(v == ((u + 1) >> 1), "hextobin 11");
				check(err == NULL, "hextobin 12");
				v --;
				check(buf[v] == (databin[v] & 0xF0),
					"hextobin 13");
			} else {
				check(v == (u >> 1), "hextobin 14");
				check(err == NULL, "hextobin 15");
			}
			check(memcmp(buf, databin, v) == 0, "hextobin 16");
		}

		for (u = 0; u <= sizeof databin; u ++) {
			memset(buf, 0xFF, sizeof buf);
			v = cttk_hextobin_gen(buf, u,
				datastr, (sizeof databin) << 1, &err, 0);
			check(u == v, "hextobin 17");
			check(buf[v] == 0xFF, "hextobin 18");
			if (u == sizeof databin) {
				check(err == NULL, "hextobin 19");
			} else {
				check(err == datastr + (u << 1), "hextobin 20");
			}
			check(memcmp(buf, databin, u) == 0, "hextobin 21");
		}

		for (u = 0, v = 0; datastr[u] && v < (sizeof tmp) - 10; u ++) {
			for (j = 0; j < (int)(u % 7); j ++) {
				tmp[v] = 1 + (v & 31);
				v ++;
			}
			tmp[v ++] = datastr[u];
		}
		if ((u & 1) != 0) {
			v --;
		}
		memset(buf, 0xFF, sizeof buf);
		w = cttk_hextobin_gen(buf, sizeof buf,
			tmp, v, &err, CTTK_HEX_SKIP_WS);
		check(w == (u >> 1), "hextobin 22");
		check(buf[w] == 0xFF, "hextobin 23");
		check(err == NULL, "hextobin 24");
		check(memcmp(buf, databin, w) == 0, "hextobin 25");

		printf(".");
		fflush(stdout);
	}

	printf(" done.\n");
	fflush(stdout);
}

static char
b64char(int x)
{
	return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz0123456789+/" [x];
}

static void
b64enc(char *dst, const unsigned char *buf, size_t len,
	int pad, int line_len, int crlf)
{
	size_t u, v;
	int lc;

	lc = 0;
	v = 0;
	for (u = 0; (u + 2) < len; u += 3) {
		uint32_t x;

		x = ((uint32_t)buf[u] << 16)
			| ((uint32_t)buf[u + 1] << 8)
			| (uint32_t)buf[u + 2];
		dst[v ++] = b64char(x >> 18);
		dst[v ++] = b64char((x >> 12) & 63);
		dst[v ++] = b64char((x >> 6) & 63);
		dst[v ++] = b64char(x & 63);
		lc ++;
		if (lc == line_len) {
			lc = 0;
			if (crlf) {
				dst[v ++] = '\r';
			}
			dst[v ++] = '\n';
		}
	}
	if (u == len - 1) {
		uint32_t x;

		x = buf[u];
		dst[v ++] = b64char(x >> 2);
		dst[v ++] = b64char((x << 4) & 63);
		if (pad) {
			dst[v ++] = '=';
			dst[v ++] = '=';
		}
		lc ++;
	} else if (u == len - 2) {
		uint32_t x;

		x = ((uint32_t)buf[u] << 8) | (uint32_t)buf[u + 1];
		dst[v ++] = b64char(x >> 10);
		dst[v ++] = b64char((x >> 4) & 63);
		dst[v ++] = b64char((x << 2) & 63);
		if (pad) {
			dst[v ++] = '=';
		}
		lc ++;
	}
	if (lc != 0 && line_len != 0) {
		if (crlf) {
			dst[v ++] = '\r';
		}
		dst[v ++] = '\n';
	}
	dst[v ++] = 0;
}

static void
test_base64(void)
{
	/*
	 * Test data is a random 256-byte blob.
	 */
	static const unsigned char databin[] = {
		0x6D, 0xDD, 0x85, 0x66, 0xB3, 0x64, 0xB6, 0x41,
		0xF6, 0x70, 0x69, 0xD7, 0x20, 0xCB, 0x2A, 0x10,
		0xE1, 0x33, 0x34, 0xE1, 0x8B, 0x26, 0xB9, 0x71,
		0xFD, 0xE1, 0x58, 0x87, 0xFD, 0xB2, 0xD1, 0xDA,
		0x9D, 0x93, 0x04, 0x67, 0x8A, 0xDC, 0x17, 0xDA,
		0xCB, 0x38, 0xE5, 0xB1, 0x81, 0xCF, 0x8A, 0xF9,
		0xDB, 0x51, 0xC5, 0x70, 0x2F, 0x26, 0x88, 0x7F,
		0x36, 0x65, 0xD4, 0xCD, 0x22, 0x67, 0xF9, 0x1D,
		0x72, 0x9F, 0xE1, 0xFE, 0x2B, 0xF8, 0xF6, 0x7D,
		0x3B, 0x2B, 0xEE, 0x84, 0x3F, 0xAF, 0xEF, 0x01,
		0x26, 0x0B, 0x21, 0x21, 0x52, 0x49, 0xE2, 0xFC,
		0x76, 0x46, 0x3B, 0x70, 0xA2, 0x94, 0x92, 0x1C,
		0xAF, 0x7D, 0x50, 0xE0, 0x2C, 0x6D, 0x83, 0xA9,
		0x09, 0x56, 0x21, 0x5D, 0x8D, 0x56, 0x26, 0x06,
		0x15, 0x63, 0x7E, 0xB3, 0x8C, 0x45, 0x6C, 0x7D,
		0x82, 0xBD, 0xA0, 0xC7, 0x8E, 0x58, 0xDD, 0xDA,
		0x70, 0xB8, 0x11, 0x83, 0x2E, 0x81, 0xBC, 0xCF,
		0xAD, 0x5D, 0x09, 0xDA, 0x60, 0x03, 0xE8, 0x9E,
		0x72, 0xF9, 0xAE, 0xDC, 0xAC, 0x56, 0x6F, 0xF8,
		0xEB, 0x44, 0x58, 0xBA, 0x95, 0xB9, 0x2B, 0x71,
		0x85, 0xD4, 0x58, 0x74, 0xA2, 0x54, 0xC4, 0x66,
		0x80, 0xE4, 0x67, 0xFE, 0x09, 0x70, 0xD5, 0x2A,
		0x05, 0x12, 0x63, 0x77, 0xFB, 0xD8, 0x19, 0x4B,
		0xA6, 0xBE, 0xFE, 0x01, 0x09, 0x7D, 0x52, 0x6A,
		0x76, 0x8B, 0x12, 0x76, 0x70, 0x67, 0xBA, 0xEC,
		0x26, 0x97, 0x6C, 0x60, 0xBD, 0x67, 0x8E, 0xD1,
		0xCA, 0x58, 0xD4, 0x87, 0x54, 0x8E, 0x7D, 0xAA,
		0x4A, 0x0F, 0xE2, 0x92, 0x13, 0xDF, 0x5E, 0x9F,
		0x11, 0xB7, 0x44, 0x70, 0x84, 0x04, 0x8F, 0x03,
		0xB4, 0xBB, 0x83, 0x87, 0x76, 0x1C, 0xC5, 0xB7,
		0xF2, 0xF8, 0x2C, 0xD4, 0xFF, 0xAC, 0xDA, 0xAD,
		0xAA, 0x59, 0xBA, 0xD9, 0xA9, 0x8D, 0x54, 0x37
	};

	static const char datab64[] =
"bd2FZrNktkH2cGnXIMsqEOEzNOGLJrlx/eFYh/2y0dqdkwRnitwX2ss45bGBz4r521HFcC8miH82"
"ZdTNImf5HXKf4f4r+PZ9OyvuhD+v7wEmCyEhUkni/HZGO3CilJIcr31Q4Cxtg6kJViFdjVYmBhVj"
"frOMRWx9gr2gx45Y3dpwuBGDLoG8z61dCdpgA+iecvmu3KxWb/jrRFi6lbkrcYXUWHSiVMRmgORn"
"/glw1SoFEmN3+9gZS6a+/gEJfVJqdosSdnBnuuwml2xgvWeO0cpY1IdUjn2qSg/ikhPfXp8Rt0Rw"
"hASPA7S7g4d2HMW38vgs1P+s2q2qWbrZqY1UNw==";

	size_t u;

	printf("Test base64: ");
	fflush(stdout);

	for (u = 0; u <= sizeof databin; u ++) {
		char text[(sizeof databin) << 1], tref[sizeof text];
		unsigned char data[sizeof databin];
		int ff;
		size_t len;

		/*
		 * Check test Base64 encoder against reference.
		 */
		b64enc(tref, databin, u, 1, 0, 0);
		if (u == sizeof databin) {
			check(strcmp(tref, datab64) == 0, "b64enc ref");
		} else {
			len = 4 * (u / 3);
			check(memcmp(tref, datab64, len) == 0, "b64enc ref");
		}

		/*
		 * Test encoder with all flag combinations and truncation.
		 */
		for (ff = 0; ff < 10; ff ++) {
			int pad, line_len, crlf;
			unsigned flags;
			size_t v;

			flags = 0;
			pad = ff / 5;
			if (!pad) {
				flags |= CTTK_B64ENC_NO_PAD;
			}
			switch (ff % 3) {
			case 0:
				line_len = 0;
				crlf = 0;
				break;
			case 1:
				line_len = 19;
				crlf = 0;
				flags |= CTTK_B64ENC_NEWLINE;
				break;
			case 2:
				line_len = 16;
				crlf = 0;
				flags |= CTTK_B64ENC_NEWLINE;
				flags |= CTTK_B64ENC_LINE64;
				break;
			case 3:
				line_len = 19;
				crlf = 1;
				flags |= CTTK_B64ENC_NEWLINE;
				flags |= CTTK_B64ENC_CRLF;
				break;
			case 4:
				line_len = 16;
				crlf = 1;
				flags |= CTTK_B64ENC_NEWLINE;
				flags |= CTTK_B64ENC_LINE64;
				flags |= CTTK_B64ENC_CRLF;
				break;
			}

			b64enc(tref, databin, u, pad, line_len, crlf);
			len = cttk_bintob64_gen(NULL, 0, databin, u, flags);
			check(len == strlen(tref), "b64enc (%zu, %d) 1", u, ff);
			len = cttk_bintob64_gen(text, sizeof text,
				databin, u, flags);
			check(len == strlen(tref), "b64enc (%zu, %d) 2", u, ff);
			check(len == strlen(text), "b64enc (%zu, %d) 3", u, ff);
			check(!strcmp(text, tref), "b64enc (%zu, %d) 4", u, ff);
			check(cttk_bintob64_gen(text, 0, databin,
				u, flags) == 0, "b64enc (%zu, %d) 5", u, ff);

			for (v = 0; v <= len; v ++) {
				size_t len2;

				len2 = cttk_bintob64_gen(text, v + 1,
					databin, u, flags);
				check(len2 == v,
					"b64enc (%zu, %d, %zu) 6", u, ff, v);
				check(len2 == strlen(text),
					"b64enc (%zu, %d, %zu) 7", u, ff, v);
				check(memcmp(text, tref, len2) == 0,
					"b64enc (%zu, %d, %zu) 8", u, ff, v);
			}
		}

		/*
		 * Test decoder with added whitespace and non-whitespace.
		 */
		for (ff = 0; ff < 2; ff ++) {
			size_t v;

			b64enc(tref + 1, databin, u, ff, 0, 0);
			len = strlen(tref + 1);
			for (v = 0; v <= len; v ++) {
				size_t len2, len3;
				const char *err;

				if (v == 0) {
					tref[0] = ' ';
				} else {
					tref[v - 1] = tref[v];
					tref[v] = ' ';
				}
				err = datab64;
				len2 = cttk_b64tobin_gen(data, sizeof data,
					tref, len + 1, &err,
					ff ? 0 : CTTK_B64DEC_NO_PAD);
				check(len2 == u,
					"b64dec (%zu, %d, %zu) 1", u, ff, v);
				check(err == NULL,
					"b64dec (%zu, %d, %zu) 2", u, ff, v);
				check(memcmp(data, databin, u) == 0,
					"b64dec (%zu, %d, %zu) 3", u, ff, v);

				err = datab64;
				len2 = cttk_b64tobin_gen(data, sizeof data,
					tref, len + 1, &err,
					(ff ? 0 : CTTK_B64DEC_NO_PAD)
					| CTTK_B64DEC_NO_WS);
				check(err == tref + v,
					"b64dec (%zu, %d, %zu) 4", u, ff, v);
				len3 = (v * 3) / 4;
				if (len3 > u) {
					len3 = u;
				}
				check(len2 == len3,
					"b64dec (%zu, %d, %zu) 5", u, ff, v);
				check(memcmp(data, databin, len2) == 0,
					"b64dec (%zu, %d, %zu) 6", u, ff, v);

				tref[v] = '%';
				err = datab64;
				len2 = cttk_b64tobin_gen(data, sizeof data,
					tref, len + 1, &err,
					ff ? 0 : CTTK_B64DEC_NO_PAD);
				check(err == tref + v,
					"b64dec (%zu, %d, %zu) 7", u, ff, v);
				len3 = (v * 3) / 4;
				if (len3 > u) {
					len3 = u;
				}
				check(len2 == len3,
					"b64dec (%zu, %d, %zu) 8", u, ff, v);
				check(memcmp(data, databin, len2) == 0,
					"b64dec (%zu, %d, %zu) 9", u, ff, v);
			}
		}

		/*
		 * Test decoder with a truncated input.
		 */
		for (ff = 0; ff < 2; ff ++) {
			size_t v;

			b64enc(tref, databin, u, ff, 0, 0);
			len = strlen(tref);
			for (v = 0; v <= len; v ++) {
				size_t len2, len3;
				const char *err, *err2;

				err = datab64;
				len2 = cttk_b64tobin_gen(data, sizeof data,
					tref, v, &err,
					ff ? 0 : CTTK_B64DEC_NO_PAD);
				len3 = (v * 3) / 4;
				if (len3 > u) {
					len3 = u;
				}
				check(len2 == len3,
					"b64trunc1 (%zu, %d, %zu) 1", u, ff, v);
				if (ff) {
					if (v % 4 == 0) {
						err2 = NULL;
					} else {
						err2 = tref + v;
					}
				} else {
					int ok;

					if (len3 == u) {
						ok = 1;
					} else if (v % 4 == 0) {
						ok = 1;
					} else if (v % 4 == 1) {
						ok = 0;
					} else if (v % 4 == 2) {
						ok = !(databin[len3] & 0xF0);
					} else {
						ok = !(databin[len3] & 0xC0);
					}
					err2 = ok ? NULL : tref + v;
				}
				check(err == err2,
					"b64trunc1 (%zu, %d, %zu) 2", u, ff, v);
				check(memcmp(data, databin, u) == 0,
					"b64trunc1 (%zu, %d, %zu) 3", u, ff, v);
			}
		}

		/*
		 * Test decoder with a truncated output.
		 */
		for (ff = 0; ff < 2; ff ++) {
			size_t v;

			b64enc(tref, databin, u, ff, 0, 0);
			len = strlen(tref);
			for (v = 0; v <= u; v ++) {
				size_t len2;
				const char *err, *err2;

				err = datab64;
				len2 = cttk_b64tobin_gen(data, v,
					tref, len, &err,
					ff ? 0 : CTTK_B64DEC_NO_PAD);
				check(len2 == v,
					"b64trunc2 (%zu, %d, %zu) 1", u, ff, v);
				err2 = (v == u) ? NULL : tref + (4 * v + 2) / 3;
				check(err == err2,
					"b64trunc2 (%zu, %d, %zu) 2", u, ff, v);
				check(memcmp(data, databin, v) == 0,
					"b64trunc2 (%zu, %d, %zu) 3", u, ff, v);
			}
		}

		if ((u & 7) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_mul(void)
{
	int i, j, k;
	uint32_t xu32, yu32;
	int32_t xs32, ys32;
	uint64_t xu64, yu64;
	int64_t xs64, ys64;

	printf("Test mul: ");
	fflush(stdout);

	for (i = 0; i < 32 * 64; i ++) {
		k = i >> 5;
		xu64 = (uint64_t)((i & 15) - 8);
		if (i < 32 * 4) {
			xu64 >>= k;
		} else {
			xu64 <<= k;
			xu64 ^= (uint64_t)((i >> 4) & 1) << (i >> 6);
		}
		xs64 = *(int64_t *)&xu64;
		xu32 = (uint32_t)xu64;
		xs32 = *(int32_t *)&xu32;
		for (j = 0; j < 32 * 64; j ++) {
			uint32_t ru32;
			int32_t rs32;
			uint64_t ru32w, ru64;
			int64_t rs32w, rs64;

			k = j >> 5;
			yu64 = (uint64_t)((j & 15) - 8);
			if (j < 32 * 4) {
				yu64 >>= k;
			} else {
				yu64 <<= k;
				yu64 ^= (uint64_t)((j >> 4) & 1) << (j >> 6);
			}
			ys64 = *(int64_t *)&yu64;
			yu32 = (uint32_t)yu64;
			ys32 = *(int32_t *)&yu32;

			ru32 = xu32 * yu32;
			rs32 = *(int32_t *)&ru32;
			ru32w = (uint64_t)xu32 * (uint64_t)yu32;
			rs32w = (int64_t)xs32 * (int64_t)ys32;
			ru64 = xu64 * yu64;
			rs64 = xs64 * ys64;

			check(cttk_mulu32(xu32, yu32) == ru32,
				"mulu32: %lu, %lu\n",
				(unsigned long)xu32,
				(unsigned long)yu32);
			check(cttk_muls32(xs32, ys32) == rs32,
				"muls32: %llu, %llu\n",
				(long)xs32,
				(long)ys32);
			check(cttk_mulu32w(xu32, yu32) == ru32w,
				"mulu32w: %lu, %lu\n",
				(unsigned long)xu32,
				(unsigned long)yu32);
			check(cttk_muls32w(xs32, ys32) == rs32w,
				"muls32w: %lu, %lu\n",
				(long)xs32,
				(long)ys32);
			check(cttk_mulu64(xu64, yu64) == ru64,
				"mulu64: %llu, %llu\n",
				(unsigned long long)xu64,
				(unsigned long long)yu64);
			check(cttk_muls64(xs64, ys64) == rs64,
				"muls64: %llu, %llu\n",
				(long long)xs64,
				(long long)ys64);
		}

		if ((i & 31) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_bitlength(void)
{
	int i, j;

	printf("Test bitlength: ");
	fflush(stdout);

	check(cttk_u32_bitlength(0) == 0, "bitlength 0");

	for (i = 0; i <= 31; i ++) {
		for (j = 0; j < 256; j ++) {
			uint32_t x, y, bl;

			x = (uint32_t)1 << i;
			if (j < 64) {
				y = (uint32_t)j;
				if (y < x) {
					x += y;
				}
			} else if (j < 128) {
				if (i >= 6) {
					y = (uint32_t)(j - 64);
					x += y << (i - 6);
				}
			} else if (j < 192) {
				y = (uint32_t)(j - 128);
				if (y < x) {
					x = (x << 1) - 1;
					x -= y;
				}
			} else {
				if (i >= 6) {
					x = (x << 1) - 1;
					y = (uint32_t)(j - 192);
					x -= y << (i - 6);
				}
			}

			bl = cttk_u32_bitlength(x);
			check(bl == (uint32_t)(i + 1),
				"bitlength: %08lX -> %u (%d,%d)",
				(unsigned long)x, (unsigned)bl, i, j);
		}
	}

	printf("done.\n");
	fflush(stdout);
}

/*
 * Custom rudimentary bigint implementation for test purposes.
 * Each integer is an array of 16-bit words, in little-endian order.
 * Maximum size is 512 bits.
 */
typedef struct {
	uint16_t v[32];
} zint;

static void
zint_zero(zint *z)
{
	memset(z->v, 0, sizeof z->v);
}

static void
zint_set_u64(zint *z, uint64_t x)
{
	memset(z->v, 0, sizeof z->v);
	z->v[0] = (uint16_t)x;
	z->v[1] = (uint16_t)(x >> 16);
	z->v[2] = (uint16_t)(x >> 32);
	z->v[3] = (uint16_t)(x >> 48);
}

/* unused
static void
zint_set_s64(zint *z, int64_t x)
{
	zint_set_u64(z, (uint64_t)x);
	if (x < 0) {
		memset(z->v + 4, 0xFF, (sizeof z->v) - 4 * sizeof(uint16_t));
	}
}
*/

static unsigned
zint_bitlength(const zint *z)
{
	unsigned w;
	int i;

	if (z->v[31] >= 0x8000) {
		w = 0xFFFF;
	} else {
		w = 0x0000;
	}
	for (i = 31; i >= 0; i --) {
		if (z->v[i] != w) {
			int j;
			unsigned x;

			x = z->v[i] ^ w;
			for (j = 15; j >= 0; j --) {
				if ((x >> j) != 0) {
					return (i << 4) + j + 1;
				}
			}
		}
	}
	return 0;
}

static uint64_t
zint_get_u64(const zint *z)
{
	return (uint64_t)z->v[0]
		| ((uint64_t)z->v[1] << 16)
		| ((uint64_t)z->v[2] << 32)
		| ((uint64_t)z->v[3] << 48);
}

static int64_t
zint_get_s64(const zint *z)
{
	uint64_t x;

	x = zint_get_u64(z);
	return *(int64_t *)&x;
}

static void
zint_trunc(zint *z, unsigned bl)
{
	unsigned k;
	uint16_t w;

	if (bl >= 512) {
		return;
	}
	if (bl == 0) {
		memset(z->v, 0, sizeof z->v);
		return;
	}
	bl --;
	k = bl >> 4;
	bl &= 15;
	w = z->v[k];
	if (((w >> bl) & 1) != 0) {
		z->v[k] |= (0xFFFF << bl) & 0xFFFF;
		for (k ++; k < 32; k ++) {
			z->v[k] = 0xFFFF;
		}
	} else {
		z->v[k] &= ~(0xFFFF << bl);
		for (k ++; k < 32; k ++) {
			z->v[k] = 0x0000;
		}
	}
}

static void
zint_copy(zint *d, const zint *s)
{
	memmove(d->v, s->v, sizeof s->v);
}

static void
zint_add(zint *d, const zint *a, const zint *b)
{
	int i;
	uint32_t cc;

	cc = 0;
	for (i = 0; i < 32; i ++) {
		uint32_t wa, wb, wd;

		wa = a->v[i];
		wb = b->v[i];
		wd = wa + wb + cc;
		cc = wd >> 16;
		d->v[i] = wd & 0xFFFF;
	}
}

static void
zint_sub(zint *d, const zint *a, const zint *b)
{
	int i;
	uint32_t cc;

	cc = 0;
	for (i = 0; i < 32; i ++) {
		uint32_t wa, wb, wd;

		wa = a->v[i];
		wb = b->v[i];
		wd = wa - wb - cc;
		cc = (wd >> 16) & 1;
		d->v[i] = wd & 0xFFFF;
	}
}

static int
zint_cmp(const zint *a, const zint *b)
{
	zint d;
	int i;

	zint_sub(&d, a, b);
	if (d.v[31] >= 0x8000) {
		return -1;
	}
	for (i = 0; i < 32; i ++) {
		if (d.v[i] != 0) {
			return 1;
		}
	}
	return 0;
}

static int
zint_sign(const zint *a)
{
	int i;

	if (a->v[31] >= 0x8000) {
		return -1;
	}
	for (i = 0; i < 32; i ++) {
		if (a->v[i] != 0) {
			return 1;
		}
	}
	return 0;
}

static void
zint_decode(zint *z, const void *src, size_t len, int be, int sig)
{
	const unsigned char *buf;
	size_t u;
	unsigned bx;

	zint_zero(z);
	if (len == 0) {
		return;
	}
	buf = src;
	if (sig) {
		if (be) {
			bx = buf[0] >= 0x80 ? 0xFF : 0x00;
		} else {
			bx = buf[len - 1] >= 0x80 ? 0xFF : 0x00;
		}
	} else {
		bx = 0x00;
	}
	for (u = 0; u < 64; u ++) {
		unsigned b;

		if (u < len) {
			b = be ? buf[len - 1 - u] : buf[u];
		} else {
			b = bx;
		}
		if ((u & 1) != 0) {
			b <<= 8;
		}
		z->v[u >> 1] |= b;
	}
}

static void
zint_encode(void *dst, size_t len, const zint *z, int be)
{
	unsigned char *buf;
	size_t u;
	unsigned ssx;

	buf = dst;
	ssx = (z->v[31] >= 0x8000) ? 0xFF : 0x00;
	for (u = 0; u < len; u ++) {
		unsigned b;

		if (u < 64) {
			b = z->v[u >> 1];
			if ((u & 1) != 0) {
				b >>= 8;
			}
		} else {
			b = ssx;
		}
		if (be) {
			buf[len - 1 - u] = b;
		} else {
			buf[u] = b;
		}
	}
}

static void
zint_mul(zint *d, zint *a, zint *b)
{
	zint x, t;
	int i, j;

	zint_zero(&t);
	for (i = 0; i < 32; i ++) {
		uint32_t wb, cc;

		wb = b->v[i];
		memset(x.v, 0, i * sizeof(uint16_t));
		cc = 0;
		for (j = i; j < 32; j ++) {
			uint32_t wa, wx;

			wa = a->v[j - i];
			wx = wa * wb + cc;
			cc = wx >> 16;
			x.v[j] = (uint16_t)wx;
		}
		zint_add(&t, &t, &x);
	}
	zint_copy(d, &t);
}

static void
zint_lsh(zint *d, zint *a, unsigned n)
{
	zint t;
	int i, nd, nm;

	nd = (int)(n >> 4);
	nm = (int)(n & 15);
	for (i = 0; i < 32; i ++) {
		if (i < nd) {
			t.v[i] = 0;
		} else if (i == nd) {
			t.v[i] = (uint16_t)((uint32_t)a->v[0] << nm);
		} else {
			t.v[i] = (uint16_t)(((uint32_t)a->v[i - nd] << nm)
				| ((uint32_t)a->v[i - nd - 1] >> (16 - nm)));
		}
	}
	zint_copy(d, &t);
}

static void
zint_rsh(zint *d, zint *a, unsigned n)
{
	int i, nd, nm;
	uint32_t ssa;

	nd = (int)(n >> 4);
	nm = (int)(n & 15);
	ssa = (a->v[31] >= 0x8000) ? 0xFFFF : 0x0000;
	for (i = 0; i < 32; i ++) {
		if (i + nd < 31) {
			d->v[i] = (uint16_t)(((uint32_t)a->v[i + nd] >> nm)
				| ((uint32_t)a->v[i + nd + 1] << (16 - nm)));
		} else if (i + nd == 31) {
			d->v[i] = (uint16_t)(((uint32_t)a->v[31] >> nm)
				| (ssa << (16 - nm)));
		} else {
			d->v[i] = ssa;
		}
	}
}

static void
zint_and(zint *d, const zint *a, const zint *b)
{
	int i;

	for (i = 0; i < 32; i ++) {
		d->v[i] = a->v[i] & b->v[i];
	}
}

static void
zint_or(zint *d, const zint *a, const zint *b)
{
	int i;

	for (i = 0; i < 32; i ++) {
		d->v[i] = a->v[i] | b->v[i];
	}
}

static void
zint_xor(zint *d, const zint *a, const zint *b)
{
	int i;

	for (i = 0; i < 32; i ++) {
		d->v[i] = a->v[i] ^ b->v[i];
	}
}

static void
zint_eqv(zint *d, const zint *a, const zint *b)
{
	int i;

	for (i = 0; i < 32; i ++) {
		d->v[i] = ~(a->v[i] ^ b->v[i]);
	}
}

static void
zint_not(zint *d, const zint *a)
{
	int i;

	for (i = 0; i < 32; i ++) {
		d->v[i] = ~a->v[i];
	}
}

/*
 * PRNG. Not crypto-quality, but "randomish" enough for generating test
 * values.
 */
static uint32_t rnd_state[16];

static void
rnd_init(uint64_t seed)
{
	int i;

	for (i = 0; i < 16; i ++) {
		rnd_state[i] = (uint32_t)seed;
		seed = (seed * 0x2F25F7F336563959) + 0xEC2BAAED5DF4DC7D;
	}
}

static void
rnd_step(void)
{
	int i;

	for (i = 0; i < 10; i ++) {

#define QROUND(a, b, c, d)   do { \
		rnd_state[a] += rnd_state[b]; \
		rnd_state[d] ^= rnd_state[a]; \
		rnd_state[d] = (rnd_state[d] << 16) | (rnd_state[d] >> 16); \
		rnd_state[c] += rnd_state[d]; \
		rnd_state[b] ^= rnd_state[c]; \
		rnd_state[b] = (rnd_state[b] << 12) | (rnd_state[b] >> 20); \
		rnd_state[a] += rnd_state[b]; \
		rnd_state[d] ^= rnd_state[a]; \
		rnd_state[d] = (rnd_state[d] <<  8) | (rnd_state[d] >> 24); \
		rnd_state[c] += rnd_state[d]; \
		rnd_state[b] ^= rnd_state[c]; \
		rnd_state[b] = (rnd_state[b] <<  7) | (rnd_state[b] >> 25); \
	} while (0)

		QROUND( 0,  4,  8, 12);
		QROUND( 1,  5,  9, 13);
		QROUND( 2,  6, 10, 14);
		QROUND( 3,  7, 11, 15);
		QROUND( 0,  5, 10, 15);
		QROUND( 1,  6, 11, 12);
		QROUND( 2,  7,  8, 13);
		QROUND( 3,  4,  9, 14);

#undef QROUND

	}
}

static uint32_t
rnd32(void)
{
	rnd_step();
	return rnd_state[0];
}

static uint64_t
rnd64(void)
{
	rnd_step();
	return (uint64_t)rnd_state[0] + ((uint64_t)rnd_state[1] << 32);
}

static void
rnd(void *dst, size_t len)
{
	unsigned char *buf;
	size_t u;
	uint64_t x;

	buf = dst;
	x = 0;
	for (u = 0; u < len; u ++) {
		if ((u & 7) == 0) {
			x = rnd64();
		}
		buf[u] = (unsigned char)x;
		x >>= 8;
	}
}

static void
test_i31_set(void)
{
	cttk_i31_def(x, 500);
	int i, j;
	zint z;

	printf("Test i31 set: ");
	fflush(stdout);

	rnd_init(1);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x, i);

		for (j = 0; j < 64; j ++) {
			uint64_t tu64;
			uint32_t tu32;

			tu64 = (uint64_t)1 << j;
			cttk_i31_set_u64(x, tu64);
			if (j < (i - 1)) {
				check(cttk_i31_to_u64(x) == tu64,
					"set_u64 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u64(x) == 0,
					"set_u64 2 (%d,%d)", i, j);
			}
			tu64 = (tu64 << 1) - 1;
			cttk_i31_set_u64(x, tu64);
			if (j < (i - 1)) {
				check(cttk_i31_to_u64(x) == tu64,
					"set_u64 3 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u64(x) == 0,
					"set_u64 4 (%d,%d)", i, j);
			}
			cttk_i31_set_u64_trunc(x, tu64);
			if (j < (i - 1)) {
				check(cttk_i31_to_u64_trunc(x) == tu64,
					"set_u64 5 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u64_trunc(x) == (uint64_t)-1,
					"set_u64 6 (%d,%d)", i, j);
			}

			if (j >= 32) {
				continue;
			}

			tu32 = (uint32_t)1 << j;
			cttk_i31_set_u32(x, tu32);
			if (j < (i - 1)) {
				check(cttk_i31_to_u32(x) == tu32,
					"set_u32 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u32(x) == 0,
					"set_u32 2 (%d,%d)", i, j);
			}
			tu32 = (tu32 << 1) - 1;
			cttk_i31_set_u32(x, tu32);
			if (j < (i - 1)) {
				check(cttk_i31_to_u32(x) == tu32,
					"set_u32 3 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u32(x) == 0,
					"set_u32 4 (%d,%d)", i, j);
			}
			cttk_i31_set_u32_trunc(x, tu32);
			if (j < (i - 1)) {
				check(cttk_i31_to_u32_trunc(x) == tu32,
					"set_u32 5 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u32_trunc(x) == (uint32_t)-1,
					"set_u32 6 (%d,%d)", i, j);
			}
		}

		for (j = 0; j < 100; j ++) {
			uint64_t tu64;
			uint32_t tu32;
			int64_t ts64;
			int32_t ts32;

			tu64 = rnd64();
			ts64 = *(int64_t *)&tu64;
			tu32 = rnd32();
			ts32 = *(int32_t *)&tu32;

			cttk_i31_set_u64_trunc(x, tu64);
			zint_set_u64(&z, tu64);
			zint_trunc(&z, i);
			if (i <= 64 && zint_get_s64(&z) < 0) {
				check(cttk_i31_to_u64(x) == 0,
					"to_u64 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u64(x) == zint_get_u64(&z),
					"to_u64 2 (%d,%d)", i, j);
			}
			if (i >= 65 && tu64 >= ((uint64_t)1 << 63)) {
				check(cttk_i31_to_s64(x) == 0,
					"to_s64 1 (%d,%d)", i, j);
				check(cttk_i31_to_s64_trunc(x) == ts64,
					"to_s64_trunc 1 (%d,%d)", i, j);
				check(cttk_i31_to_u64_trunc(x) == tu64,
					"to_u64_trunc 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_s64(x) == zint_get_s64(&z),
					"to_s64 2 (%d,%d)", i, j);
				check(cttk_i31_to_s64_trunc(x)
					== zint_get_s64(&z),
					"to_s64_trunc 2 (%d,%d)", i, j);
				check(cttk_i31_to_u64_trunc(x)
					== zint_get_u64(&z),
					"to_u64_trunc 2 (%d,%d)", i, j);
			}

			cttk_i31_set_u32_trunc(x, tu32);
			zint_set_u64(&z, tu32);
			zint_trunc(&z, i);
			if (zint_get_s64(&z) < 0) {
				check(cttk_i31_to_u32(x) == 0,
					"to_u32 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_u32(x) == zint_get_u64(&z),
					"to_u32 2 (%d,%d)", i, j);
			}
			if (i >= 33 && tu32 >= ((uint32_t)1 << 31)) {
				check(cttk_i31_to_s32(x) == 0,
					"to_s32 1 (%d,%d)", i, j);
				check(cttk_i31_to_s32_trunc(x) == ts32,
					"to_s32_trunc 1 (%d,%d)", i, j);
				check(cttk_i31_to_u32_trunc(x) == tu32,
					"to_u32_trunc 1 (%d,%d)", i, j);
			} else {
				check(cttk_i31_to_s32(x) == zint_get_s64(&z),
					"to_s32 2 (%d,%d)", i, j);
				check(cttk_i31_to_s32_trunc(x)
					== zint_get_s64(&z),
					"to_s32_trunc 2 (%d,%d)", i, j);
				check(cttk_i31_to_u32_trunc(x)
					== (uint32_t)zint_get_u64(&z),
					"to_u32_trunc 2 (%d,%d)", i, j);
			}
		}

		if ((i & 7) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static int
memiszero(const void *data, size_t len)
{
	unsigned x;
	const unsigned char *buf;

	buf = data;
	x = 0;
	while (len -- > 0) {
		x |= *buf ++;
	}
	return x == 0;
}

static void
test_i31_codec(void)
{
	cttk_i31_def(x, 300);
	zint z;
	int i, j, be;
	unsigned char tmp1[100], tmp2[100];

	printf("Test i31 codec: ");
	fflush(stdout);

	rnd_init(3);

	for (be = 0; be < 2; be ++) {
		for (i = 1; i <= 128; i ++) {
			cttk_i31_init(x, i);

			for (j = 0; j < 100; j ++) {
				rnd(tmp1, 17);
				zint_decode(&z, tmp1, 17, be, 0);
				zint_trunc(&z, i + 2);
				zint_encode(tmp1, 17, &z, be);

				if (be) {
					cttk_i31_decbe_signed(x, tmp1, 17);
					cttk_i31_encbe(tmp2, 17, x);
				} else {
					cttk_i31_decle_signed(x, tmp1, 17);
					cttk_i31_encle(tmp2, 17, x);
				}
				if (zint_bitlength(&z) < (unsigned)i) {
					check(memcmp(tmp2, tmp1, 17) == 0,
						"codec 1 (%d,%d,%d)", be, i, j);
				} else {
					check(cttk_bool_to_int(
						cttk_i31_isnan(x)),
						"codec 2 (%d,%d,%d)", be, i, j);
					check(memiszero(tmp2, 17),
						"codec 3 (%d,%d,%d)", be, i, j);
				}
				if (be) {
					cttk_i31_decbe_unsigned(x, tmp1, 17);
					cttk_i31_encbe(tmp2, 17, x);
				} else {
					cttk_i31_decle_unsigned(x, tmp1, 17);
					cttk_i31_encle(tmp2, 17, x);
				}
				if (zint_bitlength(&z) < (unsigned)i
					&& z.v[31] < 0x8000)
				{
					check(memcmp(tmp2, tmp1, 17) == 0,
						"codec 4 (%d,%d,%d)", be, i, j);
				} else {
					check(cttk_bool_to_int(
						cttk_i31_isnan(x)),
						"codec 5 (%d,%d,%d)", be, i, j);
					check(memiszero(tmp2, 17),
						"codec 6 (%d,%d,%d)", be, i, j);
				}

				rnd(tmp1, 17);
				if (be) {
					cttk_i31_decbe_signed_trunc(
						x, tmp1, 17);
					cttk_i31_encbe(tmp2, 17, x);
				} else {
					cttk_i31_decle_signed_trunc(
						x, tmp1, 17);
					cttk_i31_encle(tmp2, 17, x);
				}
				zint_decode(&z, tmp1, 17, be, 0);
				zint_trunc(&z, i);
				zint_encode(tmp1, 17, &z, be);
				check(memcmp(tmp1, tmp2, 17) == 0,
					"codec 7 (%d,%d,%d)", be, i, j);

				rnd(tmp1, 8);
				if (be) {
					cttk_i31_decbe_unsigned_trunc(
						x, tmp1, 8);
					cttk_i31_encbe(tmp2, 17, x);
				} else {
					cttk_i31_decle_unsigned_trunc(
						x, tmp1, 8);
					cttk_i31_encle(tmp2, 17, x);
				}
				zint_decode(&z, tmp1, 8, be, 0);
				zint_trunc(&z, i);
				zint_encode(tmp1, 17, &z, be);
				check(memcmp(tmp1, tmp2, 17) == 0,
					"codec 8 (%d,%d,%d)", be, i, j);
			}

			if ((i & 7) == 0) {
				printf(".");
				fflush(stdout);
			}
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_cmp(void)
{
	cttk_i31_def(x1, 300);
	cttk_i31_def(x2, 300);
	zint z1, z2;
	int i, j;
	unsigned char tmp1[100], tmp2[100];

	printf("Test i31 codec: ");
	fflush(stdout);

	rnd_init(4);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x1, i);
		cttk_i31_init(x2, i);

		check(!cttk_bool_to_int(cttk_i31_eq0(x1)),
			"cmp nan1 1 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_neq0(x1)),
			"cmp nan1 2 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_lt0(x1)),
			"cmp nan1 3 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_leq0(x1)),
			"cmp nan1 4 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_gt0(x1)),
			"cmp nan1 5 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_geq0(x1)),
			"cmp nan1 6 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_eq(x1, x2)),
			"cmp nan1 7 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_neq(x1, x2)),
			"cmp nan1 8 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_lt(x1, x2)),
			"cmp nan1 9 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_leq(x1, x2)),
			"cmp nan1 10 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_gt(x1, x2)),
			"cmp nan1 11 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_geq(x1, x2)),
			"cmp nan1 12 (%d)", i);

		cttk_i31_set_s32(x1, 0);
		check(cttk_bool_to_int(cttk_i31_eq0(x1)),
			"cmp eq0 1 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_neq0(x1)),
			"cmp neq0 2 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_lt0(x1)),
			"cmp lt0 3 (%d)", i);
		check(cttk_bool_to_int(cttk_i31_leq0(x1)),
			"cmp leq0 4 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_gt0(x1)),
			"cmp gt0 5 (%d)", i);
		check(cttk_bool_to_int(cttk_i31_geq0(x1)),
			"cmp geq0 6 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_eq(x1, x2)),
			"cmp nan2 1 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_neq(x1, x2)),
			"cmp nan2 2 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_lt(x1, x2)),
			"cmp nan2 3 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_leq(x1, x2)),
			"cmp nan2 4 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_gt(x1, x2)),
			"cmp nan2 5 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_geq(x1, x2)),
			"cmp nan2 6 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_eq(x2, x1)),
			"cmp nan2 7 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_neq(x2, x1)),
			"cmp nan2 8 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_lt(x2, x1)),
			"cmp nan2 9 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_leq(x2, x1)),
			"cmp nan2 10 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_gt(x2, x1)),
			"cmp nan2 11 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_geq(x2, x1)),
			"cmp nan2 12 (%d)", i);

		for (j = 0; j < 100; j ++) {
			int cc;

			rnd(tmp1, 17);
			rnd(tmp2, 17);
			zint_decode(&z1, tmp1, 17, 0, 0);
			zint_decode(&z2, tmp2, 17, 0, 0);
			zint_trunc(&z1, i);
			zint_trunc(&z2, i);
			zint_encode(tmp1, 17, &z1, 0);
			zint_encode(tmp2, 17, &z2, 0);
			cttk_i31_decle_signed(x1, tmp1, 17);
			cttk_i31_decle_signed(x2, tmp2, 17);

			cc = zint_cmp(&z1, &z2);
			check(cttk_i31_cmp(x1, x2) == cc,
				"cmp 1 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_lt(x1, x2))
				== (cc < 0), "cmp 2 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_leq(x1, x2))
				== (cc <= 0), "cmp 3 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_gt(x1, x2))
				== (cc > 0), "cmp 4 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_geq(x1, x2))
				== (cc >= 0), "cmp 5 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_eq(x1, x1)),
				"cmp 6 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_geq(x1, x1)),
				"cmp 7 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_leq(x1, x1)),
				"cmp 8 (%d,%d)", i, j);
			check(!cttk_bool_to_int(cttk_i31_neq(x1, x1)),
				"cmp 9 (%d,%d)", i, j);
			check(!cttk_bool_to_int(cttk_i31_gt(x1, x1)),
				"cmp 10 (%d,%d)", i, j);
			check(!cttk_bool_to_int(cttk_i31_lt(x1, x1)),
				"cmp 11 (%d,%d)", i, j);

			cc = zint_sign(&z1);
			check(cttk_bool_to_int(cttk_i31_lt0(x1)) == (cc < 0),
				"cmp 12 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_leq0(x1)) == (cc <= 0),
				"cmp 13 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_gt0(x1)) == (cc > 0),
				"cmp 14 (%d,%d)", i, j);
			check(cttk_bool_to_int(cttk_i31_geq0(x1)) == (cc >= 0),
				"cmp 15 (%d,%d)", i, j);
		}

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_addsub(void)
{
	cttk_i31_def(x1, 300);
	cttk_i31_def(x2, 300);
	cttk_i31_def(x3, 300);
	zint z1, z2, z3;
	int i, j;
	unsigned char tmp1[100], tmp2[100], tmp3[100], tmp4[100];

	printf("Test i31 add/sub: ");
	fflush(stdout);

	rnd_init(5);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x1, i);
		cttk_i31_init(x2, i);
		cttk_i31_init(x3, i);

		for (j = 0; j < 100; j ++) {
			rnd(tmp1, 17);
			rnd(tmp2, 17);
			zint_decode(&z1, tmp1, 17, 0, 0);
			zint_decode(&z2, tmp2, 17, 0, 0);
			zint_trunc(&z1, i);
			zint_trunc(&z2, i);
			zint_encode(tmp1, 17, &z1, 0);
			zint_encode(tmp2, 17, &z2, 0);
			cttk_i31_decle_signed(x1, tmp1, 17);
			cttk_i31_decle_signed(x2, tmp2, 17);

			zint_add(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_add(x3, x1, x2);
			if (zint_bitlength(&z3) >= (uint32_t)i) {
				check(cttk_bool_to_int(cttk_i31_isnan(x3)),
					"add 1 (%d,%d)", i, j);
			} else {
				cttk_i31_encle(tmp4, 17, x3);
				check(memcmp(tmp3, tmp4, 17) == 0,
					"add 2 (%d,%d)", i, j);
			}
			zint_trunc(&z3, i);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_add_trunc(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"add 3 (%d,%d)", i, j);

			zint_sub(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_sub(x3, x1, x2);
			if (zint_bitlength(&z3) >= (uint32_t)i) {
				check(cttk_bool_to_int(cttk_i31_isnan(x3)),
					"sub 1 (%d,%d)", i, j);
			} else {
				cttk_i31_encle(tmp4, 17, x3);
				check(memcmp(tmp3, tmp4, 17) == 0,
					"sub 2 (%d,%d)", i, j);
			}
			zint_trunc(&z3, i);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_sub_trunc(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"sub 3 (%d,%d)", i, j);

			zint_zero(&z1);
			zint_sub(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_neg(x3, x2);
			if (zint_bitlength(&z3) >= (uint32_t)i) {
				check(cttk_bool_to_int(cttk_i31_isnan(x3)),
					"neg 1 (%d,%d)", i, j);
			} else {
				cttk_i31_encle(tmp4, 17, x3);
				check(memcmp(tmp3, tmp4, 17) == 0,
					"neg 2 (%d,%d)", i, j);
			}
			zint_trunc(&z3, i);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_neg_trunc(x3, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"neg 3 (%d,%d)", i, j);
		}

		memset(tmp1, 0, 17);
		tmp1[(i - 1) >> 3] |= 1 << ((i - 1) & 7);
		cttk_i31_decle_unsigned_trunc(x1, tmp1, 17);
		check(cttk_bool_to_int(cttk_i31_lt0(x1)),
			"negmin 1 (%d)", i);
		cttk_i31_neg(x3, x1);
		check(cttk_bool_to_int(cttk_i31_isnan(x3)),
			"negmin 2 (%d)", i);
		cttk_i31_neg_trunc(x3, x1);
		check(!cttk_bool_to_int(cttk_i31_isnan(x3)),
			"negmin 3 (%d)", i);
		check(cttk_bool_to_int(cttk_i31_eq(x1, x3)),
			"negmin 4 (%d)", i);

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_mul(void)
{
	cttk_i31_def(x1, 300);
	cttk_i31_def(x2, 300);
	cttk_i31_def(x3, 300);
	zint z1, z2, z3;
	int i, j;
	unsigned char tmp1[100], tmp2[100], tmp3[100], tmp4[100];

	printf("Test i31 mul: ");
	fflush(stdout);

	rnd_init(5);

	cttk_i31_init(x1, 1);
	cttk_i31_init(x2, 1);
	cttk_i31_init(x3, 1);
	cttk_i31_set_s32(x1, 0);
	cttk_i31_set_s32(x2, -1);
	cttk_i31_mul(x3, x1, x2);
	check(!cttk_bool_to_int(cttk_i31_isnan(x3)), "mul0-1");
	check(cttk_i31_to_s32(x3) == 0, "mul0-2");
	cttk_i31_mul(x3, x2, x1);
	check(!cttk_bool_to_int(cttk_i31_isnan(x3)), "mul0-3");
	check(cttk_i31_to_s32(x3) == 0, "mul0-4");

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x1, i);
		cttk_i31_init(x2, i);
		cttk_i31_init(x3, i);

		for (j = 0; j < 1000; j ++) {
			rnd(tmp1, 17);
			rnd(tmp2, 17);
			zint_decode(&z1, tmp1, 17, 0, 0);
			zint_decode(&z2, tmp2, 17, 0, 0);
			zint_trunc(&z1, i);
			zint_trunc(&z2, i);
			zint_encode(tmp1, 17, &z1, 0);
			zint_encode(tmp2, 17, &z2, 0);
			cttk_i31_decle_signed(x1, tmp1, 17);
			cttk_i31_decle_signed(x2, tmp2, 17);

			zint_mul(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_mul(x3, x1, x2);
			if (zint_bitlength(&z3) >= (uint32_t)i) {
				check(cttk_bool_to_int(cttk_i31_isnan(x3)),
					"mul 1 (%d,%d)", i, j);
			} else {
				check(!cttk_bool_to_int(cttk_i31_isnan(x3)),
					"mul 2 (%d,%d)", i, j);
				cttk_i31_encle(tmp4, 17, x3);
				check(memcmp(tmp3, tmp4, 17) == 0,
					"mul 3 (%d,%d)", i, j);
			}
			zint_trunc(&z3, i);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_mul_trunc(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(!cttk_bool_to_int(cttk_i31_isnan(x3)),
				"mul 4 (%d,%d)", i, j);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"mul 5 (%d,%d)", i, j);

			rnd(tmp1, 17);
			rnd(tmp2, 17);
			zint_decode(&z1, tmp1, 17, 0, 0);
			zint_decode(&z2, tmp2, 17, 0, 0);
			zint_trunc(&z1, (i + 2) >> 1);
			zint_trunc(&z2, (i + 2) >> 1);
			zint_encode(tmp1, 17, &z1, 0);
			zint_encode(tmp2, 17, &z2, 0);
			cttk_i31_decle_signed(x1, tmp1, 17);
			cttk_i31_decle_signed(x2, tmp2, 17);

			zint_mul(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_mul(x3, x1, x2);
			if (zint_bitlength(&z3) >= (uint32_t)i) {
				check(cttk_bool_to_int(cttk_i31_isnan(x3)),
					"mul 6 (%d,%d)", i, j);
			} else {
				check(!cttk_bool_to_int(cttk_i31_isnan(x3)),
					"mul 7 (%d,%d)", i, j);
				cttk_i31_encle(tmp4, 17, x3);
				check(memcmp(tmp3, tmp4, 17) == 0,
					"mul 8 (%d,%d)", i, j);
			}
			zint_trunc(&z3, i);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_mul_trunc(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(!cttk_bool_to_int(cttk_i31_isnan(x3)),
				"mul 9 (%d,%d)", i, j);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"mul 10 (%d,%d)", i, j);

			cttk_i31_mul_trunc(x1, x1, x2);
			cttk_i31_encle(tmp4, 17, x1);
			check(!cttk_bool_to_int(cttk_i31_isnan(x1)),
				"mul 11 (%d,%d)", i, j);
			check(memcmp(tmp3, tmp4, 17) == 0,
				"mul 12 (%d,%d)", i, j);
		}

		memset(tmp1, 0, 17);
		for (j = i - 1; j < 17 * 8; j ++) {
			tmp1[j >> 3] |= 1 << (j & 7);
		}
		memset(tmp2, 0xFF, 17);
		cttk_i31_decle_signed(x1, tmp1, 17);
		cttk_i31_decle_signed(x2, tmp2, 17);
		check(!cttk_bool_to_int(cttk_i31_isnan(x1)), "mul 13 (%d)", i);
		check(!cttk_bool_to_int(cttk_i31_isnan(x2)), "mul 14 (%d)", i);
		cttk_i31_mul(x3, x1, x2);
		check(cttk_bool_to_int(cttk_i31_isnan(x3)), "mul 15 (%d)", i);
		cttk_i31_mul_trunc(x3, x1, x2);
		check(!cttk_bool_to_int(cttk_i31_isnan(x3)), "mul 16 (%d)", i);
		check(cttk_bool_to_int(cttk_i31_eq(x3, x1)), "mul 17 (%d)", i);

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_shift(void)
{
	cttk_i31_def(x1, 300);
	cttk_i31_def(x2, 300);
	zint z1, z2;
	int i, j, k;
	unsigned char tmp1[100], tmp2[100], tmp3[100];

	printf("Test i31 shift: ");
	fflush(stdout);

	rnd_init(6);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x1, i);
		cttk_i31_init(x2, i);

		for (j = 0; j <= i + 32; j ++) {
			for (k = 0; k < 50; k ++) {
				rnd(tmp1, 17);
				zint_decode(&z1, tmp1, 17, 0, 0);
				zint_trunc(&z1, i);
				zint_encode(tmp1, 17, &z1, 0);
				cttk_i31_decle_signed(x1, tmp1, 17);

				zint_lsh(&z2, &z1, j);
				zint_encode(tmp2, 17, &z2, 0);
				cttk_i31_lsh(x2, x1, j);
				if (zint_bitlength(&z2) >= (uint32_t)i) {
					check(cttk_bool_to_int(
						cttk_i31_isnan(x2)),
						"lsh 1 (%d,%d,%d)", i, j, k);
				} else {
					check(!cttk_bool_to_int(
						cttk_i31_isnan(x2)),
						"lsh 2 (%d,%d,%d)", i, j, k);
					cttk_i31_encle(tmp3, 17, x2);
					check(memcmp(tmp2, tmp3, 17) == 0,
						"lsh 3 (%d,%d,%d)", i, j, k);
				}

				cttk_i31_lsh_prot(x2, x1, j);
				if (zint_bitlength(&z2) >= (uint32_t)i) {
					check(cttk_bool_to_int(
						cttk_i31_isnan(x2)),
						"lsh 4 (%d,%d,%d)", i, j, k);
				} else {
					check(!cttk_bool_to_int(
						cttk_i31_isnan(x2)),
						"lsh 5 (%d,%d,%d)", i, j, k);
					cttk_i31_encle(tmp3, 17, x2);
					check(memcmp(tmp2, tmp3, 17) == 0,
						"lsh 6 (%d,%d,%d)", i, j, k);
				}

				zint_trunc(&z2, i);
				zint_encode(tmp2, 17, &z2, 0);
				cttk_i31_lsh_trunc(x2, x1, j);
				cttk_i31_encle(tmp3, 17, x2);
				check(memcmp(tmp2, tmp3, 17) == 0,
					"lsh 7 (%d,%d,%d)", i, j, k);
				cttk_i31_lsh_trunc_prot(x2, x1, j);
				cttk_i31_encle(tmp3, 17, x2);
				check(memcmp(tmp2, tmp3, 17) == 0,
					"lsh 8 (%d,%d,%d)", i, j, k);

				zint_rsh(&z2, &z1, j);
				zint_encode(tmp2, 17, &z2, 0);
				cttk_i31_rsh(x2, x1, j);
				cttk_i31_encle(tmp3, 17, x2);
				check(memcmp(tmp2, tmp3, 17) == 0,
					"rsh 1 (%d,%d,%d)", i, j, k);
				cttk_i31_rsh_prot(x2, x1, j);
				cttk_i31_encle(tmp3, 17, x2);
				check(memcmp(tmp2, tmp3, 17) == 0,
					"rsh 2 (%d,%d,%d)", i, j, k);
			}
		}

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_div(void)
{
	cttk_i31_def(a, 300);
	cttk_i31_def(b, 300);
	cttk_i31_def(q, 300);
	cttk_i31_def(r, 300);
	cttk_i31_def(m, 300);
	cttk_i31_def(x, 300);
	int i, j;
	unsigned char tmp1[100], tmp2[100];

	printf("Test i31 div: ");
	fflush(stdout);

	rnd_init(7);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(a, i);
		cttk_i31_init(b, i);
		cttk_i31_init(q, i);
		cttk_i31_init(r, i);
		cttk_i31_init(m, i);
		cttk_i31_init(x, i);

		for (j = 0; j < 1000; j ++) {
			cttk_bool bb;

			rnd(tmp1, 17);
			rnd(tmp2, 17);
			cttk_i31_decle_signed_trunc(a, tmp1, 17);
			cttk_i31_decle_signed_trunc(b, tmp2, 17);
			if (j >= 500) {
				cttk_i31_rsh(b, b, i >> 1);
			}

			/*
			 * Set bb to true if b == -1 and a == MinValue.
			 */
			if (i >= 2) {
				cttk_i31_set_u32_trunc(x, 1);
				cttk_i31_add(x, x, b);
				bb = cttk_i31_eq0(x);
				cttk_i31_lsh_trunc(x, a, 1);
				bb = cttk_and(bb, cttk_and(cttk_i31_eq0(x),
					cttk_i31_neq0(a)));
			} else {
				bb = cttk_bool_of_u32(i == 1
					&& (tmp1[0] & tmp2[0] & 1) == 1);
			}

			cttk_i31_divrem(q, r, a, b);

			if (cttk_bool_to_int(cttk_i31_eq0(b))) {
				check(cttk_bool_to_int(cttk_i31_isnan(q)),
					"div 1 (%d,%d)", i, j);
				check(cttk_bool_to_int(cttk_i31_isnan(r)),
					"div 2 (%d,%d)", i, j);
			} else if (cttk_bool_to_int(bb)) {
				check(cttk_bool_to_int(cttk_i31_isnan(q)),
					"div 3 (%d,%d)", i, j);
				check(cttk_bool_to_int(cttk_i31_eq0(r)),
					"div 4 (%d,%d)", i, j);
			} else if (cttk_bool_to_int(cttk_i31_lt0(a))) {
				check(cttk_bool_to_int(cttk_i31_leq0(r)),
					"div 5 (%d,%d)", i, j);
				if (cttk_bool_to_int(cttk_i31_lt0(b))) {
					check(cttk_bool_to_int(
						cttk_i31_lt(b, r)),
						"div 6 (%d,%d)", i, j);
				} else {
					cttk_i31_neg(x, r);
					check(cttk_bool_to_int(
						cttk_i31_lt(x, b)),
						"div 7 (%d,%d)", i, j);
				}
				cttk_i31_mul(x, b, q);
				cttk_i31_add(x, x, r);
				check(cttk_bool_to_int(cttk_i31_eq(x, a)),
					"div 8 (%d,%d)", i, j);
			} else {
				check(cttk_bool_to_int(cttk_i31_geq0(r)),
					"div 9 (%d,%d)", i, j);
				if (cttk_bool_to_int(cttk_i31_lt0(b))) {
					cttk_i31_neg(x, r);
					check(cttk_bool_to_int(
						cttk_i31_lt(b, x)),
						"div 10 (%d,%d)", i, j);
				} else {
					check(cttk_bool_to_int(
						cttk_i31_lt(r, b)),
						"div 11 (%d,%d)", i, j);
				}
				cttk_i31_mul(x, b, q);
				cttk_i31_add(x, x, r);
				check(cttk_bool_to_int(cttk_i31_eq(x, a)),
					"div 12 (%d,%d)", i, j);
			}

			cttk_i31_mod(m, a, b);

			if (cttk_bool_to_int(cttk_i31_eq0(b))) {
				check(cttk_bool_to_int(cttk_i31_isnan(m)),
					"mod 1 (%d,%d)", i, j);
			} else {
				check(!cttk_bool_to_int(cttk_i31_isnan(m)),
					"mod 2 (%d,%d)", i, j);
				check(cttk_bool_to_int(cttk_i31_geq0(m)),
					"mod 3 (%d,%d)", i, j);
				if (cttk_bool_to_int(cttk_i31_geq0(r))) {
					check(cttk_bool_to_int
						(cttk_i31_eq(r, m)),
						"mod 4 (%d,%d)", i, j);
				} else if (cttk_bool_to_int(cttk_i31_gt0(b))) {
					cttk_i31_sub(m, m, r);
					check(cttk_bool_to_int
						(cttk_i31_eq(b, m)),
						"mod 5 (%d,%d)", i, j);
				} else {
					cttk_i31_add(m, m, b);
					check(cttk_bool_to_int
						(cttk_i31_eq(r, m)),
						"mod 6 (%d,%d)", i, j);
				}
			}
		}

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

static void
test_i31_bool(void)
{
	cttk_i31_def(x1, 300);
	cttk_i31_def(x2, 300);
	cttk_i31_def(x3, 300);
	zint z1, z2, z3;
	int i, j;
	unsigned char tmp1[100], tmp2[100], tmp3[100], tmp4[100];

	printf("Test i31 bool: ");
	fflush(stdout);

	rnd_init(8);

	for (i = 1; i <= 128; i ++) {
		cttk_i31_init(x1, i);
		cttk_i31_init(x2, i);
		cttk_i31_init(x3, i);

		for (j = 0; j < 100; j ++) {
			rnd(tmp1, 17);
			rnd(tmp2, 17);
			zint_decode(&z1, tmp1, 17, 0, 0);
			zint_decode(&z2, tmp2, 17, 0, 0);
			zint_trunc(&z1, i);
			zint_trunc(&z2, i);
			zint_encode(tmp1, 17, &z1, 0);
			zint_encode(tmp2, 17, &z2, 0);
			cttk_i31_decle_signed(x1, tmp1, 17);
			cttk_i31_decle_signed(x2, tmp2, 17);

			zint_and(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_and(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0, "and (%d,%d)", i, j);

			zint_or(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_or(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0, "or (%d,%d)", i, j);

			zint_xor(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_xor(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0, "xor (%d,%d)", i, j);

			zint_eqv(&z3, &z1, &z2);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_eqv(x3, x1, x2);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0, "eqv (%d,%d)", i, j);

			zint_not(&z3, &z1);
			zint_encode(tmp3, 17, &z3, 0);
			cttk_i31_not(x3, x1);
			cttk_i31_encle(tmp4, 17, x3);
			check(memcmp(tmp3, tmp4, 17) == 0, "not (%d,%d)", i, j);
		}

		if ((i & 3) == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	printf(" done.\n");
	fflush(stdout);
}

int
main(void)
{
	test_comparisons_32();
	test_comparisons_64();
	test_hex();
	test_base64();
	test_mul();
	test_bitlength();
	test_i31_set();
	test_i31_codec();
	test_i31_cmp();
	test_i31_addsub();
	test_i31_mul();
	test_i31_shift();
	test_i31_div();
	test_i31_bool();
	return 0;
}
