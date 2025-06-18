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
#include "bipartitegraph/bipartitematching.h"
#include "linearlist.h"

static bool is_residual_edge(const struct bipartite_matching *,	unsigned int,
	unsigned int);
static bool has_augment_path(struct bipartite_matching *, const struct graph *);

/* 
 * Determines a maximum matching 
 * (and a minimum vertex cover) in a bipartite graph. 
 */
void 
bipmatch_init(struct bipartite_matching *bm, const struct graph *g)
{
	long v, w, t;

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
	bm->edgeto = (long *)algcalloc(GRAPH_VERTICES(g), sizeof(long));
	
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		/* initialize empty matching */
		bm->mate[v] = BIPARTITE_UNMATCHED;
		bm->mincover[v] = false;
		bm->marked[v] = false;
		bm->edgeto[v] = -1;
	}

	/* alternating path algorithm */
	while (has_augment_path(bm, g)) {
		/* find one endpoint t in alternating path */
		t = -1;
		for (v = 0; v < GRAPH_VERTICES(g); v++)
			if (!BIPMATCH_ISMATCHED(bm, v) && bm->edgeto[v] != -1) {
				t = v;
				break;
			}

		/* 
		 * update the matching according to alternating path in
		 * edgeto[] array.
		 */
		for (v = t; v != -1; v = bm->edgeto[bm->edgeto[v]]) {
			w = bm->edgeto[v];
			bm->mate[v] = w;
			bm->mate[w] = v;
		}

		bm->cardinality++;
	}

	/* find min vertex cover from marked[] array */
	for (v = 0; v < GRAPH_VERTICES(g); v++) {
		if (bigraphbfs_color(bm->bigraph, (unsigned)v) &&
			!bm->marked[v]) {
			bm->mincover[v] = true;
		}
		if (!bigraphbfs_color(bm->bigraph, (unsigned)v) &&
			bm->marked[v]) {
			bm->mincover[v] = true;
		}
	}
}

/******************** static function boundary ********************/

/* 
 * is the edge v-w a forward edge not in the matching 
 * or a reverse edge in the matching? 
 */
static bool 
is_residual_edge(const struct bipartite_matching *bm, unsigned int v,
		unsigned int w)
{
	if (bm->mate[v] != w && bigraphbfs_color(bm->bigraph, v))
		return true;
	if (bm->mate[v] == w && !bigraphbfs_color(bm->bigraph, v))
		return true;
	
	return false;
}

/*
 * Is there an augmenting path?
 *   - if so, upon termination adjlist[] contains the level graph;
 *   - if not, upon termination marked[] specifies those vertices reachable via
 *     an alternating path from one side of the bipartition.
 *
 * An alternating path is a path whose edges belong alternately to the matching
 * and not to the matching.
 *
 * An augmenting path is an alternating path that starts and ends at unmatched
 * vertices.
 *
 * This implementation finds a shortest augmenting path
 * (fewest number of edges), though there is no particular advantage to do so
 * here
 */
static bool 
has_augment_path(struct bipartite_matching *bm,	const struct graph *g)
{
	unsigned int v, *w, *x;
	struct queue qu;
	struct single_list *adj;
	struct slist_node *nptr;

	/* 
	 * breadth-first search (starting from all unmatched vertices on one
	 * side of bipartition)
	 */
	QUEUE_INIT(&qu, sizeof(int));
	for (v = 0; v < bm->vertices; v++)
		if (bigraphbfs_color(bm->bigraph, v) &&
			!BIPMATCH_ISMATCHED(bm, v)) {
			enqueue(&qu, &v);
			bm->marked[v] = true;
		}

	/* run BFS, stopping as soon as an alternating path is found. */
	w = (unsigned int *)algmalloc(sizeof(int));
	while (!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&w);
		adj = GRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(adj, nptr, unsigned int, x) {
			/* 
			 * either (1) forward edge not in matching or
			 * (2) backward edge in matching
			 */
			if (is_residual_edge(bm, *w, *x) && !bm->marked[*x]) {
				bm->marked[*x] = true;
				bm->edgeto[*x] = *w;
				if (!BIPMATCH_ISMATCHED(bm, *x))
					return true;
				enqueue(&qu, x);
			}
		}
	}
	ALGFREE(w);
	queue_clear(&qu);
	
	return false;
}
