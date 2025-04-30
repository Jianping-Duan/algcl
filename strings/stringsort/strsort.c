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
#include "stringsort.h"
#include <getopt.h>

static void usage_info(const char *);
static void show_strings(char **, char **, char **, char **, int);

int 
main(int argc, char *argv[])
{
	char *str, **sa1, **sa2, **sa3,  **sa4;
	int i, w, sz = 0, width = 0;

	int op;
	const char *optstr = "s:w:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 's':
				if (sscanf(optarg, "%d", &sz) != 1)
					errmsg_exit("Illegal number. -n %s\n",	optarg);
				if (sz <= 0)
					errmsg_exit("Given the size must be greater than 0.\n");
				break;
			case 'w':
				if (sscanf(optarg, "%d", &width) != 1)
					errmsg_exit("Illegal number. -n %s\n",	optarg);
				if (sz <= 0)
					errmsg_exit("Given the width must be greater than 0.\n");
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}

	if (optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;

	sa1 = (char **)algcalloc(sz, sizeof(char *));
	sa2 = (char **)algcalloc(sz, sizeof(char *));
	sa3 = (char **)algcalloc(sz, sizeof(char *));
	sa4 = (char **)algcalloc(sz, sizeof(char *));

	for (i = 0; i < sz; i++) {
		sa1[i] = (char *)algcalloc(width + 1, sizeof(char));
		sa2[i] = (char *)algcalloc(width + 1, sizeof(char));
		sa3[i] = (char *)algcalloc(width + 1, sizeof(char));
		sa4[i] = (char *)algcalloc(width + 1, sizeof(char));

		str = rand_string(width);
		strncpy(sa1[i], str, width + 1);
		ALGFREE(str);
		
		w = rand_range_integer(1, width + 1);
		str = rand_string(w);
		strncpy(sa2[i], str, width + 1);
		ALGFREE(str);

		w = rand_range_integer(1, width + 1);
		str = rand_string(w);
		strncpy(sa3[i], str, width + 1);
		ALGFREE(str);

		w = rand_range_integer(1, width + 1);
		str = rand_string(w);
		strncpy(sa4[i], str, width + 1);
		ALGFREE(str);
	}

	printf("---------- Four original strings:\n");
	show_strings(sa1, sa2, sa3, sa4, sz);

	printf("---------- Sort this strings:\n");
	lsdsort(sa1, width, sz);
	mstsort(sa2, width, sz);
	quicksort(sa3, sz);
	inplace_mstsort(sa4, sz);
	show_strings(sa1, sa2, sa3, sa4, sz);

	return 0;
}

static void 
show_strings(char **sa1, char **sa2, char **sa3, char **sa4, int n)
{
	int i;
	
	printf("%-10s | %-10s | %-10s | %-10s\n", "LSDSORT", "MSTSORT", "QUICKSORT",
		"INPLACE_MSTSORT");
	printf("%-10s | %-10s | %-10s | %-10s\n", "--------", "--------", 
		"--------", "--------");
	for (i = 0; i < n; i++) {
		printf("%-10s | %-10s | %-10s | %-10s\n", sa1[i], sa2[i], sa3[i],
			sa4[i]);
	}
	printf("\n");
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -s -w\n", pname);
	fprintf(stderr, "-s: The string array of the size.\n");
	fprintf(stderr, "-w: The string of the width.\n");
	exit(EXIT_FAILURE);
}
