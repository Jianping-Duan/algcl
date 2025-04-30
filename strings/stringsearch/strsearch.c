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
#include "stringsearch.h"
#include <getopt.h>

static void usage_info(const char *);
static int (*search_fptr)(const char *, const char *);
static int strsearch(int, const char *, const char *);

int 
main(int argc, char *argv[])
{
	int flag = 0, offset, i;
	char *pat = NULL, *txt = NULL;

	int op;
	const char *optstr = "f:p:t:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 'f':
				if (sscanf(optarg, "%d", &flag) != 1)
					errmsg_exit("Illegal number. -n %s\n",	optarg);
				switch (flag) {
					case 1:
					case 2:
					case 3:
					case 4:
					case 5: break;
					default: errmsg_exit("Illegal function.\n");
				}
				break;
			case 'p':
				pat = optarg;
				break;
			case 't':
				txt = optarg;
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}

	if (optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;

	offset = strsearch(flag, pat, txt);
	printf("text:    %s\n", txt);
	printf("pattern: ");
	for (i = 0; i < offset; i++)
		printf(" ");
	printf("%s\n", pat);

	return 0;
}

static int 
strsearch(int flag, const char *pat, const char *txt)
{
	switch (flag) {
		case 1:
			search_fptr = string_violence_search;
			break;
		case 2:
			search_fptr = violence_search_back;
			break;
		case 3:
			search_fptr = string_kmp_search;
			break;
		case 4:
			search_fptr = boyer_moore_search;
			break;
		case 5:
			search_fptr = rabin_karp_search;
			break;
		default:
			errmsg_exit("Unknown search function\n");
	}

	return search_fptr(pat, txt);
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -p -t\n", pname);
	fprintf(stderr, "-f: The search function.\n");
	fprintf(stderr, "-p: The pattern string.\n");
	fprintf(stderr, "-t: The target string.\n");
	fprintf(stderr, "********** Search function *********\n");
	fprintf(stderr, "* 1. String violence search \n");
	fprintf(stderr, "* 2. String violence search using Index-Back \n");
	fprintf(stderr, "* 3. Knuth Morris Pratt search \n");
	fprintf(stderr, "* 4. Boyer Moore search.\n");
	fprintf(stderr, "* 5. Rabin karp search .\n");
	fprintf(stderr, "*************************************\n");

	exit(EXIT_FAILURE);
}
