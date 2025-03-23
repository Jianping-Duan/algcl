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
#include "digraphdfs.h"
#include "singlelist.h"

static void dfs(struct digraph_dfs *, const struct digraph *, unsigned int);

/* 
 * Computes the vertices in digraph g 
 * that are reachable from the source vertex s.
 */
void 
digraph_dfs_init(struct digraph_dfs *ddfs, const struct digraph *g,
				unsigned int s)
{
	unsigned int v;

	if(s >= DIGRAPH_VERTICES(g)) {
		errmsg_exit("Vertex %u is not between 0 and %u.\n", s,
			DIGRAPH_VERTICES(g) - 1);
	}
	
	ddfs->count = 0;
	ddfs->vertices = DIGRAPH_VERTICES(g);
	ddfs->marked = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
	for(v = 0; v < DIGRAPH_VERTICES(g); v++)
		ddfs->marked[v] = false;

	dfs(ddfs, g, s);
}

/* 
 * Computes the vertices in digraph G that are
 * connected to any of the source vertices svset.
 */
void 
digraph_dfs_svset(struct digraph_dfs *ddfs, const struct digraph *g,
				const struct single_list *svset)
{
	unsigned int i, *v;
	struct slist_node *nptr;

	if(svset == NULL)
		errmsg_exit("Argument source vertices set is null.\n");

	if(SLIST_LENGTH(svset) == 0)
		errmsg_exit("Zero vertices.\n");

	ddfs->count = 0;
	ddfs->vertices = DIGRAPH_VERTICES(g);
	ddfs->marked = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
	for(i = 0; i < DIGRAPH_VERTICES(g); i++)
		ddfs->marked[i] = false;

	SLIST_FOREACH(svset, nptr, unsigned int, v) {
		if(*v >= DIGRAPH_VERTICES(g)) {
			DIGRAPH_DFS_CLEAR(ddfs);
			errmsg_exit("Vertex %u is not between 0 and %u.\n", *v,
				DIGRAPH_VERTICES(g) - 1);
		}
		
		if(!ddfs->marked[*v])
			dfs(ddfs, g, *v);
	}
}

/******************** static function boundary ********************/

static void 
dfs(struct digraph_dfs *ddfs, const struct digraph *g, unsigned int v)
{
	struct single_list *adjlist;
	struct slist_node *nptr;
	unsigned int *w;

	ddfs->marked[v] = true;
	ddfs->count++;

	adjlist = DIGRAPH_ADJLIST(g, v);
	SLIST_FOREACH(adjlist, nptr, unsigned int, w) {
		if(!ddfs->marked[*w])
			dfs(ddfs, g, *w);
	}
}
