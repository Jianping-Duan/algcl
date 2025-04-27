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
#include "unionfind/quickfinduf.h"

#define BUFSIZE		128

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct quick_find_uf uf;
	char buf[BUFSIZE];
	int i, n, p, q;
	clock_t start_time, end_time;
	
	if (argc != 2) 
		errmsg_exit("Usage: %s <datafile>\n", argv[0]);
	
	fin = open_file(argv[1], "r");
	fscanf(fin, "%d", &n);
	fseek(fin, 1L, SEEK_CUR);

	start_time = clock();
	qfuf_init(&uf, (unsigned)n);
	for (i = 0; i < n; i++) {
		fgets(buf, BUFSIZE, fin);
		sscanf(buf, "%d %d", &p, &q);
		if (qfuf_connected(&uf, p, q))
			continue;
		qfuf_union(&uf, p, q);
		printf("%d %d\n", p, q);
	}
	end_time = clock();

	printf("%ld components.\n", QFUF_COUNT(&uf));
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);

	QFUF_CLEAR(&uf);

	return 0;
}
