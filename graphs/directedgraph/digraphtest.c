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
#include "digraph.h"
#include "digraphgenerator.h"

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct digraph g[20];
	int i;
	
	if (argc != 2)
		errmsg_exit("Usage: %s <datafile>\n", argv[0]);
	
	SET_RANDOM_SEED;
	
	printf("Prints a digraph from input stream.\n");
	fin = open_file(argv[1], "r");
	digraph_init_fistream(&g[0], fin);
	close_file(fin);
	digraph_print(&g[0]);
	printf("\n");
	
	printf("Clones other digraph from the current digraph.\n");
	digraph_clone(&g[0], &g[1]);
	digraph_print(&g[1]);
	printf("\n");
	
	printf("Reverses the previous digraph and print it.\n");
	digraph_reverse(&g[1], &g[2]);
	digraph_print(&g[2]);
	printf("\n");
	
	printf("Generates a random simple digraph.\n");
	simple_digraph1(15, 18, &g[3]);
	digraph_print(&g[3]);
	printf("\n");
	
	printf("Generates a random simple digraph using probability p.\n");
	simple_digraph2(15, 0.5, &g[4]);
	digraph_print(&g[4]);
	printf("\n");
	
	printf("Generates a complete digraph.\n");
	complete_digraph(15, &g[5]);
	digraph_print(&g[5]);
	printf("\n");
	
	printf("Generates a random simple DAG.\n");
	simple_acyclic_digraph(15, 18, &g[6]);
	digraph_print(&g[6]);
	printf("\n");
	
	printf("Generates a random tournament digraph.\n");
	tournament_digraph(8, &g[7]);
	digraph_print(&g[7]);
	printf("\n");
	
	printf("Generates a complete rooted-in DAG.\n");
	complete_rootedin_dag(8, &g[8]);
	digraph_print(&g[8]);
	printf("\n");
	
	printf("Generates a random rooted-in DAG.\n");
	rootedin_dag(15, 18, &g[9]);
	digraph_print(&g[9]);
	printf("\n");
	
	printf("Generates a complete rooted-out DAG.\n");
	complete_rootedout_dag(8, &g[10]);
	digraph_print(&g[10]);
	printf("\n");
	
	printf("Generates a random rooted-out DAG.\n");
	rootedout_dag(15, 18, &g[11]);
	digraph_print(&g[11]);
	printf("\n");
	
	printf("Generates a path digraph.\n");
	path_digraph(15, &g[12]);
	digraph_print(&g[12]);
	printf("\n");
	
	printf("Generates a complete binary tree digraph.\n");
	binary_tree_digraph(15, &g[13]);
	digraph_print(&g[13]);
	printf("\n");
	
	printf("Generates a cycle digraph.\n");
	cycle_digraph(15, &g[14]);
	digraph_print(&g[14]);
	printf("\n");
	
	printf("Generates an Eulerian cycle digraph.\n");
	eulerian_cycle_digraph(15, 20, &g[15]);
	digraph_print(&g[15]);
	printf("\n");
	
	printf("Generates an Eulerian path digraph.\n");
	eulerian_path_digraph(15, 20, &g[16]);
	digraph_print(&g[16]);
	printf("\n");
	
	printf("Generates a random simple digraph that have strong "
		"components.\n");
	strong_component_digraph(15, 25, 5, &g[17]);
	digraph_print(&g[17]);
	printf("\n");
	
	for (i = 0; i < 18; i++)
		digraph_clear(&g[i]);
	
	return 0;
}
