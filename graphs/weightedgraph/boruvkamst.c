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
#include "boruvkamst.h"
#include "unionfind/unionfind.h"
#include "singlelist.h"
#include <math.h>	/* isless() */

static inline int less(const void *, const void *);

/* 
 * Compute a minimum spanning tree (or forest) 
 * of an edge-weighted graph.
 */
float
boruvka_mst_get(const struct ewgraph *g, struct single_list *mst)
{
	struct edge **closest, *e;
	struct union_find uf;
	struct single_list edgeset;
	struct slist_node *nptr;
	unsigned int i, j, k, v, w;
	float wt = 0.0;

	slist_init(mst, 0, NULL);

	uf_init(&uf, EWGRAPH_VERTICES(g));
	closest = (struct edge **)
		algcalloc(EWGRAPH_VERTICES(g), sizeof(struct edge *));
	for (k = 0; k < EWGRAPH_VERTICES(g); k++)
		closest[k] = NULL;

	/* Repeat at most log V times or until we have V-1 edges. */
	for (k = 1; k < EWGRAPH_VERTICES(g) &&
		SLIST_LENGTH(mst) < EWGRAPH_VERTICES(g) - 1;
		k += k) {

		/* 
		 * Foreach tree in forest, find closest edge if edge weights are equal,
		 * ties are broken in favor of first edge in edgeset.
		 */
		ewgraph_edges_get(g, &edgeset);
		SLIST_FOREACH(&edgeset, nptr, struct edge, e) {
			v = EDGE_EITHER(e), w = edge_other(e, v);
			i = uf_find(&uf, v), j = uf_find(&uf, w);
			if (i == j)	/* same tree */
				continue;
			if (closest[i] == NULL || less(e, closest[i]))
				closest[i] = e;
			if (closest[j] == NULL || less(e, closest[j]))
				closest[j] = e;
		}

		/* add newly discovered edges to MST */
		for (i = 0; i < EWGRAPH_VERTICES(g); i++) 
			if ((e = closest[i]) != NULL) {
				v = EDGE_EITHER(e);
				w = edge_other(e, v);
				/* don't add the same edge twice */
				if (!uf_connected(&uf, v, w)) {
					uf_union(&uf, v, w);
					slist_append(mst, e);
					wt += EDGE_WEIGHT(e);
				}
			}

		slist_clear(&edgeset);
	}

	ALGFREE(closest);
	UF_CLEAR(&uf);

	return wt;
}

/******************** static function boundary ********************/

static inline int 
less(const void *k1, const void *k2)
{
	struct edge *e1, *e2;

	e1 = (struct edge *)k1;
	e2 = (struct edge *)k2;
	return isless(e1->weight, e2->weight) ? 1 : 0;
}
