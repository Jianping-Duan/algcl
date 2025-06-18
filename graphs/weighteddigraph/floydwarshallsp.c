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
#include "floydwarshallsp.h"
#include "singlelist.h"

/* 
 * Computes a shortest paths tree from each vertex to every other vertex in
 * the edge-weighted digraph g. 
 * If no such shortest path exists for some pair of vertices,
 * it computes a negative cycle.
 */
void 
fwsp_init(struct floyd_warshall_sp *sp, const struct adjmatrix_ewdigraph *g)
{
	unsigned int i, v, w;
	struct diedge *adj, e;

	sp->vertices = ADJMATEWDG_VERTICES(g);
	sp->negcycle = false;

	sp->distto = (float **)
		algcalloc(ADJMATEWDG_VERTICES(g), sizeof(float *));
	sp->edgeto = (struct diedge **)
		algcalloc(ADJMATEWDG_VERTICES(g), sizeof(struct diedge *));
	for (v = 0; v < ADJMATEWDG_VERTICES(g); v++) {
		sp->distto[v] = (float *)
			algcalloc(ADJMATEWDG_VERTICES(g), sizeof(float));
		sp->edgeto[v] = (struct diedge *)
			algcalloc(ADJMATEWDG_VERTICES(g),
			sizeof(struct diedge));
	}

	/* initialize distances to infinity */
	for (v = 0; v < ADJMATEWDG_VERTICES(g); v++)
		for (w = 0; w < ADJMATEWDG_VERTICES(g); w++) {
			sp->distto[v][w] = INFINITY;
			diedge_set_value(&(sp->edgeto[v][w]), -1, -1, 0.0);
		}

	/* initialize distances using edge-weighted digraph's */
	for (v = 0; v < ADJMATEWDG_VERTICES(g); v++) {
		adj = ADJMATEWDG_ADJMAT(g, v);
		for (w = 0; w < ADJMATEWDG_VERTICES(g); w++) {
			e = adj[w];
			if (diedge_isvalid(&e)) {
				sp->distto[DIEDGE_FROM(&e)][DIEDGE_TO(&e)] =
					DIEDGE_WEIGHT(&e);
				sp->edgeto[DIEDGE_FROM(&e)][DIEDGE_TO(&e)] = e;
			}
		}

		/* in case of self-loops */
		if (sp->distto[v][v] > (float)0.0) {
			sp->distto[v][v] = 0.0;
			diedge_set_value(&(sp->edgeto[v][v]), -1, -1, 0.0);
		}
	}

	/* Floyd-Warshall updates */
	for (i = 0; i < ADJMATEWDG_VERTICES(g); i++)
		/* 
		 * compute shortest paths using only 0, 1, ..., i
		 * as intermediate vertices.
		 */
		for (v = 0; v < ADJMATEWDG_VERTICES(g); v++) {
			if (!diedge_isvalid(&(sp->edgeto[v][i])))
				continue;	/* optimization */
			for (w = 0; w < ADJMATEWDG_VERTICES(g); w++)
				if (sp->distto[v][w] > sp->distto[v][i] +
					sp->distto[i][w]) {
					sp->distto[v][w] = sp->distto[v][i] +
						sp->distto[i][w];
					sp->edgeto[v][w] = sp->edgeto[i][w];
				}

			/* check for negative cycle */
			if (sp->distto[v][v] < (float)0.0) {
				sp->negcycle = true;
				return;
			}
		}
}

/* Returns a shortest path from vertex s to vertex t*/
void 
fwsp_path_get(const struct floyd_warshall_sp *sp, unsigned int s,
	unsigned int t, struct single_list *paths)
{
	struct diedge e;

	if (!FWSP_HAS_PATH(sp, s, t)) {
		slist_init(paths, 0, NULL);
		return;
	}

	slist_init(paths, sizeof(struct diedge), NULL);
	for (e = sp->edgeto[s][t]; diedge_isvalid(&e);
		e = sp->edgeto[s][DIEDGE_FROM(&e)]) {
		slist_put(paths, &e);
	}
}

void 
fwsp_clear(struct floyd_warshall_sp *sp)
{
	unsigned int v;

	if (sp->vertices == 0)
		return;

	for (v = 0; v < sp->vertices; v++) {
		ALGFREE(sp->distto[v]);
		ALGFREE(sp->edgeto[v]);
	}
	ALGFREE(sp->distto);
	ALGFREE(sp->edgeto);

	sp->vertices = 0;
}
