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

#ifndef CONFIG_H__
#define CONFIG_H__

/*
 * This file contains compile-time flags and option that can override
 * the autodetection performed in relevant files.
 *
 * Most of these macros are "flags" which have three settings:
 *
 *  - Enabled: value should be defined to 1.
 *  - Disabled: value should be defined to 0.
 *  - Autodetection: value should be undefined, allowing the default
 *    behaviour to take place.
 */

/*
 * If CTTK_CTMUL is set, then this is equivalent to setting all
 * of the following flags to the same value:
 *    CTTK_CTMUL32
 *    CTTK_CTMULU32W
 *    CTTK_CTMULS32W
 *    CTTK_CTMUL64
 * These flags can still be set individually, and will then override
 * the CTTK_CTMUL value.
 */

/*
 * If CTTK_CTMUL32 is set, then the 32-bit multiplication opcode (with
 * 32-bit result) is assumed to be constant-time.
 *
#define CTTK_CTMUL32   1
 */

/*
 * If CTTK_CTMULU32W is set, then the 32x32->64 unsigned multiplication
 * is assumed to be constant-time.
 *
#define CTTK_CTMULU32W   1
 */

/*
 * If CTTK_CTMULS32W is set, then the 32x32->64 signed multiplication
 * is assumed to be constant-time.
 *
#define CTTK_CTMULS32W   1
 */

/*
 * If CTTK_CTMUL64 is set, then the 64-bit multiplication opcode (with
 * 64-bit result) is assumed to be constant-time.
 *
#define CTTK_CTMUL64   1
 */

/*
 * If CTTK_NO_MALLOC is set, then the <stdlib.h> header will not be
 * included, and no dynamic memory allocation will be performed. Dynamic
 * memory allocation is performed for some arithmetic operations that
 * require extra space, when operands exceed the size that can be handled
 * with only stack buffers (see CTTK_MAX_INT_BUF).
 *
#define CTTK_NO_MALLOC   1
 */

/*
 * The CTTK_MAX_INT_BUF macro defines the maximum size, in bytes, of
 * temporary integers allocated on the stack as part of some arithmetic
 * operations. Functions that need several such temporaries will use
 * CTTK_MAX_INT_BUF as the total aggregate size of such temporaries.
 * If the required size is greater, then dynamic memory allocation is
 * used, unless it was disabled with CTTK_NO_MALLOC, in which case the
 * result will be forcibly set to NaN.
 *
 * Don't define this value to less than 64. Default is 4096.
 *
#define CTTK_MAX_INT_BUF   4096
 */

#endif
