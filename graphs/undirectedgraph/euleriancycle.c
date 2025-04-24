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
#include "eulergraph/euleriancycle.h"
#include "graphbfsp.h"
#include "linearlist.h"

/* 
 * an undirected edge, with a field to indicate whether 
 * the edge has already been used.
 */
struct edge {
	unsigned int v;
	unsigned int w;
	bool isused;
};

static inline void set_edge(struct edge *, unsigned int, unsigned int);
static inline unsigned int other_edge(struct edge *, unsigned int);
static long nonisolated_vertex(const struct graph *);

/* 
 * Computes an Eulerian cycle in the specified graph,
 * if one exists. 
 */
void 
eulcycle_get(struct stack *cycle, const struct graph *g)
{
	unsigned int v, *w, k, selfloops;
	long ns;
	struct edge *edges, *e;
	struct queue *adjs;
	struct single_list *slist;
	struct slist_node *nptr;
	struct stack st;
	
	/* initialize empty stack */
	STACK_INIT(cycle, sizeof(int));
	
	/* must have at least one edge */
	if (GRAPH_EDGES(g) == 0)
		return;
	
	/* 
	 * Necessary condition: 
	 * all vertices have even degree.
	 */
    for (v = 0; v < GRAPH_VERTICES(g); v++)
		if (GRAPH_DEGREE(g, v) % 2 != 0)
			return;
		
	/* 
	 * Create local view of adjacency lists, to iterate one vertex at a time.
     * The helper Edge data type is used to avoid exploring
	 * both copies of an edge v-w. 
	 */
	adjs = (struct queue *)algmalloc(GRAPH_VERTICES(g) * sizeof(struct queue));
	for (v = 0; v < GRAPH_VERTICES(g); v++)
		QUEUE_INIT(&adjs[v], 0);
	
	
	edges = (struct edge *)
		algmalloc((GRAPH_EDGES(g) + 1) * sizeof(struct edge));
	
	for (v = 0, k = 0; v < GRAPH_VERTICES(g); v++) {
		selfloops = 0;
		slist = GRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			/* careful with self loops */
			if (v == *w) {
				if (selfloops % 2 == 0) {
					set_edge(&edges[k], v, *w);
					enqueue(&adjs[v], &edges[k]);
					enqueue(&adjs[*w], &edges[k]);
					k++;
				}
				selfloops++;
			} else if (v < *w) {
				set_edge(&edges[k], v, *w);
				enqueue(&adjs[v], &edges[k]);
				enqueue(&adjs[*w], &edges[k]);
				k++;
			}
		}
	}
	
	/* Initialize stack with any non-isolated vertex. */
	ns = nonisolated_vertex(g);
	STACK_INIT(&st, sizeof(long));
	if (ns >= 0)
		stack_push(&st, &ns);
	
	/* 
	 * Greedily search through edges in iterative DFS style.
	 */
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!STACK_ISEMPTY(&st)) {
		stack_pop(&st, (void **)&w);
		while (!QUEUE_ISEMPTY(&adjs[*w])) {
			dequeue(&adjs[*w], (void **)&e);
			if (e->isused)
				continue;
			e->isused = true;
			stack_push(&st, w);
			*w = other_edge(e, *w);
		}
		
		/* 
		 * Push vertex with no more leaving edges to cycle.
		 */
		stack_push(cycle, w);
	}
	
	stack_clear(&st);
	ALGFREE(edges);
	
	/* check if all edges are used */
	assert(STACK_SIZE(cycle) == GRAPH_EDGES(g) + 1);
}

/* 
 * Determines whether a graph has an Eulerian cycle using necessary and
 * sufficient conditions (without computing the cycle itself):
 *   - at least one edge.
 *   - degree(v) is even for every vertex v.
 *   - the graph is connected (ignoring isolated vertices)
 */
bool 
eulcycle_necesuff_condition(const struct graph *g)
{
	unsigned int v;
	struct graph_bfsp bfs;
	long ns;
	
	/* Condition 0: at least 1 edge. */
	if (GRAPH_EDGES(g) == 0)
		return false;
	
	/* Condition 1: degree(v) is even for every vertex. */
	for (v = 0; v < GRAPH_VERTICES(g); v++)
		if (GRAPH_DEGREE(g, v) % 2 != 0)
			return false;
		
	/* Condition 2: graph is connected, ignoring isolated vertices. */
	if ((ns = nonisolated_vertex(g)) == -1)
		return false;
	
	graph_bfsp_init(&bfs, (unsigned int)ns, g);
	for (v = 0; v < GRAPH_VERTICES(g); v++)
		if (GRAPH_DEGREE(g, v) > 0 && !GRAPH_BFSP_HASPATH(&bfs, v))
			return false;
	
	return true;
}

/******************** static function boundary ********************/

static inline void 
set_edge(struct edge *e, unsigned int v, unsigned int w)
{
	e->v = v;
	e->w = w;
	e->isused = false;
}

/* returns the other vertex of the edge */
static inline unsigned int 
other_edge(struct edge *e, unsigned int v)
{
	if (e->v == v)
		return e->w;
	else if (e->w == v)
		return e->v;
	else
		errmsg_exit("Illegal endpoint.\n");

	return 1;
}

/* 
 * Returns any non-isolated vertex; 
 * -1 if no such vertex.
 */
static long 
nonisolated_vertex(const struct graph *g)
{
	unsigned int v;
	
	for (v = 0; v < GRAPH_VERTICES(g); v++)
		if (GRAPH_DEGREE(g, v) > 0)
			return v;
	return -1;
}
