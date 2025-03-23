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
#include "graphconcomp.h"
#include "queue.h"

int
main(int argc, char *argv[])
{
	FILE *fin;
	struct graph g;
	struct graph_cc gc;
	unsigned int m, i, v, *w;
	struct queue *comps;
	
	if(argc != 2)
		errmsg_exit("Usage: %s <datafile> \n", argv[0]);

	SET_RANDOM_SEED;
	
	fin = open_file(argv[1], "r");
	graph_init_fistream(&g, fin);
	close_file(fin);
	graph_print(&g);
	printf("\n");
	
	graphcc_init(&gc, &g);
	m = GRAPHCC_COUNT(&gc);
	printf("%u components.\n", m);
	
	/* 
	 * compute list of vertices in 
	 * each connected component.
	 */
	comps = (struct queue *)algmalloc(m * sizeof(struct queue));
	for(i = 0; i < m; i++)
		QUEUE_INIT(&comps[i], sizeof(int));
	
	for(v = 0; v < GRAPH_VERTICES(&g); v++)
		enqueue(&comps[GRAPHCC_ID(&gc, v)], &v);
	
	/* print result */
	for(i = 0; i < m; i++) {
		while(!QUEUE_ISEMPTY(&comps[i])) {
			dequeue(&comps[i], (void **)&w);
			printf("%u ", *w);
		}
		printf("\n");
	}
	
	for(i = 0; i < m; i++)
		queue_clear(&comps[i]);
	ALGFREE(comps);
	
	graph_clear(&g);
	GRAPHCC_CLEAR(&gc);
	
	return 0;
}
