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
#ifndef _LAZYPRIMMAST_H_
#define _LAZYPRIMMAST_H_

#include "edgeweightedgraph.h"

struct single_list;
struct pairing_heap;

struct lazy_prim_mst {
	bool *marked;	/* marked[v] = true iff v on tree */
	float weight;	/* total weight of MST */
	struct single_list *mst;	/* edges in the MST */
	struct pairing_heap *pq;	/* edges with one endpoint in tree */
};

/* Returns the edges in a minimum spanning tree (or forest) */
#define LPMST_EDGES_GET(lpmst)	((lpmst)->mst)

/* Returns the sum of the edge weights in a minimum spanning tree (or forest) */
#define LPMST_WEIGHT_GET(lpmst)		((lpmst)->weight)

#define LPMST_CLEAR(lpmst)	do {	\
	ALGFREE((lpmst)->marked);	\
	slist_clear((lpmst)->mst);	\
	ALGFREE((lpmst)->mst);		\
	pheap_clear((lpmst)->pq);	\
	ALGFREE((lpmst)->pq);		\
	(lpmst)->weight = 0.0;		\
} while (0)

/* Compute a minimum spanning tree (or forest) of an edge-weighted graph. */
void lpmst_init(struct lazy_prim_mst *lpmst, const struct ewgraph *g);

#endif	/* _LAZYPRIMMAST_H_ */
