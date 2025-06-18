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
#include "eulergraph/directedeulerian.h"
#include "linearlist.h"

static long nonisolated_vertex(const struct digraph *);

/* 
 * Computes an Eulerian cycle in the specified digraph,
 * if one exists.
 */
void 
dieuler_cycle_get(struct stack *cycle, const struct digraph *g)
{
	unsigned int v, *w;
	long ns;
	struct queue *adj;
	struct stack st;
	struct single_list *slist;
	struct slist_node *nptr;

	/* initialize empty stack */
	STACK_INIT(cycle, sizeof(int));
	
	/* must have at least one edge */
	if (DIGRAPH_EDGES(g) == 0)
		return;
	
	/* 
	 * Necessary condition: indegree(v) = outdegree(v)
	 * for each vertex v (without this check, DFS might
	 * return a path instead of a cycle) 
	 */
	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		if (DIGRAPH_OUTDEGREE(g, v) != DIGRAPH_INDEGREE(g, v))
			return;
	
	/* Initialize stack with any non-isolated vertex */
	if ((ns = nonisolated_vertex(g)) == -1)
		return;
	STACK_INIT(&st, sizeof(int));
	stack_push(&st, (unsigned int *)&ns);
	
	/* 
	 * Create local view of adjacency lists,
	 * to iterate one vertex at a time.
	 */
	adj = (struct queue *)
		algcalloc(DIGRAPH_VERTICES(g), sizeof(struct queue));
	for (v = 0; v < DIGRAPH_VERTICES(g); v++) {
		QUEUE_INIT(&adj[v], sizeof(int));
		slist = DIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			enqueue(&adj[v], w);
		}
	}

	/* 
	 * Greedily add to putative cycle, 
	 * depth-first search style.
	 */
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!STACK_ISEMPTY(&st)) {
		stack_pop(&st, (void **)&w);
		while (!QUEUE_ISEMPTY(&adj[*w])) {
			stack_push(&st, w);
			dequeue(&adj[*w], (void **)&w);
		}
		/* add vertex with no more leaving edges to cycle */
		stack_push(cycle, w);
	}

	stack_clear(&st);
	ALGFREE(w);

	/* check if all edges have been used */
	assert(STACK_SIZE(cycle) == DIGRAPH_EDGES(g) + 1);
}

/* 
 * Computes an Eulerian path in the specified digraph, 
 * if one exists.
 */
void 
dieuler_path_get(struct stack *path, const struct digraph *g)
{
	unsigned int v, *w;
	int deficit;
	long ns;
	struct queue *adj;
	struct stack st;
	struct single_list *slist;
	struct slist_node *nptr;

	/* initialize empty stack */
	STACK_INIT(path, sizeof(int));

	/* 
	 * Find vertex from which to start potential Eulerian
	 * path: a vertex v with outdegree(v) > indegree(v)
	 * if it exits; otherwise a vertex with 
	 * outdegree(v) > 0.
	 */
	ns = nonisolated_vertex(g);
	for (v = 0, deficit = 0; v < DIGRAPH_VERTICES(g); v++)
		if (DIGRAPH_OUTDEGREE(g, v) > DIGRAPH_INDEGREE(g, v)) {
			deficit += (DIGRAPH_OUTDEGREE(g, v) -
				DIGRAPH_INDEGREE(g, v));
			ns = v;
		}

	/* digraph can't have an Eulerian path */
	if (deficit > 1)
		return;

	/* 
	 * Special case for digraph with zero edges
	 * (has a degenerate Eulerian path)
	 */
	if (ns == -1)
		ns = 0;

	/* 
	 * Create local view of adjacency lists,
	 * to iterate one vertex at a time.
	 */
	adj = (struct queue *)
		algcalloc(DIGRAPH_VERTICES(g), sizeof(struct queue));
	for (v = 0; v < DIGRAPH_VERTICES(g); v++) {
		QUEUE_INIT(&adj[v], sizeof(int));
		slist = DIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			enqueue(&adj[v], w);
		}
	}

	/* 
	 * Greedily add to putative path,
	 * depth-first search style.
	 */
	STACK_INIT(&st, sizeof(int));
	stack_push(&st, (unsigned int *)&ns);
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!STACK_ISEMPTY(&st)) {
		stack_pop(&st, (void **)&w);
		while (!QUEUE_ISEMPTY(&adj[*w])) {
			stack_push(&st, w);
			dequeue(&adj[*w], (void **)&w);
		}
		/* add vertex with no more leaving edges to path */
		stack_push(path, w);
	}

	stack_clear(&st);
	ALGFREE(w);

	/* check if all edges have been used */
	assert(STACK_SIZE(path) == DIGRAPH_EDGES(g) + 1);
}

/******************** static function boundary ********************/

/* 
 * Returns any non-isolated vertex; 
 * -1 if no such vertex.
 */
static long 
nonisolated_vertex(const struct digraph *g)
{
	unsigned int v;

	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		if (DIGRAPH_OUTDEGREE(g, v) > 0)
			return v;
	return -1;
}
