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
#include "graphconcomp.h"
#include "singlelist.h"

/* depth-first search */
static void dfs(struct graph_cc *, unsigned int, const struct graph *);

/* 
 * Computes the connected components of the undirected graph g.
 */
void 
graphcc_init(struct graph_cc *gc, const struct graph *g)
{
	unsigned int i, v;
	
	gc->vertices = g->vertices;
	gc->count = 0;
	
	gc->marked = (bool *)algmalloc(g->vertices * sizeof(bool));
	gc->ids = (unsigned int *)algmalloc(g->vertices * sizeof(int));
	gc->sizes = (unsigned int *)algmalloc(g->vertices * sizeof(int));
	
	for (i = 0; i < g->vertices; i++) {
		gc->marked[i] = false;
		gc->ids[i] = 0;
		gc->sizes[i] = 0;
	}
	
	for (v = 0; v < g->vertices; v++)
		if (!gc->marked[v]) {
			dfs(gc, v, g);
			gc->count++;
		}
}

/******************** static function boundary ********************/

/* depth-first search */
static void 
dfs(struct graph_cc *gc, unsigned int v, const struct graph *g)
{
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *w;
	
	gc->marked[v] = true;
	gc->ids[v] = gc->count;
	gc->sizes[gc->count]++;
	
	slist = GRAPH_ADJLIST(g, v);
	SLIST_FOREACH(slist, nptr, unsigned int, w) {
		if (!gc->marked[*w])
			dfs(gc, *w, g);
	}
}
