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

/* 
 * The KWIK provides a client for computing all 
 * occurrences of a keyword 
 * in a given string, with surrounding context.
 * This is known as keyword-in-context search.
 */
#include "stringsuffixes.h"
#include <getopt.h>

static void usage_info(const char *);

/* 
 * Reads a string from a file specified as the first
 * command-line argument; read an integer k specified as the
 * second command line argument; then repeatedly processes
 * use queries, printing all occurrences of the given query
 * string in the text string with k characters of surrounding
 * context on either side.
 */
int 
main(int argc, char *argv[])
{
	char *text, *query, *subtxt;
	struct string_suffixes ss;
	long qlen, i, index, from, to;
	long tlen, k;
	FILE *txtfile;
	char *fname = NULL;
	int sz = 0;

	int op;
	const char *optstr = "f:n:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
		case 'f':
			fname = optarg;
			break;
		case 'n':
			if (sscanf(optarg, "%d", &sz) != 1)
				errmsg_exit("Illegal number. -n %s\n",	optarg);
			if (sz <= 0) {
				errmsg_exit("The number of characters must be "
					"greater than 0.\n");
			}
			break;
		default:
			fprintf(stderr, "Parameters error.\n");
			usage_info(argv[0]);
		}
	}

	if (optind < argc)
		usage_info(argv[0]);

	txtfile = open_file(fname, "r");
	
	/* read in text */
	text = string_read_all(txtfile);
	tlen = strlen(text);

	/* build suffix array */
	strsuffix_init(&ss, text);

	while ((query = string_read_line(stdin)) != NULL && query[0] != '\0') {
		qlen = strlen(query);
		i = strsuffix_rank(&ss, query);
		for (i = strsuffix_rank(&ss, query); i < tlen; i++) {
			index = from = strsuffix_index(&ss, i);
			to = MIN(tlen, from + qlen);

			subtxt = substring(text, from, to);
			if (strcmp(query, subtxt) != 0) {
				ALGFREE(query);
				ALGFREE(subtxt);
				break;
			}

			k = index - sz;
			from = MAX(0, k);
			to = MIN(tlen, index + sz + qlen);

			subtxt = substring(text, from, to);
			printf("%s\n", subtxt);
			ALGFREE(subtxt);
		}

		ALGFREE(query);
		printf("\n");
	}

	ALGFREE(text);
	STRSUFFIX_CLEAR(&ss);

	return 0;
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -n\n", pname);
	fprintf(stderr, "-f: The text file.\n");
	fprintf(stderr, "-n: The number of characters.\n");
	exit(EXIT_FAILURE);
}
