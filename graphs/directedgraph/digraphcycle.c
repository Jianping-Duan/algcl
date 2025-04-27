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
#include "digraphcycle.h"
#include "linearlist.h"

static void dfs(struct digraph_cycle *, const struct digraph *, unsigned int);

/* 
 * Determines whether the digraph G has a 
 * directed cycle and, if so, finds such a cycle.
 */
void 
digraph_cycle_init(struct digraph_cycle *gc, const struct digraph *g)
{
	unsigned int v;
	
	gc->marked = (bool *)algmalloc(g->vertices * sizeof(bool));
	gc->edgeto = (long *)algmalloc(g->vertices * sizeof(long));
	gc->onstack = (bool *)algmalloc(g->vertices * sizeof(bool));

	gc->cycle = (struct stack *)algmalloc(sizeof(struct stack));
	STACK_INIT(gc->cycle, sizeof(int));
	
	for (v = 0; v < DIGRAPH_VERTICES(g); v++) {
		gc->marked[v] = false;
		gc->edgeto[v] = -1;
		gc->onstack[v] = false;
	}

	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		if (!gc->marked[v] && STACK_ISEMPTY(gc->cycle))
			dfs(gc, g, v);
}

/******************** static function boundary ********************/

/*  run DFS and find a directed cycle (if one exists) */
static void 
dfs(struct digraph_cycle *gc, const struct digraph *g, unsigned int v)
{
	struct single_list *slist;
	struct slist_node *nptr;
	unsigned int *w;
	long x;
	
	gc->onstack[v] = true;
	gc->marked[v] = true;
	slist = DIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(slist, nptr, unsigned int, w) {
		/* short circuit if cycle already found */
		if (!STACK_ISEMPTY(gc->cycle))
			return;
			
		if (!gc->marked[*w]) {
			gc->edgeto[*w] = v;
			dfs(gc, g, *w);
		}
		/* trace back digraph cycle */
		else if (gc->onstack[*w]) {
			for (x = v; x != -1 && x != *w; x = gc->edgeto[x])
				stack_push(gc->cycle, &x);
			stack_push(gc->cycle, w);
			stack_push(gc->cycle, &v);
		}
	}
	gc->onstack[v] = false;
}
