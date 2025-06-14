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
#include "lempelzivwelch.h"
#include "binaryin.h"
#include "binaryout.h"
#include "tstrie.h"

/* number of input chars */
#define RADIX	128

/* number of codewords = 2^W */
#define LENGTH	4096

/* codeword width */
#define WIDTH	12

/* max string length */
#define MAX_STRLEN	64

static void left_trim(char *, long);

/* 
 * Reads a sequence of 8-bit bytes from file input; 
 * compresses them using LZW compression with 12-bit codewords; 
 * and writes the results to file output.
 */
void 
lzw_compress(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	struct tstrie st;
	char *input, s[2], *prefix, *subs;
	int i, code;
	unsigned int plen;
	unsigned long slen;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);
	TSTRIE_INIT(&st);

	input = binput_read_string(&bi, &slen);

	/* since TST is not balanced, */
	/* it would be better to insert in a different order */
	for (i = 1; i < RADIX; i++) {
		s[0] = (char)i;
		s[1] = '\0';
		tstrie_put(&st, s, i);
	}

	code = RADIX + 1;	/* R is codeword for EOF */

	while (slen > 1) {
		/* find max prefix match 'prefix' */
		prefix = tstrie_longest_prefix(&st, input);
		plen = strlen(prefix);

		/* write prefix's ecoding */
		boutput_write_int_r(&bo, tstrie_get(&st, prefix), WIDTH);
		ALGFREE(prefix);

		/* add s to symbol table. */
		if (plen < slen && code < LENGTH) {
			subs = substring(input, 0, plen + 1); 
			tstrie_put(&st, subs, code++);
			ALGFREE(subs);
		}

		/* scan past string in input. */
		left_trim(input, plen);
		slen -= plen;
	}

	boutput_write_int_r(&bo, RADIX, WIDTH);
	boutput_close(&bo);
	BINPUT_CLOSE(&bi);

	ALGFREE(input);
	tstrie_clear(&st);

	end_time = clock();
	printf("Compression finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}

/* 
 * Reads a sequence of bit encoded using LZW compression with
 * 12-bit codewords from file input; expands them; and writes
 * the results to file output.
 */
void 
lzw_expand(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	char **st, *val, *tmp;
	char s[2];
	int i, codeword;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	if ((codeword = binput_read_int_r(&bi, WIDTH)) == RADIX) {
		BINPUT_CLOSE(&bi);
		return;	/* expanded message is empty string */
	}

	st = (char **)algmalloc(LENGTH * sizeof(char *));
	for (i = 0; i < LENGTH; i++)
		*(st + i) = (char *)algmalloc(MAX_STRLEN * sizeof(char));

	for (i = 1; i < RADIX; i++) {
		s[0] = (char)i;
		s[1] = '\0';
		strcpy(st[i], s);
	}
	strcpy(st[i++], " ");

	val = (char *)algmalloc(MAX_STRLEN * sizeof(char));
	tmp = (char *)algmalloc(MAX_STRLEN * sizeof(char));
	strncpy(val, st[codeword], MAX_STRLEN - 1);

	boutput_init(&bo, outfile);
	while (1) {
		boutput_write_string(&bo, val);
		if ((codeword = binput_read_int_r(&bi, WIDTH)) == RADIX)
			break;
		strncpy(tmp, st[codeword], MAX_STRLEN - 1);

		if (i == codeword) {	/* special case hack */
			strcat(tmp, val);
			s[0] = val[0];
			s[1] = '\0';
			strcat(tmp, s);
		}

		if (i < LENGTH) {
			strcpy(st[i], val);
			s[0] = tmp[0];
			s[1] = '\0';
			strcat(st[i], s);
			i++;
		}
		memset(val, 0, MAX_STRLEN);
		strcpy(val, tmp);
		memset(tmp, 0, MAX_STRLEN);
	}

	boutput_close(&bo);
	BINPUT_CLOSE(&bi);

	ALGFREE(val);
	ALGFREE(tmp);
	for (i = 0; i < LENGTH; i++)
		ALGFREE(*(st + i));
	ALGFREE(st);

	end_time = clock();
	printf("Expansion finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}

/******************** static function boundary ********************/

static void 
left_trim(char *tgt, long i)
{
	long len, k;

	if (i <= 0)
		return;

	if ((len = strlen(tgt)) <= i)
		return;

	for (k = 0; i < len; i++, k++) {
		*(tgt + k) = *(tgt + i);
		*(tgt + i) = '\0';
	}
	*(tgt + k) = '\0';
}
