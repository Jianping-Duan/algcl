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
#include "gabowscc.h"
#include "linearlist.h"

static void dfs(struct gabow_scc *, const struct digraph *, unsigned int,
	struct stack *, struct stack *);

/* Computes the strong components of the digraph g */
void 
gbscc_init(struct gabow_scc *scc, const struct digraph *g)
{
	unsigned int v;
	struct stack st1, st2;

	scc->precounter = 0;
	scc->count = 0;
	scc->vertices = DIGRAPH_VERTICES(g);

	scc->marked = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
	scc->id = (long *)algcalloc(DIGRAPH_VERTICES(g), sizeof(long));
	scc->preorder = (unsigned int *)
		algcalloc(DIGRAPH_VERTICES(g), sizeof(int));

	for (v = 0; v < DIGRAPH_VERTICES(g); v++) {
		scc->marked[v] = false;
		scc->id[v] = -1;
		scc->preorder[v] = 0;
	}

	STACK_INIT(&st1, sizeof(int));
	STACK_INIT(&st2, sizeof(int));

	for (v = 0; v < DIGRAPH_VERTICES(g); v++)
		if (!scc->marked[v])
			dfs(scc, g, v, &st1, &st2);

	stack_clear(&st1);
	stack_clear(&st2);
}

/******************** static function boundary ********************/

static void
dfs(struct gabow_scc *scc, const struct digraph *g, unsigned int v,
	struct stack *st1, struct stack *st2)
{
	unsigned int *w, *x;
	struct single_list *adj;
	struct slist_node *nptr;

	scc->marked[v] = true;
	scc->preorder[v] = scc->precounter++;
	stack_push(st1, &v);
	stack_push(st2, &v);

	adj = DIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, unsigned int, w) {
		if (!scc->marked[*w])
			dfs(scc, g, *w, st1, st2);
		else if (scc->id[*w] == -1) {
			x = (unsigned int *)stack_peek(st2);
			while (scc->preorder[*x] > scc->preorder[*w] &&
				  !STACK_ISEMPTY(st2)) {
				stack_pop(st2, (void **)&x);
				x = (unsigned int *)stack_peek(st2);
			}
		}
	}

	/* found strong component containing v */
	w = (unsigned int *)algmalloc(sizeof(int));
	x = (unsigned int *)stack_peek(st2);
	if (*x == v) {
		stack_pop(st2, (void **)&x);
		do {
			stack_pop(st1, (void **)&w);
			scc->id[*w] = scc->count;
		} while (*w != v && !STACK_ISEMPTY(st1));
		scc->count++;
	}
	ALGFREE(w);
}
