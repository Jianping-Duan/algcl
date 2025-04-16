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
#include "algcomm.h"
#include <getopt.h>

static void usage_info(const char *);

int
main(int argc, char *argv[])
{
	FILE *fp;
	int items[128], start_item, end_item;
	int num, i, cnt = 0;

	int op;
	const char *optstr= "s:e:n:";

	extern char *optarg;
	extern int optind;
	
	SET_RANDOM_SEED;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 's':
				if (sscanf(optarg, "%d", &start_item) != 1)
					errmsg_exit("Illegal number. -s %s\n", optarg);
				break;
			case 'e':
				if (sscanf(optarg, "%d", &end_item) != 1)
					errmsg_exit("Illegal number. -e %s\n", optarg);
				break;
			case 'n':
				if (sscanf(optarg, "%d", &num) != 1)
					errmsg_exit("Illegal number. -n %s\n", optarg);
				break;
			default: 
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}

	if (optind < argc)
		usage_info(argv[0]);
	
	fp = open_file(argv[1], "w+b");
	
	while (num > 0) {
		for (i = 0; i < MIN(num, 128); i++) {
			items[i] = rand_range_integer(start_item, end_item);
			cnt++;
		}
		fwrite(&items[0], sizeof(int), cnt, fp);
		num -= 128;
		cnt = 0;
	}
	
	close_file(fp);
	
	return 0;
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -s -e -n\n", pname);
	fprintf(stderr, "-s: The start integer number.\n");
	fprintf(stderr, "-e: The end integer number.\n");
	fprintf(stderr, "-n: The number of integers.\n");
	exit(EXIT_FAILURE);
}
