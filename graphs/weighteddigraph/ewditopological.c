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
#include "topological/ewditopological.h"
#include "ewdigraphcycle.h"
#include "ewdigraphdfso.h"
#include "linearlist.h"

static int vequal(const void *, const void *);

void 
ewdtplg_use_dfso(const struct ewdigraph *g, struct single_list *order)
{
	struct ewdigraph_cycle dc;
	struct ewdigraph_dfso dfso;

	ewdigraph_cycle_init(&dc, g);
	if(EWDIGRAPH_HAS_CYCLE(&dc)) {
		slist_init(order, 0, NULL);
		return;
	}

	ewdigraph_dfso_init(&dfso, g);
	EWDIGRAPH_DFSO_REVERSEPOST(&dfso, order);

	EWDIGRAPH_CYCLE_CLEAR(&dc);
	EWDIGRAPH_DFSO_CLEAR(&dfso);
}

void 
ewdtplg_use_queue(const struct ewdigraph *g, struct single_list *order)
{
	unsigned int v, *w, x, cnt = 0;
	int *indegree;
	struct queue qu;
	struct single_list *adj;
	struct slist_node *nptr;
	struct diedge *e;

	/* indegrees of remaining vertices */
	indegree = (int *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(int));
	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++)
		indegree[v] = EWDIGRAPH_INDEGREE(g, v);
	
	/* 
	 * Initialize queue to contain all vertices
	 * with indegree = 0.
	 */
	QUEUE_INIT(&qu, sizeof(int));
	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++)
		if(indegree[v] == 0)
			enqueue(&qu, &v);

	slist_init(order, sizeof(int), vequal);
	w = (unsigned int *)algmalloc(sizeof(int));
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&w);
		slist_append(order, w);
		cnt++;
		adj = EWDIGRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(adj, nptr, struct diedge, e) {
			x = DIEDGE_TO(e);
			indegree[x]--;
			if(indegree[x] == 0)
				enqueue(&qu, &x);
		}
	}
	ALGFREE(w);
	ALGFREE(indegree);
	queue_clear(&qu);

	/* 
	 * There is a directed cycle in subgraph of
	 * vertices with indegree >= 1. 
	 */
	if(cnt != EWDIGRAPH_VERTICES(g))
		slist_clear(order);
}

/******************** static function boundary ********************/

static int 
vequal(const void *k1, const void *k2)
{
	int *v = (int *)k1, *w = (int *)k2;
	return *v == *w;
}
