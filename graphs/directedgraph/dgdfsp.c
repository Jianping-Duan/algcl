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
#include "digraphdfsp.h"
#include "stack.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct digraph g;
	struct stack paths;
	struct digraph_dfsp gdfs;
	unsigned int s, v;
	long *w;
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
				if(sscanf(optarg, "%u", &s) != 1) {
					errmsg_exit("Illegal number. -s %s\n",
						optarg);
				}
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	if(optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;
	
	fin = open_file(fname, "r");
	digraph_init_fistream(&g, fin);
	close_file(fin);
	digraph_print(&g);
	printf("\n");
	
	digraph_dfsp_init(&gdfs, s, &g);
	w = (long *)algmalloc(sizeof(long));
	for(v = 0; v < DIGRAPH_VERTICES(&g); v++) {
		if(DIGRAPH_DFSP_HASPATH(&gdfs, v)) {
			printf("%u to %u: ", s, v);
			digraph_dfsp_paths(&gdfs, v, &paths);
			while(!STACK_ISEMPTY(&paths)) {
				stack_pop(&paths, (void **)&w);
				if(s == *w)
					printf("%ld", *w);
				else
					printf("-%ld", *w);
			}
			stack_clear(&paths);
			printf("\n");
		}
		else
			printf("%u to %u: not connected.\n", s, v);
	}
	
	ALGFREE(w);
	digraph_clear(&g);
	DIGRAPH_DFSP_CLEAR(&gdfs);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -s\n", pname);
	fprintf(stderr, "-f: The data file for the digraph.\n");
	fprintf(stderr, "-s: The soruce vertex of the digraph.\n");
	exit(EXIT_FAILURE);
}
