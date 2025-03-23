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
#ifndef _BINARYOUT_H_
#define _BINARYOUT_H_

#include "algcomm.h"

struct binary_output {
	FILE *ostream;	/* output stream (file output) */
	int buffer;		/* 8-bit buffer of bits to write */
	int size;		/* number of bits remaining in buffer */
};

/* Initialize the binary file output stream. */
static inline void 
boutput_init(struct binary_output *bo, const char *filename)
{
	bo->ostream = open_file(filename, "wb");
	bo->buffer = 0;
	bo->size = 0;
}

/* 
 * Flushes standard output, padding 0s if number of bits 
 * written so far is not a multiple of 8.
 */
void boutput_flush(struct binary_output *bo);

/* Flushes and closes file output. */
void boutput_close(struct binary_output *bo);

/* Writes the specified bit to file output. */
void boutput_write_bool(struct binary_output *bo, bool bit);

/* Writes the 8-bit char to file output. */
void boutput_write_char(struct binary_output *bo, char x);

/* Writes the r-bit char to file output. */
void boutput_write_char_r(struct binary_output *bo, char x, int r);

/* Writes the string of 8-bit characters to file output. */
void boutput_write_string(struct binary_output *bo, const char *s);

/* Writes the string of r-bit characters to file output. */
void boutput_write_string_r(struct binary_output *bo, const char *s, int r);

/* Writes the 16-bit int to file output. */
void boutput_write_short(struct binary_output *bo, short x);

/* Writes the 32-bit int to file output. */
void boutput_write_int(struct binary_output *bo, int x);

/* Writes the string of r-bit int to file output. */
void boutput_write_int_r(struct binary_output *bo, int x, int r);

/* Writes the 64-bit long to file output. */
void boutput_write_long(struct binary_output *bo, long x);

#endif	/* _BINARYOUT_H_ */
