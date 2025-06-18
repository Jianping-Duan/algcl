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
#include "bipartitegraph/hopcroftkarp.h"
#include "graphgenerator.h"
#include <getopt.h>

static void usage_info(const char *);

int main(int argc, char *argv[])
{
	unsigned int v1, v2, e;
	long v, w;
	struct graph g;
	struct hopcroft_karp bm;

	int op;
	const char *optstr = "v:V:e:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
		case 'v':
			if (sscanf(optarg, "%u", &v1) != 1)
				errmsg_exit("Illegal number. -v %s\n", optarg);
			break;
		case 'V':
			if (sscanf(optarg, "%u", &v2) != 1)
				errmsg_exit("Illegal number. -V %s\n", optarg);
			break;
		case 'e':
			if (sscanf(optarg, "%u", &e) != 1)
				errmsg_exit("Illegal number. -e %s\n", optarg);
			break;
		default:
			fprintf(stderr, "Parameters error.\n");
			usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;
	
	bipartite_graph1(v1, v2, e, &g);
	graph_print(&g);
	printf("\n");
	
	hkbipmatch_init(&bm, &g);	

	printf("Number of edges in max matching: %u\n", HKBIPMATCH_SIZE(&bm));
	printf("Number of edges in min vertex cover: %u\n",
		HKBIPMATCH_SIZE(&bm));
	printf("Graph has a perfact matching: %s\n",
		HKBIPMATCH_ISPERFECT(&bm) ? "Yes" : "No");
	printf("\n");

	printf("Max matching:\n");
	for (v = 0; v < GRAPH_VERTICES(&g); v++) {
		w = HKBIPMATCH_MATE(&bm, v);
		 /* print each edge only once */
		if (HKBIPMATCH_ISMATCHED(&bm, v) && v < w)
			printf("%ld-%ld  ", v, w);
	}
	printf("\n");

	printf("Min vertex cover:\n");
	for (v = 0; v < GRAPH_VERTICES(&g); v++)
		if (HKBIPMATCH_MIN_COVER(&bm, v))
			printf("%ld ", v);
	printf("\n");

	graph_clear(&g);
	HKBIPMATCH_CLEAR(&bm);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage %s -v -V -e.\n", pname);
	fprintf(stderr, "Create random bipartite graph with 'v' vertices on "
		"left side,\n");
	fprintf(stderr, "'V' vertices on right side, and 'e' edges.\n");
	exit(EXIT_FAILURE);
}
