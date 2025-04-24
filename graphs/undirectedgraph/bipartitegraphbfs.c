/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, 2025 Jianping Duan <static.integer@hotmail.com>
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
 * 3. Neither the name of the University nor the names of its contributors
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
#include "bipartitegraph/bipartitegraphbfs.h"
#include "linearlist.h"

static void bfs(struct bipartite_graph_bfs *, const struct graph *,
	unsigned int);

/* 
 * Determines whether an undirected graph is bipartite 
 * and finds either a bipartition or an odd-length cycle.
 */
void 
bigraphbfs_get(struct bipartite_graph_bfs *bg, const struct graph *g)
{
	unsigned int v;
	
	bg->isbipartite = true;
	bg->vertices = GRAPH_VERTICES(g);
	QUEUE_INIT(&bg->cycle, sizeof(int));
	
	bg->marked = (bool *)algmalloc(GRAPH_VERTICES(g) * sizeof(bool));
	bg->color = (enum bigraph_color *)
		algmalloc(GRAPH_VERTICES(g) * sizeof(enum bigraph_color));
	bg->edgeto = (long *)algmalloc(GRAPH_VERTICES(g) * sizeof(long));
	
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		bg->marked[v] = false;
		bg->color[v] = BLACK;
		bg->edgeto[v] = -1;
	}
	
	for (v = 0; v < GRAPH_VERTICES(g) && bg->isbipartite; v++)
		if (!bg->marked[v])
			bfs(bg, g, v);
}

/* Returns the side of the bipartite that vertex v is on. */
enum bigraph_color 
bigraphbfs_color(struct bipartite_graph_bfs *bg, 
				unsigned int v)
{
	if (v >= bg->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			bg->vertices - 1);
	}
	if (!bg->isbipartite)
		errmsg_exit("graph is not bipartite.\n");
	
	return bg->color[v];
}

/******************** static function boundary ********************/

static void 
bfs(struct bipartite_graph_bfs *bg, const struct graph *g, unsigned int s)
{
	struct queue qu;
	struct stack st;
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *v, *w, x, y;
	
	QUEUE_INIT(&qu, sizeof(int));
	bg->marked[s] = true;
	bg->color[s] = WHITE;
	enqueue(&qu, &s);
	
	v = (unsigned int *)algmalloc(sizeof(int));
	while (!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&v);
		slist = GRAPH_ADJLIST(g, *v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			if (!bg->marked[*w]) {
				bg->marked[*w] = true;
				bg->edgeto[*w] = *v;
				bg->color[*w] = !bg->color[*v];
				enqueue(&qu, w);
			} else if (bg->color[*w] == bg->color[*v]) {
				bg->isbipartite = false;
				
				/* 
				 * To form odd cycle, consider s-v path and s-w path and let x
				 * be closest node to v and w common to two paths then
				 * (w-x path) + (x-v path) + (edge v-w) is an odd-length cycle.
				 * Note: distTo[v] == distTo[w];
				 */
				STACK_INIT(&st, sizeof(int));
				x = *v, y = *w;
				while (x != y) {
					stack_push(&st, &x);
					enqueue(&bg->cycle, &y);
					x = (unsigned)bg->edgeto[y];
					y = (unsigned)bg->edgeto[y];
				}
				stack_push(&st, &x);
				
				while (!STACK_ISEMPTY(&st)) {
					stack_pop(&st, (void **)&v);
					enqueue(&bg->cycle, v);
				}
				enqueue(&bg->cycle, w);
				
				stack_clear(&st);
				return;
			}
		}
	}
	
	ALGFREE(v);
	queue_clear(&qu);
}
