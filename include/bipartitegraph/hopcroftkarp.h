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
#ifndef _HOPCROFTKARP_H_
#define _HOPCROFTKARP_H_

#include "bipartitegraph/bipartitegraphbfs.h"

/* Hopcroft-Karp bipartite matching */
struct hopcroft_karp {
	unsigned int vertices;	/* number of vertices in the graph */
	struct bipartite_graph_bfs *bigraph; /* bipartition */
	unsigned int cardinality;	/* cardinality of current matching */
	long *mate;	/* mate[v] =  w if v-w is an edge in current matching,
				   mate[v] =  w if v-w is an edge in current matching,
				   = -1 if v is not in current matching. */
	bool *mincover;	/* mincover[v] = true iff v is in min vertex cover */
	bool *marked;	/* marked[v] = true iff v is reachable via alternating
					   path */
	unsigned int *distto;	/* distto[v] = number of edges on shortest path
							   to v */
};

#ifdef BIPARTITE_UNMATCHED
#undef BIPARTITE_UNMATCHED
#endif

#define BIPARTITE_UNMATCHED	-1

/* 
 * Returns the vertex to which the specified vertex is matched in the maximum
 * matching computed by the algorithm. 
 */
#define HKBIPMATCH_MATE(bm, v)	\
	((v) >= (bm)->vertices ? BIPARTITE_UNMATCHED : (bm)->mate[v])

/* 
 * Returns true if the specified vertex is matched 
 * in the maximum matching computed by the algorithm. 
 */
#define HKBIPMATCH_ISMATCHED(bm, v)	\
   (HKBIPMATCH_MATE(bm, v) != BIPARTITE_UNMATCHED)

/* Returns the number of edges in a maximum matching. */
#define HKBIPMATCH_SIZE(bm)	((bm)->cardinality)

/* 
 * Returns true if the graph contains a perfect matching.
 * That is, the number of edges in a maximum matching is  equal to one half of
 * the number of vertices in the graph (so that every vertex is matched). 
 */
#define HKBIPMATCH_ISPERFECT(bm)	\
   ((bm)->cardinality * 2 == (bm)->vertices)

/* 
 * Returns true if the specified vertex is in the minimum vertex cover
 * computed by the algorithm. 
 */
#define HKBIPMATCH_MIN_COVER(bm, v)	\
	((v) >= (bm)->vertices ? false : (bm)->mincover[v])

#define HKBIPMATCH_CLEAR(bm)	do {	\
	(bm)->vertices = 0;					\
	(bm)->cardinality = 0;				\
	BIGRAPHBFS_CLEAR((bm)->bigraph);	\
	ALGFREE((bm)->bigraph);				\
	ALGFREE((bm)->mate);				\
	ALGFREE((bm)->mincover);			\
	ALGFREE((bm)->marked);				\
	ALGFREE((bm)->distto);				\
} while (0)

/* 
 * Determines a maximum matching (and a minimum vertex cover)
 * in a bipartite graph. 
 */
void hkbipmatch_init(struct hopcroft_karp *bm, const struct graph *g);

#endif	/* _HOPCROFTKARP_H_ */
