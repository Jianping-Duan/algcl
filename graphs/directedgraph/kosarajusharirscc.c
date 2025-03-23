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
#include "kosarajusharirscc.h"
#include "digraphdfso.h"
#include "singlelist.h"

static void dfs(struct kosaraju_sharir_scc *, const struct digraph *,
	unsigned int);

/* Computes the strong components of the digraph g */
void 
kjsscc_init(struct kosaraju_sharir_scc *scc, const struct digraph *g)
{
	unsigned int v, *w;
	struct digraph dgr;
	struct digraph_dfso dfso;
	struct single_list order;
	struct slist_node *nptr;

	scc->count = 0;
	scc->vertices = DIGRAPH_VERTICES(g);
	scc->marked = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
	scc->id = (unsigned int *)algcalloc(DIGRAPH_VERTICES(g), sizeof(int));

	for(v = 0; v < DIGRAPH_VERTICES(g); v++) {
		scc->marked[v] = false;
		scc->id[v] = 0;
	}

	/* reverse of the digraph */
	digraph_reverse(g, &dgr);

	/* compute reverse postorder of reverse digraph */
	digraph_dfso_init(&dfso, &dgr);
	DIGRAPH_DFSO_REVERSEPOST(&dfso, &order);

	/* 
	 * Run dfs on g, 
	 * using reverse postorder to guide calculation.
	 */
	SLIST_FOREACH(&order, nptr, unsigned int, w) {
		if(!scc->marked[*w]) {
			dfs(scc, g, *w);
			scc->count++;
		}
	}

	digraph_clear(&dgr);
	slist_clear(&order);
	DIGRAPH_DFSO_CLEAR(&dfso);
}

/******************** static function boundary ********************/

static void 
dfs(struct kosaraju_sharir_scc *scc, const struct digraph *g, unsigned int v)
{
	struct single_list *adj;
	struct slist_node *nptr;
	unsigned int *w;

	scc->marked[v] = true;
	scc->id[v] = scc->count;

	adj = DIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adj, nptr, unsigned int, w) {
		if(!scc->marked[*w])
			dfs(scc, g, *w);
	}
}
