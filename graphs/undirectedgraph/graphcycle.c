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
#include "graphcycle.h"
#include "linearlist.h"

static void dfs(struct graph_cycle *, const struct graph *, long,
	unsigned int);

/* Initialize an empty cycle. */
void 
graph_cycle_init(struct graph_cycle *gc, const struct graph *g)
{
	unsigned int i;
	
	gc->marked = (bool *)algmalloc(g->vertices * sizeof(bool));
	gc->edgeto = (long *)algmalloc(g->vertices * sizeof(long));
	
	STACK_INIT(&gc->cycle, sizeof(int));
	
	for(i = 0; i < GRAPH_VERTICES(g); i++) {
		gc->marked[i] = false;
		gc->edgeto[i] = -1;
	}
}

/* 
 * Determines whether the undirected graph g 
 * has a cycle and, if so, finds such a cycle. 
 */
void 
graph_cycle_get(struct graph_cycle *gc, const struct graph *g)
{
	unsigned int v;
	
	if(graph_parallel_edges(gc, g))
		return;
	
	if(!STACK_ISEMPTY(&gc->cycle))
		stack_clear(&gc->cycle);
	
	for(v = 0; v < GRAPH_VERTICES(g); v++)
		if(!gc->marked[v])
			dfs(gc, g, -1, v);
}

/* 
 * Does this graph have a self loop? side effect: 
 * initialize cycle to be self loop.
 */
bool 
graph_self_loop(struct graph_cycle *gc, const struct graph *g)
{
	unsigned int v, *w;
	struct single_list *slist;
	struct slist_node *nptr;
	
	if(!STACK_ISEMPTY(&gc->cycle))
		stack_clear(&gc->cycle);
	
	for(v = 0; v < GRAPH_VERTICES(g); v++) {
		slist = GRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			if(v == *w) {
				stack_push(&gc->cycle, &v);
				stack_push(&gc->cycle, w);
				return true;
			}
		}
	}
	return false;
}

/* 
 * Does this graph have two parallel edges? side effect:
 * initialize cycle to be two parallel edges.
 */
bool 
graph_parallel_edges(struct graph_cycle *gc, const struct graph *g)
{
	unsigned int v, *w;
	struct single_list *slist;
	struct slist_node *nptr;
	
	if(!STACK_ISEMPTY(&gc->cycle))
		stack_clear(&gc->cycle);
	
	for(v = 0; v < GRAPH_VERTICES(g); v++) {
		slist = GRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			if(gc->marked[*w]) {
				stack_push(&gc->cycle, &v);
				stack_push(&gc->cycle, w);
				stack_push(&gc->cycle, &v);
				return true;
			}
			gc->marked[*w] = true;
		}
		
		/* reset so marked[v] = false for all v */
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			gc->marked[*w] = false;
		}
	}
	return false;
}

/******************** static function boundary ********************/

static void 
dfs(struct graph_cycle *gc, const struct graph *g, long u, unsigned int v)
{
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *w;
	long x;
	
	gc->marked[v] = true;
	slist = GRAPH_ADJLIST(g, v);
	SLIST_FOREACH(slist, nptr, unsigned int, w) {
		/* short circuit if cycle already found */
		if(!STACK_ISEMPTY(&gc->cycle))
			return;
			
		if(!gc->marked[*w]) {
			gc->edgeto[*w] = v;
			dfs(gc, g, v, *w);
		}
		/* 
		 * check for cycle (but disregard reverse of 
		 * edge leading to v) 
		 */
		else if(*w != u) {
			for(x = v; x != -1 && x != *w;
				x = gc->edgeto[x]) {
				stack_push(&gc->cycle, &x);
			}
			stack_push(&gc->cycle, w);
			stack_push(&gc->cycle, &v);
		}
	}
}
