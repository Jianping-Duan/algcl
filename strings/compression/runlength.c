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
#include "runlength.h"
#include "binaryin.h"
#include "binaryout.h"

#define RADIX	256
#define LGR		8

/* 
 * Reads a sequence of bits from file input; compresses
 * them using run-length coding with 8-bit run lengths; 
 * and writes the results to file output.
 */
void 
runlen_compress(const char *infile, const char *outfile)
{
	unsigned char run = 0;
	bool old = false;
	struct binary_input bi;
	struct binary_output bo;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);

	while (!BINPUT_ISEMPTY(&bi)) {
		if (binput_read_bool(&bi) != old) {
			boutput_write_char_r(&bo, run, LGR);
			run = 1;
			old = !old;
		} else {
			if (run == RADIX - 1) {
				boutput_write_char_r(&bo, run, LGR);
				run = 0;
				boutput_write_char_r(&bo, run, LGR);
			}
			run++;
		}
	}
	
	boutput_write_char_r(&bo, run, LGR);
	boutput_close(&bo);

	end_time = clock();
	printf("Compression finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}

/* 
 * Reads a sequence of bits from file input (that are encoded
 * using run-length encoding with 8-bit run lengths); decodes them;
 * and writes the results to standard output. 
 */
void 
runlen_expand(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	bool b = false;
	int run, i;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);

	while (!BINPUT_ISEMPTY(&bi)) {
		run = binput_read_int_r(&bi, LGR);
		for (i = 0; i < run; i++)
			boutput_write_bool(&bo, b);
		b = !b;
	}
	boutput_close(&bo);

	end_time = clock();
	printf("Expansion finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}
