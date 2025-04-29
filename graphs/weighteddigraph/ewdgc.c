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
#include "ewdigraphcycle.h"
#include "stack.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	struct ewdigraph g;
	struct ewdigraph_cycle dc;
	struct diedge *e;
	struct stack *cycle;
	unsigned int i, v, w, nv, ne, nf;
	float weight;
	char se[128];

	int op;
	const char *optstr = "v:e:E:";

	extern char *optarg;
	extern int optind;

	SET_RANDOM_SEED;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 'v':
				if (sscanf(optarg, "%u", &nv) != 1)
					errmsg_exit("Illegal number. -v %s\n", optarg);
				break;
			case 'e':
				if (sscanf(optarg, "%u", &ne) != 1)
					errmsg_exit("Illegal number. -e %s\n", optarg);
				break;
			case 'E':
				if (sscanf(optarg, "%u", &nf) != 1)
					errmsg_exit("Illegal number. -f %s\n", optarg);
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);

	ewdigraph_init(&g, nv);
	for (i = 0; i < ne; i++) {
		do {
			v = rand_range_integer(0, nv);
			w = rand_range_integer(0, nv);
		} while (v >= w);
		weight = (float)rand_range_float(0.0, 1.0);
		e = make_diedge(v, w, weight);
		ewdigraph_add_edge(&g, e);
	}

	for (i = 0; i < nf; i++) {
		v = rand_range_integer(0, nv);
		w = rand_range_integer(0, nv);
		weight = (float)rand_range_float(0.0, 1.0);
		e = make_diedge(v, w, weight);
		ewdigraph_add_edge(&g, e);
	}

	ewdigraph_print(&g);
	printf("\n");

	ewdigraph_cycle_init(&dc, &g);
	if (EWDIGRAPH_HAS_CYCLE(&dc)) {
		printf("Directed cycle:\n");
		cycle = EWDIGRAPH_CYCLE_GET(&dc);
		while (!STACK_ISEMPTY(cycle)) {
			stack_pop(cycle, (void **)&e);
			DIEDGE_STRING(e, se);
			printf("%s ", se);
		}
		printf("\n");
	} else
		printf("No directed cycle.\n");
	
	ewdigraph_clear(&g);
	EWDIGRAPH_CYCLE_CLEAR(&dc);

	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -v -e -E\n", pname);
	fprintf(stderr, "-v: The vertices for the edge-weighted digraph.\n");
	fprintf(stderr, "-e: The edges of the edge-weighted graph.\n");
	fprintf(stderr, "-E: The extra edges of this edge-weighted graph.\n");
	exit(EXIT_FAILURE);
}
