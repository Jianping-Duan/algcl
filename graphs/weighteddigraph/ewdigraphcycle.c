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
#include "ewdigraphcycle.h"
#include "linearlist.h"

static void dfs(struct ewdigraph_cycle *, const struct ewdigraph *,
	unsigned int);

void 
ewdigraph_cycle_init(struct ewdigraph_cycle *ewdc, const struct ewdigraph *g)
{
	unsigned int v;

	ewdc->marked = (bool *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(bool));
	ewdc->edgeto = (struct diedge **)
		algcalloc(EWDIGRAPH_VERTICES(g), sizeof(struct diedge *));
	ewdc->onstack = (bool *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(bool));
	
	ewdc->cycle = (struct stack *)algmalloc(sizeof(struct stack));
	STACK_INIT(ewdc->cycle, 0);

	for (v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		ewdc->marked[v] = false;
		ewdc->edgeto[v] = NULL;
		ewdc->onstack[v] = false;
	}

	for (v = 0; v < EWDIGRAPH_VERTICES(g); v++)
		if (!ewdc->marked[v])
			dfs(ewdc, g, v);
}

/******************** static function boundary ********************/

static void
dfs(struct ewdigraph_cycle *ewdc, const struct ewdigraph *g, unsigned int v)
{
	struct single_list *adj;
	struct slist_node *nptr;
	struct diedge *e, *f;
	unsigned int w;

	ewdc->marked[v] = true;
	ewdc->onstack[v] = true;

	adj = EWDIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, struct diedge, e) {
		w = DIEDGE_TO(e);
	
		/* short circuit if directed cycle found */
		if (!STACK_ISEMPTY(ewdc->cycle))
			return;
		/* found new vertex, so recur */
		else if (!ewdc->marked[w]) {
			ewdc->edgeto[w] = e;
			dfs(ewdc, g, w);
		}
		/* trace back directed cycle */
		else if (ewdc->onstack[w]) {
			f = e;
			while (DIEDGE_FROM(f) != w) {
				stack_push(ewdc->cycle, f);
				f = ewdc->edgeto[DIEDGE_FROM(f)];
			}
			stack_push(ewdc->cycle, f);
			return;
		}
	}

	ewdc->onstack[v] = false;
}
