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

/* 
 * Reads in a sequence of bytes from file input and writes
 * them to standard output using hexadecimal notation, 
 * k hex digits per line, where k is given as a 
 * command-line integer (defaults to 16 if no integer is 
 * specified); also writes the number of bits.
 */

int
main(int argc, char *argv[])
{
	struct binary_input bi;
	int width = 16;
	long count = 0;
	char c;

	switch(argc) {
		case 3:
			sscanf(argv[1], "%d", &width);
			binput_init(&bi, argv[2]);
			break;
		case 2:
			binput_init(&bi, argv[1]);
			break;
		default:
			errmsg_exit("Usage: %s [bits per lines] <infile>\n", argv[0]);
	}

	while(!BINPUT_ISEMPTY(&bi)) {
		if(width <= 0) {
			binput_read_char(&bi);
			continue;
		}

		if(count == 0)
			/* eliminate blank space in the front of line */
			while(0);
		else if(count % width == 0)
			printf("%ld\n", count);
		else
			printf(" ");
		
		c = binput_read_char(&bi);
		printf("%02x", c & 0xff);

		count++;
	}
	
	if(width > 0)
		printf("\n");
	printf("%ld bits\n", count * 8);	
	
	return 0;
}
