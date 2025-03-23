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
#include "dijkstrapairsp.h"
#include "singlelist.h"

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct ewdigraph g;
	struct dijkstra_pair_sp psp;
	struct single_list *paths;
	struct slist_node *nptr;
	struct diedge *e;
	unsigned int v, w;
	char se[128];
	
	if(argc != 2)
		errmsg_exit("Usage: %s <datafile> \n", argv[0]);

	printf("Prints a edge-weighted digraph from input stream.\n");
	fin = open_file(argv[1], "r");
	ewdigraph_init_fistream(&g, fin);
	close_file(fin);
	if(EWDIGRAPH_VERTICES(&g) <= 100)
		ewdigraph_print(&g);
	else
		printf("Vertices are too many!!!\n");
	printf("\n");

	djkr_pairsp_init(&psp, &g);
	for(v = 0; v < EWDIGRAPH_VERTICES(&g); v++)
		for(w = 0; w < EWDIGRAPH_VERTICES(&g); w++)
			if(DJKR_PAIRSP_HAS_PATH(&psp, v, w)) {
				printf("%u to %u (%5.3f)  ", v, w, 
					(double)djkr_pairsp_dist(&psp, v, w));
				paths = djkr_pairsp_path(&psp, v, w);
				SLIST_FOREACH(paths, nptr, struct diedge, e) {
					DIEDGE_STRING(e, se);
					printf("%s ", se);
				}
				printf("\n");
				ALGFREE(paths);
			}
			else
				printf("%u to %u no path.\n", v, w);

	djkr_pairsp_clear(&psp);
	ewdigraph_clear(&g);

	return 0;
}
