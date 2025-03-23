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
#include "graphbfsp.h"
#include "linearlist.h"

#define INFINITY	UINT_MAX

static void gbfs(struct graph_bfsp *, unsigned int, const struct graph *);

/* 
 * Computes the shortest path between the source vertex s 
 * and every other vertex in the graph g. 
 */
void 
graph_bfsp_init(struct graph_bfsp *bfs, unsigned int s, const struct graph *g)
{
	unsigned int i;
	
	if(s >= g->vertices)
		errmsg_exit("vertex %u is not between 0 and %u\n", s, g->vertices - 1);
	
	bfs->vertices = g->vertices;
	
	bfs->marked = (bool *)algmalloc(g->vertices * sizeof(bool));
	bfs->edgeto = (long *)algmalloc(g->vertices * sizeof(long));
	bfs->distto = (long *)algmalloc(g->vertices * sizeof(long));
	
	for(i = 0; i < bfs->vertices; i++) {
		bfs->marked[i] = false;
		bfs->distto[i] = 0;
		bfs->edgeto[i] = 0;
	}
	
	gbfs(bfs, s, g);
}

/* 
 * Gets a shortest path between the source 
 * vertex s and v, or if no such path. 
 */
void 
graph_bfsp_paths(const struct graph_bfsp *bfs, unsigned int v,
				struct stack *paths)
{
	long w;
	
	if(v >= bfs->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u\n", v,
			bfs->vertices - 1);
	}
	
	if(!GRAPH_BFSP_HASPATH(bfs, v)) {
		return;
	}
	
	STACK_INIT(paths, sizeof(long));
	for(w = v; w != -1 && bfs->distto[w] != 0; 
		w = bfs->edgeto[w]) {
		stack_push(paths, &w);
	}
	stack_push(paths, &w);	/* source vertex */
}

/******************** static function boundary ********************/

/* breadth-first search from a source vertex */
static void 
gbfs(struct graph_bfsp *bfs, unsigned int s, const struct graph *g)
{
	unsigned int i, *v, *w;
	struct queue qu;
	struct single_list *slist;
	struct slist_node *nptr;
	
	QUEUE_INIT(&qu, sizeof(int));
	v = (unsigned int *)algmalloc(sizeof(int));
	
	for(i = 0; i < g->vertices; i++)
		bfs->distto[i] = INFINITY;
	
	bfs->distto[s] = 0;
	bfs->marked[s] = true;
	enqueue(&qu, &s);
	
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&v);
		slist = GRAPH_ADJLIST(g, *v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			if(!bfs->marked[*w]) {
				bfs->marked[*w] = true;
				bfs->edgeto[*w] = *v;
				bfs->distto[*w] = bfs->distto[*v] + 1;
				enqueue(&qu, w);
			}
		}
	}
	queue_clear(&qu);
	ALGFREE(v);
}
