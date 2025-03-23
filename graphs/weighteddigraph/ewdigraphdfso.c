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
#include "ewdigraphdfso.h"
#include "singlelist.h"

static int vequal(const void *, const void *);
static void dfso(struct ewdigraph_dfso *, unsigned int,
	const struct ewdigraph *);

/* Determines a depth-first order for the ewdigraph */
void 
ewdigraph_dfso_init(struct ewdigraph_dfso *dfs, const struct ewdigraph *g)
{
	unsigned int v;

	dfs->marked = (bool *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(bool));
	dfs->pre = (unsigned int *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(int));
	dfs->post = (unsigned int *)algcalloc(EWDIGRAPH_VERTICES(g), sizeof(int));

	dfs->preorder = (struct single_list *)
		algmalloc(sizeof(struct single_list));
	dfs->postorder = (struct single_list *)
		algmalloc(sizeof(struct single_list));
	
	dfs->precounter = 0;
	dfs->postcounter = 0;
	dfs->vertices = EWDIGRAPH_VERTICES(g);

	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++) {
		dfs->marked[v] = false;
		dfs->pre[v] = 0;
		dfs->post[v] = 0;
	}

	slist_init(dfs->preorder, sizeof(int), vequal);
	slist_init(dfs->postorder, sizeof(int), vequal);

	for(v = 0; v < EWDIGRAPH_VERTICES(g); v++)
		if(!dfs->marked[v])
			dfso(dfs, v, g);
}

/******************** static function boundary ********************/

static int 
vequal(const void *k1, const void *k2)
{
	int *v = (int *)k1, *w = (int *)k2;
	return *v == *w;
}

/* 
 * Run DFS in ewdigraph G from vertex v 
 * and compute preorder/postorder.
 */
static void 
dfso(struct ewdigraph_dfso *dfs, unsigned int v, const struct ewdigraph *g)
{
	struct single_list *adjlist;
	struct slist_node *nptr;
	struct diedge *e;
	unsigned int w;

	dfs->marked[v] = true;
	slist_append(dfs->preorder, &v);
	dfs->pre[v] = dfs->precounter++;

	adjlist = EWDIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adjlist, nptr, struct diedge, e) {
		w = DIEDGE_TO(e);
		if(!dfs->marked[w])
			dfso(dfs, w, g);
	}

	slist_append(dfs->postorder, &v);
	dfs->post[v] = dfs->postcounter++;
}
