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
#include "bipartitegraph/hopcroftkarp.h"
#include "linearlist.h"

static bool is_residual_edge(const struct hopcroft_karp *, unsigned int,
	unsigned int);
static inline bool is_level_edge(const struct hopcroft_karp *, unsigned int,
	unsigned int);
static bool has_augment_path(struct hopcroft_karp *, const struct graph *);

/* 
 * Determines a maximum matching (and a minimum vertex cover)
 * in a bipartite graph.
 */
void 
hkbipmatch_init(struct hopcroft_karp *bm, const struct graph *g)
{
	unsigned int v, *w, *x, *y, *z;
	struct single_list *adjlist, **adj;
	struct slist_node *nptr, *loc = NULL;
	struct stack path;
	long vflag = -1;

	bm->bigraph = (struct bipartite_graph_bfs *)
		algmalloc(sizeof(struct bipartite_graph_bfs));
	bigraphbfs_get(bm->bigraph, g);
	if (!BIGRAPHBFS_ISBIPARTITE(bm->bigraph)) {
		BIGRAPHBFS_CLEAR(bm->bigraph);
		ALGFREE(bm->bigraph);
		errmsg_exit("graph is not bipartite.\n");
	}

	bm->vertices = GRAPH_VERTICES(g);
	bm->cardinality = 0;

	bm->mate = (long *)algcalloc(GRAPH_VERTICES(g), sizeof(long));
	bm->mincover = (bool *)algcalloc(GRAPH_VERTICES(g), sizeof(bool));
	bm->marked = (bool *)algcalloc(GRAPH_VERTICES(g), sizeof(bool));
	bm->distto = (unsigned int *)algcalloc(GRAPH_VERTICES(g), sizeof(int));
	
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		/* initialize empty matching */
		bm->mate[v] = BIPARTITE_UNMATCHED;
		bm->mincover[v] = false;
		bm->marked[v] = false;
		bm->distto[v] = UINT_MAX;
	}

	/* 
	 * To be able to iterate over each adjacency list, keeping track of which
	 * vertex in each adjacency list needs to be explored next.
	 */
	adj = (struct single_list **)
		algcalloc(GRAPH_VERTICES(g), sizeof(struct single_list *));
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		adj[v] = (struct single_list *)algmalloc(sizeof(struct single_list));
		slist_init(adj[v], sizeof(int), NULL);

		adjlist = GRAPH_ADJLIST(g, v);
		SLIST_FOREACH(adjlist, nptr, unsigned int, w) {
			slist_append(adj[v], w);
		}
	}

	/* 
	 * The call to has_augment_path() provides enough info to reconstruct
	 * level graph.
	 */
	STACK_INIT(&path, sizeof(int));
	x = (unsigned int *)algmalloc(sizeof(int));
	y = (unsigned int *)algmalloc(sizeof(int));
	z = (unsigned int *)algmalloc(sizeof(int));
	while (has_augment_path(bm, g)) {
		/* For each unmatched vertex v on one side of bipartition. */
		for (v = 0; v < GRAPH_VERTICES(g); v++) {
			if (HKBIPMATCH_ISMATCHED(bm, v) || !bigraphbfs_color(bm->bigraph, v))
				continue;

			/* Find augmenting path from s using nonrecursive DFS. */
			stack_push(&path, &v);
			while (!STACK_ISEMPTY(&path)) {
				w = (unsigned int *)stack_peek(&path);

				if (*w != vflag) {
					vflag = *w;
					slist_rewind(adj[*w], &loc);
				}

				/* Retreat, no more edges in level graph leaving w. */
				if (!slist_has_next(loc))
					stack_pop(&path, (void **)&x);
				/* advance */
				else {
					/* 
					 * Process edge v-w only if it is an edge in level graph.
					 */
					x = NULL;
					x = (unsigned int *)slist_next_key(&loc);
					if (!is_level_edge(bm, *w, *x))
						continue;
					/* add w to augmenting path */
					stack_push(&path, x);

					/* 
					 * Augmenting path found: update the matching.
					 */
					if (!HKBIPMATCH_ISMATCHED(bm, *x)) {
						while (!STACK_ISEMPTY(&path)) {
							stack_pop(&path, (void **)&y);
							stack_pop(&path, (void **)&z);
							bm->mate[*y] = *z;
							bm->mate[*z] = *y;
						}
						bm->cardinality++;
					} else
						stack_pop(&path, (void **)&x);
				}
			}
			stack_clear(&path);
		}
	}

	/* also find a min vertex cover */
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		if (bigraphbfs_color(bm->bigraph, v) && !bm->marked[v])
			bm->mincover[v] = true;
		if (!bigraphbfs_color(bm->bigraph, v) && bm->marked[v])
			bm->mincover[v] = true;
	}

	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		slist_clear(adj[v]);
		ALGFREE(adj[v]);
	}
	ALGFREE(adj);
	ALGFREE(x);
	ALGFREE(y);
	ALGFREE(z);
}

