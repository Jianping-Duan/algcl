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
#include "bellmanfordsp.h"
#include "linearlist.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct ewdigraph g;
	struct bellmanford_sp sp;
	struct single_list paths;
	struct slist_node *nptr;
	struct diedge *e;
	struct stack *st;
	unsigned int s, v;
	char se[128];
	clock_t start_time, end_time;
	char *fname = NULL;

	int op;
	const char *optstr = "f:s:";

	extern char *optarg;
	extern int optind;
	
	if(argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while((op = getopt(argc, argv, optstr)) != -1) {
		switch(op) {
			case 'f':
				fname = optarg;
				break;
			case 's':
				if(sscanf(optarg, "%u", &s) != 1)
					errmsg_exit("Illegal number. -s %s\n", optarg);
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	if(optind < argc)
		usage_info(argv[0]);
	
	SET_RANDOM_SEED;

	printf("Prints a edge-weighted digraph from input stream.\n");
	fin = open_file(fname, "r");
	ewdigraph_init_fistream(&g, fin);
	close_file(fin);
	if(EWDIGRAPH_VERTICES(&g) <= 100)
		ewdigraph_print(&g);
	else
		printf("Vertices are too many!!!\n");
	printf("\n");

	if(s >= EWDIGRAPH_VERTICES(&g)) {
		ewdigraph_clear(&g);
		errmsg_exit("soruce vertex must between 0 and %u\n", 
			EWDIGRAPH_VERTICES(&g) - 1);
	}

	start_time = clock();
	bmfsp_init(&sp, &g, s);

	if(BMFSP_HAS_NEGATIVE_CYCLE(&sp)) {
		printf("Prints it negative cycle.\n");
		e = (struct diedge *)algmalloc(sizeof(struct diedge));
		st = BMFSP_NEGATIVE_CYCLE(&sp);
		while(!STACK_ISEMPTY(st)) {
			stack_pop(st, (void **)&e);
			DIEDGE_STRING(e, se);
			printf("%s\n", se);
		}
		ALGFREE(e);
	}
	else {
		printf("Prints it shortest path.\n");
		for(v = 0; v < EWDIGRAPH_VERTICES(&g); v++) {
			if(BMFSP_HAS_PATHTO(&sp, v)) {
				printf("%u %u (%.3f)  ", s, v, (double)bmfsp_distto(&sp, v));
				bmfsp_paths_get(&sp, v, &paths);
				SLIST_FOREACH(&paths, nptr, struct diedge, e) {
					DIEDGE_STRING(e, se);
					printf("%s  ", se);
				}
				printf("\n");
			}
			else
				printf("%u to %u no path.\n", s, v); 
		}
		slist_clear(&paths);
	}
	end_time = clock();
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) /
		(double)CLOCKS_PER_SEC);
	
	BMFSP_CLEAR(&sp);
	ewdigraph_clear(&g);

	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -s\n", pname);
	fprintf(stderr, "-f: The data file for the edge-weighted digraph..\n");
	fprintf(stderr, "-s: The soruce vertex of the edge-weighted digraph.\n");
	exit(EXIT_FAILURE);
}
