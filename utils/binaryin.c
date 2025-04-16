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
#include "binaryin.h"

static inline void fill_buffer(struct binary_input *);

/* Initialize binary file input stream. */
void
binput_init(struct binary_input *bin, const char *filename)
{
	bin->istream = open_file(filename, "rb");
	bin->buffer = 0;
	bin->size = 0;
	fill_buffer(bin);
}

/* 
 * Reads the next bit of data from file input 
 * and return as a boolean.
 */
bool
binput_read_bool(struct binary_input *bin)
{
	bool bit;

	if (BINPUT_ISEMPTY(bin))
		errmsg_exit("Reading from empty input stream.\n");

	bin->size--;
	bit = ((bin->buffer >> bin->size) & 1) == 1;

	if (bin->size == 0)
		fill_buffer(bin);

	return bit;
}

/* 
 * Reads the next 8 bits from input stream and 
 * return as an 8-bit char.
 */
char
binput_read_char(struct binary_input *bin)
{
	int ch;
	int n;

	if (BINPUT_ISEMPTY(bin))
		errmsg_exit("Reading from empty input stream.\n");

	/* special case when aligned byte */
	if (bin->size == 8) {
		ch = bin->buffer;
		fill_buffer(bin);
		return (char)(ch & 0xff);
	}

	/* 
	 * Combine last n bits of current buffer with
	 * first 8-n bits of new buffer.
	 */
	ch = bin->buffer;
	ch <<= (8 - bin->size);
	n = bin->size;

	fill_buffer(bin);
	if (BINPUT_ISEMPTY(bin))
		errmsg_exit("Reading from empty input stream.\n");

	bin->size = n;
	ch |= ((unsigned int)bin->buffer >> n);
	
	/* 
	 * The above code doesn't quite work for the last
	 * character if n = 8 because buffer will be -1,
	 * so there is a special case for aligned byte.
	 */

	return (char)(ch & 0xff);
}

/* 
 * Reads the next r bits from file input and 
 * return as an r-bit character. 
 */
char
binput_read_char_r(struct binary_input *bin, int r)
{
	char ch;
	bool bit;
	int i;

	if (r < 1 || r > 8)
		errmsg_exit("Illegal value of r = %d\n", r);

	/* optimize r = 8 case */
	if (r == 8)
		return binput_read_char(bin);

	ch = 0;
	for (i = 0; i < r; i++) {
		ch <<= 1;
		bit = binput_read_bool(bin);
		if (bit)
			ch |= 1;
	}

	return ch;
}

/* 
 * Reads the remaining bytes of data from file 
 * input and return as a string. 
 */
char *
binput_read_string(struct binary_input *bin)
{
	size_t sz, i = 0;
	long loc;
	char *str, ch;
	
	/* write down file current location */
	loc = ftell(bin->istream);

	/* calculate file size */
	rewind(bin->istream);
	fseek(bin->istream, 0, SEEK_END);
	sz = ftell(bin->istream);

	str = (char *)algmalloc(sz * sizeof(char));
	fseek(bin->istream, loc, SEEK_SET);
	while (!BINPUT_ISEMPTY(bin)) {
		ch = binput_read_char(bin);
		*(str + i++) = ch;
	}
	*(str + i - 2) = '\0';

	return str;
}

/* 
 * Reads the next 16 bits from file input and 
 * return as a 16-bit short. 
 */
short
binput_read_short(struct binary_input *bin)
{
	short x;
	unsigned char c;
	int i;

	x = 0;
	for (i = 0; i < 2; i++) {
		c = (unsigned char)binput_read_char(bin);
		x <<= 8;
		x |= c;
	}

	return x;
}

/* 
 * Reads the next 32 bits from file input and 
 * return as a 32-bit int.
 */
int
binput_read_int(struct binary_input *bin)
{
	int x, i;
	unsigned char c;

	x = 0;
	for (i = 0; i < 4; i++) {
		c = (unsigned char)binput_read_char(bin);
		x <<= 8;
		x |= c;
	}

	return x;
}

/* 
 * Reads the next r bits from file input and 
 * return as an r-bit int.
 */
int
binput_read_int_r(struct binary_input *bin, int r)
{
	int x, i;
	bool bit;

	if (r < 1 || r > 32)
		errmsg_exit("Illegal value of r = %d\n", r);

	/* optimize r = 32 case */
	if (r == 32)
		return binput_read_int(bin);

	x = 0;
	for (i = 0; i < r; i++) {
		x <<= 1;
		bit = binput_read_bool(bin);
		if (bit)
			x |= 1;
	}

	return x;
}

/* 
 * Reads the next 64 bits from file input and 
 * return as a 64-bit long. 
 */
long
binput_read_long(struct binary_input *bin)
{
	long x;
	int i;
	unsigned char c;

	x = 0;
	for (i = 0; i < 8; i++) {
		c = (unsigned char)binput_read_char(bin);
		x <<= 8;
		x |= c;
	}

	return x;
}

/* Reads a char from the input stream. */
static inline void
fill_buffer(struct binary_input *bin)
{
	if (!feof(bin->istream)) {
		bin->buffer = fgetc(bin->istream);
		bin->size = 8;
	} else {
		bin->buffer = EOF;
		bin->size = -1;
	}
}
