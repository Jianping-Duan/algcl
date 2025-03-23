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
#include "binaryout.h"

static const char dna[] = "ACGT";
static int *dnaind;

static void compress(const char *, const char *);
static void expand(const char *, const char *);

int main(int argc, char *argv[])
{
	int i, n;

	if(argc != 4) {
		fprintf(stderr, "Usage: %s <ops> <infile> <outfile>\n", argv[0]);
		fprintf(stderr, "********** OPS **********\n");
		fprintf(stderr, "* c or C: compress the file. \n");
		fprintf(stderr, "* e or E: expand the file. \n");
		fprintf(stderr, "*************************\n");
		exit(EXIT_FAILURE);
	}

	dnaind = (int *)algmalloc(CHAR_MAX * sizeof(int));
	for(i = 0; i < CHAR_MAX; i++)
		dnaind[i] = -1;
	
	n = strlen(dna);
	for(i = 0; i < n; i++)
		dnaind[(int)dna[i]] = i;

	if(strcmp(argv[1], "c") == 0 || strcmp(argv[1], "C") == 0)
		compress(argv[2], argv[3]);
	else if(strcmp(argv[1], "e") == 0 || strcmp(argv[1], "E") == 0)
		expand(argv[2], argv[3]);
	else
		printf("Unknow operation, %s\n", argv[1]);

	ALGFREE(dnaind);

	return 0;
}

static void 
compress(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	char *s;
	int n, i, d;

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);

	s = binput_read_string(&bi);
	n = strlen(s);
	boutput_write_int(&bo, n);

	for(i = 0; i < n; i++) {
		d = dnaind[string_char_at(s, i)];
		boutput_write_int_r(&bo, d, 2);
	}

	BINPUT_CLOSE(&bi);
	boutput_close(&bo);
	ALGFREE(s);
}

static void 
expand(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	int n, i, c;

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);

	n = binput_read_int(&bi);
	for(i = 0; i < n; i++) {
		c = binput_read_int_r(&bi, 2);
		boutput_write_char(&bo, dna[c]);
	}
	boutput_write_char(&bo, '\n');

	BINPUT_CLOSE(&bi);
	boutput_close(&bo);
}
