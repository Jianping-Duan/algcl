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
#include "graph.h"
#include "graphgenerator.h"

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct graph g[20];
	int i;
	
	if(argc != 2)
		errmsg_exit("Usage: %s <datafile>\n", argv[0]);
	
	SET_RANDOM_SEED;
	
	printf("Prints a graph from input stream.\n");
	fin = open_file(argv[1], "r");
	graph_init_fistream(&g[0], fin);
	close_file(fin);
	graph_print(&g[0]);
	printf("\n");
	
	printf("Generates a random simple graph.\n");
	simple_graph1(10, 10, &g[1]);
	graph_print(&g[1]);
	printf("\n");
	
	printf("Generates a random simple graphs "
		"using probability p.\n");
	simple_graph2(12, 0.5, &g[2]);
	graph_print(&g[2]);
	printf("\n");
	
	printf("Generates a random simple bipartite graph.\n");
	bipartite_graph1(5, 5, 13, &g[3]);
	graph_print(&g[3]);
	printf("\n");
	
	printf("Generates a random simple bipartite graph "
			"using probability p.\n");
	bipartite_graph2(9, 10, 0.6, &g[4]);
	graph_print(&g[4]);
	printf("\n");
	
	printf("Generates a path graph.\n");
	path_graph(8, &g[5]);
	graph_print(&g[5]);
	printf("\n");
	
	printf("Generates a complete binary tree graph.\n");
	binary_tree_graph(13, &g[6]);
	graph_print(&g[6]);
	printf("\n");
	
	printf("Generates a cycle graph.\n");
	cycle_graph(14, &g[7]);
	graph_print(&g[7]);
	printf("\n");
	
	printf("Generates an Eulerian cycle graph.\n");
	eulerian_cycle_graph(10, 15, &g[8]);
	graph_print(&g[8]);
	printf("\n");
	
	printf("Generates an Eulerian path graph.\n");
	eulerian_path_graph(10, 15, &g[9]);
	graph_print(&g[9]);
	printf("\n");
	
	printf("Generates a wheel graph.\n");
	wheel_graph(10, &g[10]);
	graph_print(&g[10]);
	printf("\n");
	
	printf("Generates a star graph.\n");
	star_graph(10, &g[11]);
	graph_print(&g[11]);
	printf("\n");
	
	printf("Generates a random k-regular graph.\n");
	regular_graph(4, 6, &g[12]);
	graph_print(&g[12]);
	printf("\n");
	
	printf("Generates a random tree graph.\n");
	tree_graph(11, &g[13]);
	graph_print(&g[13]);
	printf("\n");
	
	for(i = 0; i < 14; i++)
		graph_clear(&g[i]);
	
	return 0;
}
