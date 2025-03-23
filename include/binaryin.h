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
#ifndef _BINARYIN_H_
#define _BINARYIN_H_

/* 
 * This head file provides functions for reading in bits from file input. 
 */

#include "algcomm.h"

struct binary_input {
	FILE *istream;	/* input stream */
	int buffer;		/* one character buffer */
	int size;		/* number of bits left in buffer */
};

/* Close this file input stream. */
#define BINPUT_CLOSE(bin)	do {	\
	close_file((bin)->istream);		\
	(bin)->buffer = EOF;			\
	(bin)->size = -1;				\
} while(0)

/* Returns true if file input stream is empty */
#define BINPUT_ISEMPTY(bin)		((bin)->buffer == EOF)

/* Initialize the binary file input stream. */
void binput_init(struct binary_input *bin, const char *filename);

/* 
 * Reads the next bit of data from file 
 * input and return as a boolean.
 */
bool binput_read_bool(struct binary_input *bin);

/* 
 * Reads the next 8 bits from file input 
 * and return as an 8-bit char.
 */
char binput_read_char(struct binary_input *bin);

/* 
 * Reads the next r bits from file input 
 * and return as an r-bit character.
 */
char binput_read_char_r(struct binary_input *bin, int r);

/* 
 * Reads the remaining bytes of data from file 
 * input and return as a string. 
 */
char * binput_read_string(struct binary_input *bin);

/* 
 * Reads the next 16 bits from file 
 * input and return as a 16-bit short.
 */
short binput_read_short(struct binary_input *bin);

/* 
 * Reads the next 32 bits from file 
 * input and return as a 32-bit int.
 */
int binput_read_int(struct binary_input *bin);

/* 
 * Reads the next r bits from file 
 * input and return as an r-bit int.
 */
int binput_read_int_r(struct binary_input *bin, int r);

/* 
 * Reads the next 64 bits from file 
 * input and return as a 64-bit long.
 */
long binput_read_long(struct binary_input *bin);

#endif	/* _BINARYIN_H_ */
