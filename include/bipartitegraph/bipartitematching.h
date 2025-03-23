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
#ifndef _BIPARTITEMATCHING_H_
#define _BIPARTITEMATCHING_H_

/* 
 * Find a maximum cardinality matching 
 * (and minimum cardinality vertex cover)
 * in a bipartite graph using the alternating 
 * path algorithm.
 *
 * A bipartite graph in a graph whose vertices can be 
 * partitioned into two disjoint sets such that every
 * edge has one endpoint in either set.
 * A matching in a graph is a subset of its edges with 
 * no common vertices. 
 * A maximum matching is a matching with the maximum 
 * number of edges.
 * A perfect matching is a matching which matches all 
 * vertices in the graph.
 * A vertex cover in a graph is a subset of its vertices
 * such that every edge is incident to at least one vertex.
 * A minimum vertex cover is a vertex cover with the 
 * minimum number of vertices.
 * By Konig's theorem, in any bipartite graph, 
 * the maximum number of edges in matching equals the 
 * minimum number of vertices in a vertex cover.
 * The maximum matching problem in nonbipartite graphs is
 * also important, but all known algorithms for this more
 * general problem are substantially more complicated.
 *
 * This implementation uses the alternating-path algorithm.
 * It is equivalent to reducing to the maximum-flow
 * problem and running the augmenting-path algorithm on
 * the resulting flow network, 
 * but it does so with less overhead.
 * The constructor takes O((E + V) V) time, 
 * where E is the number of edges and V is the number of
 * vertices in the graph.
 */

#include "bipartitegraph/bipartitegraphbfs.h"

struct bipartite_matching {
	unsigned int vertices;	/* number of vertices in the graph */
	struct bipartite_graph_bfs *bigraph; /* bipartition */
	unsigned int cardinality;	/* cardinality of current matching */
	long *mate;	/* mate[v] = w if v-w is an edge in current matching,
				   = -1 if v is not in current matching */
	bool *mincover;	/* mincover[v] = true iff v is in min vertex cover */
	bool *marked;	/* marked[v] = true iff v is reachable via alternating
					   path */
	long *edgeto;	/* edgeTo[v] = last edge on alternating path to v */
};

#define BIPARTITE_UNMATCHED	-1

/* 
 * Returns the vertex to which the specified vertex 
 * is matched in the maximum matching computed by 
 * the algorithm. 
 */
#define BIPMATCH_MATE(bm, v)	\
	((v) >= (bm)->vertices \
	? BIPARTITE_UNMATCHED : (bm)->mate[v])

/* 
 * Returns true if the specified vertex is matched in
 * the maximum matching computed by the algorithm. 
 */
#define BIPMATCH_ISMATCHED(bm, v)	\
	(BIPMATCH_MATE(bm, v) != BIPARTITE_UNMATCHED)

/* Returns the number of edges in a maximum matching. */
#define BIPMATCH_SIZE(bm)	((bm)->cardinality)

/* 
 * Returns true if the graph contains a perfect matching.
 * That is, the number of edges in a maximum matching 
 * is equal to one half of the number of vertices in the
 * graph (so that every vertex is matched). 
 */
#define BIPMATCH_ISPERFECT(bm)	\
	((bm)->cardinality * 2 == (bm)->vertices)

/* 
 * Returns true if the specified vertex is in the minimum
 * vertex cover computed by the algorithm. 
 */
#define BIPMATCH_MIN_COVER(bm, v)	\
	((v) >= (bm)->vertices ? false : (bm)->mincover[v])

#define BIPMATCH_CLEAR(bm)		do {	\
	(bm)->vertices = 0;					\
	(bm)->cardinality = 0;				\
	BIGRAPHBFS_CLEAR((bm)->bigraph);	\
	ALGFREE((bm)->bigraph);				\
	ALGFREE((bm)->mate);				\
	ALGFREE((bm)->mincover);			\
	ALGFREE((bm)->marked);				\
	ALGFREE((bm)->edgeto);				\
} while(0)

/* 
 * Determines a maximum matching 
 * (and a minimum vertex cover) in a bipartite graph.
 */
void bipmatch_init(struct bipartite_matching *bm, const struct graph *g);

#endif	/* _BIPARTITEMATCHING_H_ */
