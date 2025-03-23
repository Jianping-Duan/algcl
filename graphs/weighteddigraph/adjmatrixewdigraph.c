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
#include "adjmatrixewdigraph.h"

/* 
 * Initializes an empty edge-weighted digraph 
 * with vs vertices and 0 edges.
 */
void 
adjmatewdg_init(struct adjmatrix_ewdigraph *g, unsigned int vs)
{
	unsigned int v, w;
	struct diedge e;

	g->vertices = vs;
	g->edges = 0;
	g->adjmat = (struct diedge **)algmalloc(vs * sizeof(struct diedge *));
	for(v = 0; v < vs; v++) {
		*(g->adjmat + v) = (struct diedge *)
			algmalloc(vs * sizeof(struct diedge));
	}

	for(v = 0; v < vs; v++)
		for(w = 0; w < vs; w++) {
			diedge_set_value(&e, -1, -1, 0.0);
			g->adjmat[v][w] = e;
		}
}

/* 
 * Adds the directed edge e to the edge-weighted digraph
 * (if there is not already an edge with 
 * the same endpoints) 
 */
void 
adjmatewdg_add_edge(struct adjmatrix_ewdigraph *g, const struct diedge *e)
{
	long v, w;

	v = DIEDGE_FROM(e);
	w = DIEDGE_TO(e);

	if(v < 0 || v >= ADJMATEWDG_VERTICES(g)) {
		errmsg_exit("Vertex %lu is not between 0 and %u.\n", v,
			ADJMATEWDG_VERTICES(g) - 1);
	}

	if(w < 0 || w >= ADJMATEWDG_VERTICES(g)) {
		errmsg_exit("Vertex %lu is not between 0 and %u.\n", w,
			ADJMATEWDG_VERTICES(g) - 1);
	}

	if(!diedge_isvalid(&(g->adjmat[v][w]))) {
		g->adjmat[v][w] = *e;
		g->edges++;
	}
}

/* 
 * Initializes a random edge-weighted digraph 
 * with vs vertices and es edges. 
 */
void 
adjmatewdg_init_randomly(struct adjmatrix_ewdigraph *g, unsigned int vs,
						unsigned int es)
{
	unsigned int v, w, cnt = 0;;
	float wt;
	struct diedge e;

	if(es > vs * vs)
		errmsg_exit("Too many edges.\n");

	adjmatewdg_init(g, vs);
	while(cnt++ != es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		wt = (float)0.01 * (float)(rand_range_integer(1, 100));
		diedge_set_value(&e, v, w, wt);
		adjmatewdg_add_edge(g, &e);
	}
}

void 
adjmatewdg_print(const struct adjmatrix_ewdigraph *g)
{
	struct diedge *adj, e;
	unsigned int v, w;
	char se[128];

	printf("%u vertices, %u edges\n", ADJMATEWDG_VERTICES(g),
		ADJMATEWDG_EDGES(g));
	for(v = 0; v < ADJMATEWDG_VERTICES(g); v++) {
		printf("%u: ", v);
		adj = ADJMATEWDG_ADJMAT(g, v);
		for(w = 0; w < ADJMATEWDG_VERTICES(g); w++) {
			e = adj[w];
			if(diedge_isvalid(&e)) {
				DIEDGE_STRING(&e, se);
				printf("%s ", se);
			}
		}
		printf("\n");
	}
}

void 
adjmatewdg_clear(struct adjmatrix_ewdigraph *g)
{
	unsigned int v;

	for(v = 0; v < g->vertices; v++)
		ALGFREE(g->adjmat[v]);
	ALGFREE(g->adjmat);

	g->edges = 0;
	g->vertices = 0;
}
