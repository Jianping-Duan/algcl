/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2025 Jianping Duan <static.integer@hotmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include "binaryout.h"

static void clear_buffer(struct binary_output *);

/* 
 * Flushes standard output, padding 0s if number of 
 * bits written so far is not a multiple of 8. 
 */
void
boutput_flush(struct binary_output *bo)
{
	clear_buffer(bo);
	fflush(bo->ostream);
}

/* Flushes and closes file output. */
void
boutput_close(struct binary_output *bo)
{
	clear_buffer(bo);
	close_file(bo->ostream);
	bo->buffer = 0;
	bo->size = 0;
}

/* Writes the specified bit to file output. */
void
boutput_write_bool(struct binary_output *bo, bool bit)
{
	/* add bit to buffer */
	bo->buffer <<= 1;
	if (bit)
		bo->buffer |= 1;

	/* 
	 * if buffer is full (8 bits), 
	 * write out as a single byte.
	 */
	bo->size++;
	if (bo->size == 8)
		clear_buffer(bo);
}

/* Writes the 8-bit char to file output. */
void
boutput_write_char(struct binary_output *bo, char x)
{
	int i;
	bool bit;

	/* optimized if byte-aligned */
	if (bo->size == 0) {
		fputc(x, bo->ostream);
		return;
	}
	
	/* otherwise write one bit at a time */
	for (i = 0; i < 8; i++) {
		bit = (((unsigned char)x >> (8 - i - 1)) & 1) == 1;
		boutput_write_bool(bo, bit);
	}
}

/* Writes the r-bit char to file output. */
void
boutput_write_char_r(struct binary_output *bo, char x, int r)
{
	int i;
	bool bit;

	if (r == 8) {
		boutput_write_char(bo, x);
		return;
	}

	if (r < 1 || r > 8)
		errmsg_exit("Illegal value of r = %d\n", r);

	if (x >= (1 << r))
		errmsg_exit("Illegal %d-bits char = %c\n", r,  x);
	
	for (i = 0; i < r; i++) {
		bit = (((unsigned char)x >> (r - i - 1)) & 1) == 1;
		boutput_write_bool(bo, bit);
	}
}

/* Writes the string of 8-bit characters to file output. */
void
boutput_write_string(struct binary_output *bo, const char *s)
{
	size_t i, n;

	n = strlen(s);
	for (i = 0; i < n; i++)
		boutput_write_char(bo, s[i]);
}

/* Writes the string of r-bit characters to file output. */
void
boutput_write_string_r(struct binary_output *bo, const char *s, int r)
{
	size_t i, n;

	n = strlen(s);
	for (i = 0; i < n; i++)
		boutput_write_char_r(bo, s[i], r);
}

/* Writes the 16-bit int to file output. */
void
boutput_write_short(struct binary_output *bo, short x)
{
	boutput_write_char(bo, ((unsigned short)x >> 8) & 0xff);
	boutput_write_char(bo, ((unsigned short)x >> 0) & 0xff);
}

/* Writes the 32-bit int to file output. */
void
boutput_write_int(struct binary_output *bo, int x)
{
	boutput_write_char(bo, ((unsigned int)x >> 24) & 0xff);
	boutput_write_char(bo, ((unsigned int)x >> 16) & 0xff);
	boutput_write_char(bo, ((unsigned int)x >> 8) & 0xff);
	boutput_write_char(bo, ((unsigned int)x >> 0) & 0xff);
}

/* Writes the string of r-bit int to file output. */
void
boutput_write_int_r(struct binary_output *bo, int x, int r)
{
	int i;
	bool bit;

	if (r == 32) {
		boutput_write_int(bo, x);
		return;
	}

	if (r < 1 || r > 32)
		errmsg_exit("Illegal value of r = %d\n", r);

	if (x >= (1 << r))
		errmsg_exit("Illegal %d-bits int = %c\n", r,  x);
	
	for (i = 0; i < r; i++) {
		bit = (((unsigned int)x >> (r - i - 1)) & 1) == 1;
		boutput_write_bool(bo, bit);
	}
}

/* Writes the 64-bit long to file output. */
void
boutput_write_long(struct binary_output *bo, long x)
{
	boutput_write_char(bo, ((unsigned long)x >> 56) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 48) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 40) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 32) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 24) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 16) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 8) & 0xff);
	boutput_write_char(bo, ((unsigned long)x >> 0) & 0xff);
}

/* 
 * write out any remaining bits in buffer to file output, 
 * padding with 0s.
 */
static void
clear_buffer(struct binary_output *bo)
{
	if (bo->size == 0)
		return;

	if (bo->size > 0)
		bo->buffer <<= (8 - bo->size);
	
	if (fputc(bo->buffer, bo->ostream) == -1) {
		errmsg_exit("Write buffer to file error, %s\n",
			strerror(errno));
	}

	bo->buffer = 0;
	bo->size = 0;
}
