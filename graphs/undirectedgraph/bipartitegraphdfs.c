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
#include "bipartitegraph/bipartitegraphdfs.h"
#include "linearlist.h"

static void dfs(struct bipartite_graph_dfs *, const struct graph *,
	unsigned int);

/* 
 * Determines whether an undirected graph is bipartite 
 * and finds either a bipartition or an odd-length cycle.
 */
void
bigraphdfs_get(struct bipartite_graph_dfs *bg, const struct graph *g)
{
	unsigned int v;
	
	bg->isbipartite = true;
	bg->vertices = GRAPH_VERTICES(g);
	STACK_INIT(&bg->cycle, sizeof(long));
	
	bg->marked = (bool *)algmalloc(GRAPH_VERTICES(g) * sizeof(bool));
	bg->color = (bool *)algmalloc(GRAPH_VERTICES(g) * sizeof(bool));
	bg->edgeto = (long *)algmalloc(GRAPH_VERTICES(g) * sizeof(long));
	
	for(v = 0; v < GRAPH_VERTICES(g); v++) {
		bg->marked[v] = false;
		bg->color[v] = false;
		bg->edgeto[v] = -1;
	}
	
	for(v = 0; v < GRAPH_VERTICES(g); v++)
		if(!bg->marked[v])
			dfs(bg, g, v);
}

/* 
 * Returns the side of the bipartite that 
 * vertex v is on. 
 */
bool bigraphdfs_color(struct bipartite_graph_dfs *bg, 
					unsigned int v)
{
	if(v >= bg->vertices) {
		errmsg_exit("vertex %u is not between 0 "
			"and %u.\n", v, bg->vertices - 1);
	}
	
	if(!bg->isbipartite)
		errmsg_exit("graph is not bipartite.\n");
	
	return bg->color[v];
}

/******************** static function boundary ********************/

static void 
dfs(struct bipartite_graph_dfs *bg, const struct graph *g, unsigned int v)
{
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *w;
	long x;
	
	bg->marked[v] = true;
	
	slist = GRAPH_ADJLIST(g, v);
	SLIST_FOREACH(slist, nptr, unsigned int, w) {
		/* short circuit if odd-length cycle found */
		if(!STACK_ISEMPTY(&bg->cycle))
			return;
		
		/* found uncolored vertex, so recur */
		if(!bg->marked[*w]) {
			bg->edgeto[*w] = v;
			bg->color[*w] = !bg->color[v];
			dfs(bg, g, *w);
		}
		/* if v-w create an odd-length cycle, find it */
		else if(bg->color[v] == bg->color[*w]) {
			bg->isbipartite = false;
			stack_push(&bg->cycle, w);
			for(x = v; x != -1 && x != *w;	x = bg->edgeto[x])
				stack_push(&bg->cycle, &x);
			stack_push(&bg->cycle, w);
		}
	}
}
