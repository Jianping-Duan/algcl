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
#include "graphcycle.h"
#include "stack.h"

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct graph g;
	struct stack cycle;
	struct graph_cycle gc;
	unsigned int *w;
	
	SET_RANDOM_SEED;
	
	if(argc != 2)
		errmsg_exit("Usage: %s <datafile> \n", argv[0]);
	
	fin = open_file(argv[1], "r");
	graph_init_fistream(&g, fin);
	close_file(fin);
	graph_print(&g);
	printf("\n");
	
	w = (unsigned int *)algmalloc(sizeof(int));
	graph_cycle_init(&gc, &g);
	
	graph_cycle_get(&gc, &g);
	if(GRAPH_HASCYCLE(&gc)) {
		cycle = GRAPH_CYCLE(&gc);
		while(!STACK_ISEMPTY(&cycle)) {
			stack_pop(&cycle, (void **)&w);
			printf("%u ", *w);
		}
		printf("\n");
	}
	else
		printf("Graph is acyclic.\n");
	
	ALGFREE(w);
	stack_clear(&cycle);
	graph_clear(&g);
	GRAPH_CYCLE_CLEAR(&gc);
	
	return 0;
}
