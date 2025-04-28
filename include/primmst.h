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
#ifndef _PRIMMST_H_
#define _PRIMMST_H_

#include "edgeweightedgraph.h"

struct index_pheap;
struct single_list;

struct prim_mst {
	struct edge **edgeto;	/* edgeTo[v] = shortest edge from tree vertex to
							   non-tree vertex */
	float *distto;	/* distTo[v] = weight of shortest such edge */
	bool *marked;	/* marked[v] = true if v on tree, false otherwise */
	struct index_pheap *pq;	/* edge of weight, index is vertex */ 
	unsigned int vertices;	/* edge-weighted graph of vertices */
};

#define PMST_CLEAR(pmst)	do {	\
	ALGFREE((pmst)->edgeto);		\
	ALGFREE((pmst)->distto);		\
	ALGFREE((pmst)->marked);		\
	ipheap_clear((pmst)->pq);		\
	ALGFREE((pmst)->pq);			\
	(pmst)->vertices = 0;			\
} while (0)


/* Compute a minimum spanning tree (or forest) of an edge-weighted graph. */
void pmst_init(struct prim_mst *pmst, const struct ewgraph *g);

/* 
 * Returns the sum of the edge weights in a minimum spanning tree (or forest),
 * and output minimum spanning tree set.
 */
float pmst_edges_get(const struct prim_mst *pmst, struct single_list *mstset);

#endif	/* _PRIMMST_H_ */
