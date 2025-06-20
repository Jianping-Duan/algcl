/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, 2025 Jianping Duan <static.integer@hotmail.com>
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
 * 3. Neither the name of the University nor the names of its contributors
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
#include "bipartitegraph/bipartitegraphbfs.h"
#include "graphgenerator.h"
#include "queue.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	unsigned int v1, v2, e, f, i, v, w;
	struct graph g;
	struct bipartite_graph_bfs bg;
	struct queue qu;
	int *x;

	int op;
	const char *optstr = "v:V:e:E:";

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
		case 'E':
			if (sscanf(optarg, "%u", &f) != 1)
				errmsg_exit("Illegal number. -E %s\n", optarg);
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
	for (i = 0; i < f; i++) {
		v = rand_range_integer(0, v1 + v2);
		w = rand_range_integer(0, v1 + v2);
		graph_add_edge(&g, v, w);
	}
	graph_print(&g);
	printf("\n");
	
	bigraphbfs_get(&bg, &g);
	if (BIGRAPHBFS_ISBIPARTITE(&bg)) {
		printf("Graph is bipartite.\n");
		for (v = 0; v < GRAPH_VERTICES(&g); v++) {
			printf("%u: %s\n", v, 
				bigraphbfs_color(&bg, v) == BLACK ?
				"Black" : "White");
		}
	} else {
		printf("Graph has an odd-length cycle:\n");
		x = (int *)algmalloc(sizeof(int));
		qu = BIGRAPHBFS_ODDCYCLE(&bg);
		while (!QUEUE_ISEMPTY(&qu)) {
			dequeue(&qu, (void **)&x);
			printf("%d ", *x);
		}
		ALGFREE(x);
		queue_clear(&qu);
		printf("\n");
	}
	
	graph_clear(&g);
	BIGRAPHBFS_CLEAR(&bg);
	
	return 0;
}

static void usage_info(const char *pname)
{
	fprintf(stderr, "Usage %s -v -V -e -E.\n", pname);
	fprintf(stderr, "Create random bipartite graph with 'v' vertices on "
		"left side,\n");
	fprintf(stderr, "'V' vertices on right side, and 'e' edges; then "
		"add 'E' random edges.\n");
	exit(EXIT_FAILURE);
}
