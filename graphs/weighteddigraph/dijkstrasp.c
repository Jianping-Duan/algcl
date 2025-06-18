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
#include "dijkstrasp.h"
#include "indexpairheap.h"
#include "singlelist.h"

/* minimum rooted-heap */
static int lessequal(const void *, const void *);
static void relax(struct dijkstra_sp *, const struct diedge *);

/* 
 * Computes a shortest-paths tree from the source vertex s
 * to every other vertex in the edge-weighted digraph g.
 */
void 
dijkstrasp_init(struct dijkstra_sp *sp, const struct ewdigraph *g,
		unsigned int s)
{
	struct single_list edgeset, *adj;
	struct slist_node *nptr;
	struct diedge *e;
	char se[128];
	unsigned int v;
	
	ewdigraph_edges_get(g, &edgeset);
	SLIST_FOREACH(&edgeset, nptr, struct diedge, e) {
		if (DIEDGE_WEIGHT(e) < 0) {
			DIEDGE_STRING(e, se);
			errmsg_exit("edge %s has negative weight.\n", se);
		}
	}

	if (s >= EWDIGRAPH_VERTICES(g))
		return;

	sp->distto = (float *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(float));
	sp->edgeto = (struct diedge **)
		algcalloc(EWDIGRAPH_VERTICES(g), sizeof(struct diedge *));
	sp->pq = (struct index_pheap *)algmalloc(sizeof(struct index_pheap));
	sp->vertices = EWDIGRAPH_VERTICES(g);

	for (v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		sp->distto[v] = INFINITY;
		sp->edgeto[v] = NULL;
	}
	sp->distto[s] = 0.0;

	ipheap_init(sp->pq, EWDIGRAPH_VERTICES(g), sizeof(float), lessequal);
	ipheap_insert(sp->pq, s, &(sp->distto[s]));
	while (!IPHEAP_ISEMPTY(sp->pq)) {
		v = ipheap_delete(sp->pq);
		adj = EWDIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(adj, nptr, struct diedge, e) {
			relax(sp, e);
		}
	}
}

/* 
 * Gets a shortest path from the source vertex s
 * to vertex v.
 */
void 
dijkstrasp_pathto(const struct dijkstra_sp *sp, unsigned int v,
		struct single_list *paths)
{
	struct diedge *e;

	slist_init(paths, 0, NULL);

	if (v >= sp->vertices)
		return;
	if (!DIJKSTRASP_HAS_PATHTO(sp, v))
		return;

	for (e = sp->edgeto[v]; e != NULL; e = sp->edgeto[DIEDGE_FROM(e)])
		slist_put(paths, e);
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

/* Relax edge e and update pq if changed. */
static void 
relax(struct dijkstra_sp *sp, const struct diedge *e)
{
	unsigned int v, w;

	v = DIEDGE_FROM(e);
	w = DIEDGE_TO(e);

	if (sp->distto[w] > sp->distto[v] + DIEDGE_WEIGHT(e)) {
		sp->distto[w] = sp->distto[v] + DIEDGE_WEIGHT(e);
		sp->edgeto[w] = (void *)e;
		if (IPHEAP_CONTAINS(sp->pq, w))
			ipheap_change(sp->pq, w, &(sp->distto[w]));
		else
			ipheap_insert(sp->pq, w, &(sp->distto[w]));
	}
}
