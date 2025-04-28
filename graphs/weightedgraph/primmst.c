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
#include "primmst.h"
#include "linearlist.h"
#include "indexpairheap.h"
#include <math.h>	/* INFINITY, islessequal() */

static int lessequal(const void *, const void *);
static void scan(struct prim_mst *, const struct ewgraph *, unsigned int);
static void prim(struct prim_mst *, const struct ewgraph *, unsigned int);

/* Compute a minimum spanning tree (or forest) of an edge-weighted graph. */
void 
pmst_init(struct prim_mst *pmst, const struct ewgraph *g)
{
	unsigned int v;

	pmst->edgeto = (struct edge **)
		algcalloc(EWGRAPH_VERTICES(g), sizeof(struct edge *));
	pmst->distto = (float *)algcalloc(EWGRAPH_VERTICES(g), sizeof(float));
	pmst->marked = (bool *)algcalloc(EWGRAPH_VERTICES(g), sizeof(bool));
	pmst->pq = (struct index_pheap *)algmalloc(sizeof(struct index_pheap));

	pmst->vertices = EWGRAPH_VERTICES(g);

	for (v = 0; v < EWGRAPH_VERTICES(g); v++) {
		pmst->edgeto[v] = NULL;
		pmst->distto[v] = INFINITY;
		pmst->marked[v] = false;
	}

	ipheap_init(pmst->pq, EWGRAPH_VERTICES(g), sizeof(float), lessequal);

	/* Run from each vertex to find minimum spanning forest. */
	for (v = 0; v < EWGRAPH_VERTICES(g); v++)
		if (!pmst->marked[v])
			prim(pmst, g, v);
}

/* 
 * Returns the sum of the edge weights in a minimum spanning tree (or forest),
 * and output minimum spanning tree set.
 */
float
pmst_edges_get(const struct prim_mst *pmst, struct single_list *mstset)
{
	float wt = 0.0;
	unsigned int v;
	struct edge *e;

	slist_init(mstset, 0, NULL); 

	for (v = 0; v < pmst->vertices; v++) {
		e = pmst->edgeto[v];
		if (e != NULL) {
			slist_append(mstset, e);
			wt += EDGE_WEIGHT(e);
		}
	}

	return wt;
}

/******************** static function boundary ********************/

/* minimum rooted-heap */
static int 
lessequal(const void *k1, const void *k2)
{
	float *e1, *e2;

	e1 = (float *)k1, e2 = (float *)k2;
	return islessequal(*e1, *e2) ? 1 : -1;
}

/* scan vertex v */
static void 
scan(struct prim_mst *pmst, const struct ewgraph *g, unsigned int v)
{
	struct single_list *adj;
	struct slist_node *nptr;
	struct edge *e;
	unsigned int w;

	pmst->marked[v] = true;
	adj = EWGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, struct edge, e) {
		w = edge_other(e, v);
		if (pmst->marked[w]) /* v-w is obsolete edge */
			continue;
		if (EDGE_WEIGHT(e) < pmst->distto[w]) {
			pmst->distto[w] = EDGE_WEIGHT(e);
			pmst->edgeto[w] = e;
			if (!IPHEAP_CONTAINS(pmst->pq, w))
				ipheap_insert(pmst->pq, w, &(pmst->distto[w]));
			else
				ipheap_change(pmst->pq, w, &(pmst->distto[w]));
		}
	}
}

/* 
 * Run Prim's algorithm in edge-weight graph g, 
 * starting from vertex s.
 */
static void 
prim(struct prim_mst *pmst, const struct ewgraph *g, unsigned int s)
{
	unsigned int v;

	pmst->distto[s] = 0.0;
	ipheap_insert(pmst->pq, s, &(pmst->distto[s]));
	while (!IPHEAP_ISEMPTY(pmst->pq)) {
		v = (unsigned int)ipheap_delete(pmst->pq);
		scan(pmst, g, v);
	}
}
