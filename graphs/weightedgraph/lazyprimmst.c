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
#include "lazyprimmst.h"
#include "linearlist.h"
#include "pairingheap.h"
#include <math.h>	/* islessequal() */

/* minimum rooted-heap */
static int lessequal(const void *, const void *);
static void scan(struct lazy_prim_mst *, const struct ewgraph *, unsigned int);
static void prim(struct lazy_prim_mst *, const struct ewgraph *, unsigned int);

/* 
 * Compute a minimum spanning tree (or forest) 
 * of an edge-weighted graph. 
 */
void 
lpmst_init(struct lazy_prim_mst *lpmst, const struct ewgraph *g)
{
	unsigned int v;

	lpmst->marked = (bool *)algcalloc(EWGRAPH_VERTICES(g), sizeof(bool));
	lpmst->mst = (struct single_list *)algmalloc(sizeof(struct single_list));
	lpmst->pq = (struct pairing_heap *)algmalloc(sizeof(struct pairing_heap));

	pheap_init(lpmst->pq, 0, lessequal);
	slist_init(lpmst->mst, sizeof(struct edge), edge_equals);
	lpmst->weight = 0.0;

	for (v = 0; v < EWGRAPH_VERTICES(g); v++)
		lpmst->marked[v] = false;

	for (v = 0; v < EWGRAPH_VERTICES(g); v++)
		if (!lpmst->marked[v])
			prim(lpmst, g, v);
}

/******************** static function boundary ********************/

/* minimum rooted-heap */
static int 
lessequal(const void *k1, const void *k2)
{
	struct edge *e1, *e2;

	e1 = (struct edge *)k1, e2 = (struct edge *)k2;
	return islessequal(e1->weight, e2->weight) ? 1 : -1;
}

/* 
 * Add all edges e incident to v onto pq
 * if the other endpoint has not yet been scanned.
 */
static void 
scan(struct lazy_prim_mst *lpmst, const struct ewgraph *g, unsigned int v)
{
	struct single_list *adj;
	struct slist_node *nptr;
	struct edge *e;

	assert(!lpmst->marked[v]);

	lpmst->marked[v] = true;
	adj = EWGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, struct edge, e) {
		if (!lpmst->marked[edge_other(e, v)])
			pheap_insert(lpmst->pq, e);
	}
}

/* Prim's algorithm */
static void 
prim(struct lazy_prim_mst *lpmst, const struct ewgraph *g, unsigned int s)
{
	unsigned int v, w;
	struct edge *e;

	scan(lpmst, g, s);

	/* better to stop when mst has V-1 edges */
	while (!PHEAP_ISEMPTY(lpmst->pq)) {
		/* smallest edge on pq */
		e = (struct edge *)pheap_delete(lpmst->pq);
		v = EDGE_EITHER(e); /* two endpoints */
		w = edge_other(e, v);

		/* lazy, both v and w already scanned */
		if (lpmst->marked[v] && lpmst->marked[w])
			continue;

		slist_append(lpmst->mst, e); /* add e to MST */
		lpmst->weight += EDGE_WEIGHT(e);

		if (!lpmst->marked[v])
			scan(lpmst, g, v); /* v becomes part of tree */

		if (!lpmst->marked[w])
			scan(lpmst, g, w); /* w becomes part of tree */
	}
}
