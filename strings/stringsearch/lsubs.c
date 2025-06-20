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
#include "longestsubstring.h"

int 
main(int argc, char *argv[])
{
	char *s1, *s2, *lcs, *lrs1, *lrs2;
	FILE *tf1, *tf2;
	clock_t start_time, end_time;

	if (argc != 3)
		errmsg_exit("Usage: %s <text file1> <text file2>\n", argv[0]);

	tf1 = open_file(argv[1], "r");
	tf2 = open_file(argv[2], "r");

	s1 = string_read_all(tf1);
	s2 = string_read_all(tf2);

	printf("Longest common substring for this text files:\n");
	start_time = clock();
	lcs = string_lcs(s1, s2);
	end_time = clock();
	printf("%s\n", lcs);
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");

	printf("Longest repeat substring in text file1:\n");
	lrs1 = string_lrs(s1);
	printf("%s\n", lrs1);
	printf("\n");

	printf("Longest repeat substring in text file2:\n");
	lrs2 = string_lrs(s2);
	printf("%s\n", lrs2);
	printf("\n");


	ALGFREE(s1);
	ALGFREE(s2);
	ALGFREE(lrs1);
	ALGFREE(lrs2);

	return 0;
}
