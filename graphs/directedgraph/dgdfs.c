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
#include "digraphdfs.h"
#include "singlelist.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct digraph g;
	struct digraph_dfs dfs;
	struct single_list svset;
	unsigned int v, s, *vset;
	int i, j, cnt = 0;
	char *fname = NULL;

	int op;
	const char *optstr = "f:S:";

	extern char *optarg;
	extern int optind;

	if (argc < (int)strlen(optstr) + 1)
		usage_info(argv[0]);

#if defined(__FreeBSD__)
	if (argv[1][1] != 'f')
		errmsg_exit("The argument '-f' is must be first.\n");
#endif

	SET_RANDOM_SEED;

	vset = (unsigned int *)algmalloc((argc - 3) * sizeof(int));

	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 'f':
				fname = optarg;
				break;
			case 'S':
				for (i = optind - 1, j = 0; (i < argc && argv[i][0] != '-') &&
					j < argc - 4; i++, j++) {
					if (sscanf(argv[i], "%u", &s) == 1) {
						vset[j] = s;
						cnt++;
					} else
						errmsg_exit("Illegal number. -S %s\n", argv[i]);
				}
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	fin = open_file(fname, "r");
	digraph_init_fistream(&g, fin);
	close_file(fin);
	digraph_print(&g);
	printf("\n");

	slist_init(&svset, sizeof(int), NULL);
	for (i = 0; i < cnt; i++)
		slist_append(&svset, &vset[i]);
	ALGFREE(vset);

	digraph_dfs_svset(&dfs, &g, &svset);

	for (v = 0; v < DIGRAPH_VERTICES(&g); v++)
		if (DIGRAPH_DFS_MARKED(&dfs, v))
			printf("%u ", v);
	printf("\n");
	
	digraph_clear(&g);
	slist_clear(&svset);
	DIGRAPH_DFS_CLEAR(&dfs);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -S\n", pname);
	fprintf(stderr, "-f: The data file for the digraph.\n");
	fprintf(stderr, "-S: The some of soruce verties for this digraph.\n");
	exit(EXIT_FAILURE);
}