/******************** static function boundary ********************/

/* 
 * Is the edge v-w a forward edge not in the matching
 * or a reverse edge in the matching? 
 */
static bool 
is_residual_edge(const struct hopcroft_karp *bm, unsigned int v, unsigned int w)
{
	if (bm->mate[v] != w && bigraphbfs_color(bm->bigraph, v))
		return true;
	if (bm->mate[v] == w && !bigraphbfs_color(bm->bigraph, v))
		return true;
	return false;
}

/* Is the edge v-w in the level graph? */
static inline bool 
is_level_edge(const struct hopcroft_karp *bm, unsigned int v, unsigned int w)
{
	return bm->distto[w] == bm->distto[v] + 1 && is_residual_edge(bm, v, w);
}

/*
 * is there an augmenting path?
 *   - if so, upon termination adjlist[] contains the level graph;
 *   - if not, upon termination marked[] specifies those vertices reachable via
 *	   an alternating path from one side of the bipartition.
 *
 * An alternating path is a path whose edges belong 
 * alternately to the matching and not to the matching.
 *
 * An augmenting path is an alternating path that 
 * starts and ends at unmatched vertices.
 *
 * This implementation finds a shortest augmenting path
 * (fewest number of edges), though there is no particular
 * advantage to do so here.
 */
static bool 
has_augment_path(struct hopcroft_karp *bm, const struct graph *g)
{
	unsigned int v, *w, *x;
	struct queue qu;
	struct single_list *adj;
	struct slist_node *nptr;
	bool haspath = false;

	/* 
	 * Breadth-first search (starting from all unmatched 
	 * vertices on one side of bipartition) 
	 */
	QUEUE_INIT(&qu, sizeof(int));
	for (v = 0; v < bm->vertices; v++)
		if (bigraphbfs_color(bm->bigraph, v) && !HKBIPMATCH_ISMATCHED(bm, v)) {
			enqueue(&qu, &v);
			bm->marked[v] = true;
			bm->distto[v] = 0;
		}

	/* 
	 * Run BFS until an augmenting path is found
	 * (and keep going until all vertices at that distance are explored)
	 */
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&w);
		adj = GRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(adj, nptr, unsigned int, x) {
			/* 
			 * either (1) forward edge not in matching
			 * or (2) backward edge in matching.
			 */
			if (is_residual_edge(bm, *w, *x) && !bm->marked[*x]) {
				bm->marked[*x] = true;
				bm->distto[*x] = bm->distto[*w] + 1;
				if (!HKBIPMATCH_ISMATCHED(bm, *x))
					haspath = true;

				/* 
				 * Stop enqueuing vertices once an alternating path has been
				 * discovered (no vertex on same side will be marked
				 * if its shortest path distance longer) 
				 */
				if (!haspath)
					enqueue(&qu, x);
			}
		}
	}
	ALGFREE(w);
	queue_clear(&qu);
	
	return haspath;
}
