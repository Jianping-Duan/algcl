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
#include "directedcycle.h"
#include "digraphgenerator.h"
#include "stack.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	struct digraph g;
	struct stack cycle;
	unsigned int vs, es, f, i, v, w, *x;

	int op;
	const char *optstr = "v:e:E:";

	extern char *optarg;
	extern int optind;
	
	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
		case 'v':
			if (sscanf(optarg, "%u", &vs) != 1)
				errmsg_exit("Illegal number. -v %s\n", optarg);
			break;
		case 'e':
			if (sscanf(optarg, "%u", &es) != 1)
				errmsg_exit("Illegal number. -e %s\n", optarg);
			break;
		case 'E':
			if (sscanf(optarg, "%u", &f) != 1)
				errmsg_exit("Illegal number. -f %s\n", optarg);
			break;
		default:
			fprintf(stderr, "Parameters error.\n");
			usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;

	simple_acyclic_digraph(vs, es, &g);
	for (i = 0; i < f; i++) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		digraph_add_edge(&g, v, w);
	}
	digraph_print(&g);
	printf("\n");

	x = (unsigned int *)algmalloc(sizeof(int));
	directed_cycle_get(&g, &cycle);
	if (STACK_ISEMPTY(&cycle))
		printf("No directed cycle.\n");
	else {
		while (!STACK_ISEMPTY(&cycle)) {
			stack_pop(&cycle, (void **)&x);
			printf("%u ", *x);
		}
		printf("\n");
		stack_clear(&cycle);
	}
	
	ALGFREE(x);
	digraph_clear(&g);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -v -e -E\n", pname);
	fprintf(stderr, "-v: The vertices for the digraph.\n");
	fprintf(stderr, "-e: The edges of the graph.\n");
	fprintf(stderr, "-E: The extra edges of this graph.\n");
	exit(EXIT_FAILURE);
}
