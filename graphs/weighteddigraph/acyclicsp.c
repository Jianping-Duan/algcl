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
#include "acyclicsp.h"
#include "topological/ewditopological.h"
#include "singlelist.h"

static void relax(struct acyclicsp *, const struct diedge *);

/* 
 * Computes a shortest paths tree from s to every 
 * other vertex in the directed acyclic graph g.
 */
void 
acycsp_init(struct acyclicsp *sp, const struct ewdigraph *g, unsigned int s)
{
	unsigned int v, *w;
	struct single_list vset, *adj;
	struct slist_node *nptr1, *nptr2;
	struct diedge *e;

	if(s >= EWDIGRAPH_VERTICES(g)) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", s, 
			EWDIGRAPH_VERTICES(g) - 1);
	}

	ewdtplg_use_queue(g, &vset);
	if(SLIST_LENGTH(&vset) == 0)
		errmsg_exit("This digraph is not acyclic.\n");

	sp->vertices = EWDIGRAPH_VERTICES(g);
	sp->distto = (float *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(float));
	sp->edgeto = (struct diedge **)
		algcalloc(EWDIGRAPH_VERTICES(g), sizeof(struct diedge *));
	
	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		sp->distto[v] = INFINITY;
		sp->edgeto[v] = NULL;
	}
	sp->distto[s] = 0.0;

	SLIST_FOREACH(&vset, nptr1, unsigned int, w) {
		adj = EWDIGRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(adj, nptr2, struct diedge, e) {
			relax(sp, e);
		}
	}
	slist_clear(&vset);
}

/* 
 * Gets a shortest path from the source 
 * vertex s to vertex v.
 */
void 
acycsp_paths_get(const struct acyclicsp *sp, unsigned int v,
				struct single_list *paths)
{
	struct diedge *e = NULL;

	slist_init(paths, 0, NULL);

	if(v >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			sp->vertices - 1);
	}
	
	for(e = sp->edgeto[v]; e != NULL;
		e = sp->edgeto[DIEDGE_FROM(e)]) {
		slist_put(paths, e);
	}
}

/******************** static function boundary ********************/

static void 
relax(struct acyclicsp *sp, const struct diedge *e)
{
	unsigned int v, w;

	v = DIEDGE_FROM(e), w = DIEDGE_TO(e);
	if(sp->distto[w] > sp->distto[v] + DIEDGE_WEIGHT(e)) {
		sp->distto[w] = sp->distto[v] + DIEDGE_WEIGHT(e);
		sp->edgeto[w] = (void *)e;
	}
}
