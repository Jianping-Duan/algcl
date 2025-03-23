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
#include "dijkstrapairsp.h"
#include "heap.h"
#include "singlelist.h"

static void validate(const struct dijkstra_pair_sp *, unsigned int);

/* 
 * Computes a shortest paths tree from each vertex to 
 * every other vertex in the edge-weighted digraph g.
 */
void
djkr_pairsp_init(struct dijkstra_pair_sp *psp, const struct ewdigraph *g)
{
	unsigned int v;

	psp->vertices = EWDIGRAPH_VERTICES(g);

	psp->allsp = (struct dijkstra_sp **)
		algmalloc(EWDIGRAPH_VERTICES(g) * sizeof(struct dijkstrasp *));
	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		psp->allsp[v] = (struct dijkstra_sp *)
			algmalloc(sizeof(struct dijkstra_sp));
		dijkstrasp_init(psp->allsp[v], g, v);
	}
}

/* Returns a shortest path from vertex s to vertex t. */
struct single_list * 
djkr_pairsp_path(const struct dijkstra_pair_sp *psp, unsigned int s,
				unsigned int t)
{
	struct single_list *path;

	validate(psp, s);
	validate(psp, t);

	path = (struct single_list *)algmalloc(sizeof(struct single_list));
	dijkstrasp_pathto(psp->allsp[s], t, path);

	return path;
}

/* 
 * Returns the length of a shortest path from vertex s to vertex t.
 */
float 
djkr_pairsp_dist(const struct dijkstra_pair_sp *psp, unsigned int s,
				unsigned int t)
{
	validate(psp, s);
	validate(psp, t);

	return DIJKSTRASP_DISTTO(psp->allsp[s], t);
}

void 
djkr_pairsp_clear(struct dijkstra_pair_sp *psp)
{
	unsigned int v;

	for(v = 0; v < psp->vertices; v++) {
		DIJKSTRASP_CLEAR(psp->allsp[v]);
		ALGFREE(psp->allsp[v]);
	}
	ALGFREE(psp->allsp);

	psp->vertices = 0;
}

/******************** static function boundary ********************/

static void 
validate(const struct dijkstra_pair_sp *psp, unsigned int v)
{
	if(v >= psp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			psp->vertices - 1);
	}
}
