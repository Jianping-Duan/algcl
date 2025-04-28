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
#include "kruskalmst.h"
#include "unionfind/unionfind.h"
#include "sortalg.h"
#include "singlelist.h"
#include <math.h>	/* islessequal() */

static int lessequal(const void *, const void *);

/* 
 * Compute a minimum spanning tree (or forest) of an edge-weighted graph,
 * and returns the sum of the edge weights is this minimum spanning tree.
 */
float 
kruskal_mst_get(const struct ewgraph *g, struct single_list *mst)
{
	struct single_list edgeset;
	struct slist_node *nptr;
	struct edge *e, **edges;
	struct union_find uf;
	unsigned int cnt = 0, i;
	unsigned int v, w;
	float wt = 0.0;

	/* initialize an emtpy MST */
	slist_init(mst, 0, NULL);

	/* create array of edges, sorted by weight */
	edges = (struct edge **)algcalloc(EWGRAPH_EDGES(g), sizeof(struct edge *));
	for (i = 0; i < EWGRAPH_EDGES(g); i++)
		edges[i] = NULL;

	ewgraph_edges_get(g, &edgeset);
	SLIST_FOREACH(&edgeset, nptr, struct edge, e) {
		edges[cnt++] = e;	
	}
	/* Shell-sort better than Quick-sort */
	SHELL_SORT(edges, cnt, sizeof(struct edge *), lessequal);

	/* run greedy algorithm */
	uf_init(&uf, EWGRAPH_VERTICES(g));
	for (i = 0; i < cnt && 
		SLIST_LENGTH(mst) < EWGRAPH_VERTICES(g) - 1; i++) {
		e = edges[i];
		v = EDGE_EITHER(e);
		w = edge_other(e, v);
		
		/* v-w does not create a cycle */
		if (!uf_connected(&uf, v, w)) {
			uf_union(&uf, v, w);
			slist_append(mst, e);
			wt += EDGE_WEIGHT(e);
		}
	}

	ALGFREE(edges);

	return wt;
}

/******************** static function boundary ********************/

/* using sort ascending algorithm */
static int 
lessequal(const void *k1, const void *k2)
{
	struct edge **e1, **e2;

	e1 = (struct edge **)k1;
	e2 = (struct edge **)k2;
	return islessequal((*e1)->weight, (*e2)->weight) ? 1 : -1;
}
