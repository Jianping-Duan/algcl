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
#include "tarjanscc.h"
#include "linearlist.h"

static void dfs(struct tarjan_scc *, const struct digraph *, unsigned int,
	struct stack *);

/* Computes the strong components of the digraph g */
void 
tjscc_init(struct tarjan_scc *scc, const struct digraph *g)
{
	unsigned int v;
	struct stack st;

	scc->precounter = 0;
	scc->count = 0;
	scc->vertices = DIGRAPH_VERTICES(g);
	scc->marked = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
	scc->id = (unsigned int *)algcalloc(DIGRAPH_VERTICES(g), sizeof(int));
	scc->low = (unsigned int *)algcalloc(DIGRAPH_VERTICES(g), sizeof(int));

	for(v = 0; v < DIGRAPH_VERTICES(g); v++) {
		scc->marked[v] = false;
		scc->id[v] = 0;
		scc->low[v] = 0;
	}

	STACK_INIT(&st, sizeof(int));
	for(v = 0; v < DIGRAPH_VERTICES(g); v++)
		if(!scc->marked[v])
			dfs(scc, g, v, &st);
	stack_clear(&st);
}

/******************** static function boundary ********************/

static void 
dfs(struct tarjan_scc *scc, const struct digraph *g, unsigned int v, 
	struct stack *st)
{
	unsigned int min, *w, *x;
	struct single_list *adj;
	struct slist_node *nptr;

	scc->marked[v] = true;
	scc->low[v] = scc->precounter++;
	min = scc->low[v];
	stack_push(st, &v);

	adj = DIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, unsigned int, w) {
		if(!scc->marked[*w])
			dfs(scc, g, *w, st);

		if(scc->low[*w] < min)
			min = scc->low[*w];
	}

	if(min < scc->low[v]) {
		scc->low[v] = min;
		return;
	}

	x = (unsigned int *)algmalloc(sizeof(int));
	do {
		stack_pop(st, (void **)&x);
		scc->id[*x] = scc->count;
		scc->low[*x] = DIGRAPH_VERTICES(g);
	} while(v != *x && !STACK_ISEMPTY(st));
	scc->count++;
	ALGFREE(x);
}
