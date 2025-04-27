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
#include "eulergraph/directedeulerian.h"
#include "digraphgenerator.h"
#include "stack.h"
#include <getopt.h>

static void usage_info(const char *);
static void print_dieuler(const struct digraph *, const char *);

int 
main(int argc, char *argv[])
{
	unsigned int v, e;
	struct digraph g[4];

	int op;
	const char *optstr = "v:e:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 'v':
				if (sscanf(optarg, "%u", &v) != 1)
					errmsg_exit("Illegal number. -v %s\n", optarg);
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
	
	/* Eulerian cycle */
	eulerian_cycle_digraph(v, e, &g[0]);
	print_dieuler(&g[0], "Eulerian cycle");
	printf("\n");
	
	/* Eulerian path */
	eulerian_path_digraph(v, e, &g[1]);
	print_dieuler(&g[1], "Eulerian path");
	printf("\n");
	
	/* empty digraph */
	digraph_init(&g[2], v);
	print_dieuler(&g[2], "Empty digraph");
	printf("\n");
	
	/* simple digraph */
	simple_digraph1(v, e, &g[3]);
	print_dieuler(&g[3], "Simple digraph");
	printf("\n");
	
	for (v = 0; v < 4; v++)
		digraph_clear(&g[v]);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -v -e\n", pname);
	fprintf(stderr, "-v: The vertices of this digraph..\n");
	fprintf(stderr, "-e: The edges of this digraph.\n");
	exit(EXIT_FAILURE);
}

static void 
print_dieuler(const struct digraph *g, const char *desc)
{
	struct stack cycle, path;
	unsigned int *v;
	
	printf("%s", desc);
	printf("\n");
	printf("------------------------------------------\n");
	digraph_print(g);
	printf("\n");
	
	dieuler_cycle_get(&cycle, g);
	dieuler_path_get(&path, g);

	v = (unsigned int *)algmalloc(sizeof(int));

	printf("Eulerian cycle:\n");
	if (!STACK_ISEMPTY(&cycle)) {
		while (!STACK_ISEMPTY(&cycle)) {
			stack_pop(&cycle, (void **)&v);
			printf("%u ", *v);
		}
		printf("\n");
	} else
		printf("None.\n");
	
	printf("Eulerian path:\n");
	if (!STACK_ISEMPTY(&path)) {
		while (!STACK_ISEMPTY(&path)) {
			stack_pop(&path, (void **)&v);
			printf("%u ", *v);
		}
		printf("\n");
	} else
		printf("None.\n");
	
	printf("------------------------------------------\n");
	
	ALGFREE(v);
	stack_clear(&cycle);
	stack_clear(&path);
}
