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
#include "boruvkamst.h"
#include "singlelist.h"

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct ewgraph g;
	struct single_list mst;
	struct slist_node *nptr;
	struct edge *e;
	char se[128];
	double weight;
	clock_t start_time, end_time;
	
	if (argc != 2) 
		errmsg_exit("Usage: %s <datafile>\n", argv[0]);

	SET_RANDOM_SEED;
	
	printf("Prints a graph from input stream.\n");
	fin = open_file(argv[1], "r");
	ewgraph_init_fistream(&g, fin);
	close_file(fin);
	/* ewgraph_print(&g); */
	printf("\n");

	printf("It's a minimum spanning tree (or forest).\n");
	start_time = clock();
	weight = (double)boruvka_mst_get(&g, &mst);
	SLIST_FOREACH(&mst, nptr, struct edge, e) {
		EDGE_STRING(e, se);
		printf("%s\n", se);
	}
	end_time = clock();
	printf("Total weights: %.5f\n", weight);
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	
	slist_clear(&mst);
	ewgraph_clear(&g);

	return 0;
}
