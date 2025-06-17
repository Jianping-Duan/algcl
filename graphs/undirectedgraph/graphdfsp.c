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
#include "graphdfsp.h"
#include "linearlist.h"

/* depth first search from v */
static void gdfs(struct graph_dfsp *, unsigned int, const struct graph *);

/* 
 * Computes a path between s and every other vertex in graph g. 
 */
void 
graph_dfsp_init(struct graph_dfsp *dfs, unsigned int s, const struct graph *g)
{
	unsigned int i;
	
	if (s > g->vertices)
		errmsg_exit("vertex %u is not between 0 and %u\n", s,
			g->vertices - 1);
	
	dfs->sv = s;
	dfs->vertices = g->vertices;
	
	dfs->marked = (bool *)algmalloc(g->vertices * sizeof(bool));
	dfs->edgeto = (long *)algmalloc(g->vertices * sizeof(long));
	
	for (i = 0; i < g->vertices; i++) {
		dfs->marked[i] = false;
		dfs->edgeto[i] = -1;
	}
	
	gdfs(dfs, s, g);
}

/* 
 * Returns a path between the source vertex sv 
 * and vertex v, or if no such path. 
 */
void 
graph_dfsp_paths(const struct graph_dfsp *dfs, unsigned int v,
		struct stack *paths)
{
	long x, s;
	
	if (v > dfs->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u\n", v,
			dfs->vertices - 1);
	}
	
	if (!GRAPH_DFSP_HASPATH(dfs, v)) {
		paths = NULL;
		return;
	}
	
	STACK_INIT(paths, sizeof(long));
	for (x = v; x != dfs->sv && x != -1; x = dfs->edgeto[x])
		stack_push(paths, &x);
	s = (long)dfs->sv;
	stack_push(paths, &s);
}

/******************** static function boundary ********************/

/* depth first search from v */
static void 
gdfs(struct graph_dfsp *dfs, unsigned int v, const struct graph *g)
{
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *w;
	
	dfs->marked[v] = true;
	
	slist = GRAPH_ADJLIST(g, v);
	SLIST_FOREACH(slist, nptr, unsigned int, w) {
		if (!dfs->marked[*w]) {
			dfs->edgeto[*w] = v;
			gdfs(dfs, *w, g);
		}
	}
}
