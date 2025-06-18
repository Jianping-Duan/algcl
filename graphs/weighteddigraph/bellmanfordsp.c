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
#include "bellmanfordsp.h"
#include "ewdigraphcycle.h"
#include "linearlist.h"

static void find_negative_cycle(const struct bellmanford_sp *);
static void relax(struct bellmanford_sp *, const struct ewdigraph *,
	unsigned int);

/* 
 * Computes a shortest paths tree from s to every other vertex in
 * the edge-weighted digraph g.
 */
void 
bmfsp_init(struct bellmanford_sp *sp, const struct ewdigraph *g,
	unsigned int s)
{
	unsigned int v, *w;

	if (s >= EWDIGRAPH_VERTICES(g)) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", s,
			EWDIGRAPH_VERTICES(g) - 1);
	}

	sp->vertices = EWDIGRAPH_VERTICES(g);
	sp->distto = (float *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(float));
	sp->edgeto = (struct diedge **)
		algcalloc(EWDIGRAPH_VERTICES(g), sizeof(struct diedge *));
	sp->onqueue = (bool *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(bool));

	for (v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		sp->distto[v] = INFINITY;
		sp->edgeto[v] = NULL;
		sp->onqueue[v] = false;
	}
	sp->distto[s] = 0.0;

	sp->quvr = (struct queue *)algmalloc(sizeof(struct queue));
	QUEUE_INIT(sp->quvr, sizeof(int));

	sp->cycle = (struct stack *)algmalloc(sizeof(struct stack));
	STACK_INIT(sp->cycle, sizeof(struct diedge));

	/* Bellman-Ford algorithm */
	w = (unsigned int *)algmalloc(sizeof(int));
	enqueue(sp->quvr, &s);
	sp->onqueue[s] = true;
	while (!QUEUE_ISEMPTY(sp->quvr) && !BMFSP_HAS_NEGATIVE_CYCLE(sp)) {
		dequeue(sp->quvr, (void **)&w);
		sp->onqueue[*w] = false;
		relax(sp, g, *w);
	}
	ALGFREE(w);
}

/* Gets a shortest path from the source s to vertex v. */
void 
bmfsp_paths_get(const struct bellmanford_sp *sp, unsigned int v,
		struct single_list *paths)
{
	struct diedge *e;

	if (v >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			sp->vertices - 1);
	}
	if (BMFSP_HAS_NEGATIVE_CYCLE(sp))
		errmsg_exit("Negative cost cycle exists.\n");

	slist_init(paths, 0, NULL);
	for (e = sp->edgeto[v]; e != NULL; e = sp->edgeto[DIEDGE_FROM(e)])
		slist_put(paths, e);
}

/******************** static function boundary ********************/

/* by finding a cycle in predecessor graph */
static void 
find_negative_cycle(const struct bellmanford_sp *sp)
{
	struct ewdigraph g;
	struct ewdigraph_cycle dc;
	struct diedge *e, *f, *h;
	struct stack *st;
	unsigned int v;

	ewdigraph_init(&g, sp->vertices);
	for (v = 0; v < sp->vertices; v++)
		if (sp->edgeto[v] != NULL) {
			e = sp->edgeto[v];
			f = make_diedge(e->v, e->w, e->weight);
			ewdigraph_add_edge(&g, f);
		}

	ewdigraph_cycle_init(&dc, &g);
	if (EWDIGRAPH_HAS_CYCLE(&dc)) {
		st = EWDIGRAPH_CYCLE_GET(&dc);
		h = (struct diedge *)algmalloc(sizeof(struct diedge));
		while (!STACK_ISEMPTY(st)) {
			stack_pop(st, (void **)&h);
			stack_push(sp->cycle, h);
		}
		ALGFREE(h);
	}

	EWDIGRAPH_CYCLE_CLEAR(&dc);
	ewdigraph_clear(&g);
}

/* 
 * Relax vertex v and put other endpoints on queue 
 * if changed.
 */
static void 
relax(struct bellmanford_sp *sp, const struct ewdigraph *g,
	unsigned int v)
{
	unsigned int w;
	struct single_list *adj;
	struct slist_node *nptr;
	struct diedge *e;

	adj = EWDIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, struct diedge, e) {
		w = DIEDGE_TO(e);
		if (sp->distto[w] > sp->distto[v] + DIEDGE_WEIGHT(e)) {
			sp->distto[w] = sp->distto[v] + DIEDGE_WEIGHT(e);
			sp->edgeto[w] = e;
			
			if (!sp->onqueue[w]) {
				enqueue(sp->quvr, &w);
				sp->onqueue[w] = true;
			}
		}

		if (++(sp->cost) % EWDIGRAPH_VERTICES(g) == 0) {
			find_negative_cycle(sp);
			if (BMFSP_HAS_NEGATIVE_CYCLE(sp))
				return;
		}
	}
}
