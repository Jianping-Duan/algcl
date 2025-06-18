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
#ifndef _BIPARTITEGRAPHDFS_H_
#define _BIPARTITEGRAPHDFS_H_

#include "graph.h"
#include "stack.h"

struct bipartite_graph_dfs {
	bool isbipartite;	/* is the graph bipartite? */
	bool *color;	/* color[v] gives vertices on one side of bipartition */
	bool *marked;	/* marked[v] = true iff v has been visited in DFS */
	long *edgeto;	/* edgeTo[v] = last edge on path to v */
	struct stack cycle;	/* odd-length cycle */
	unsigned int vertices;	/* graph of vertices */
};

/* Returns true if the graph is bipartite. */
#define BIGRAPHDFS_ISBIPARTITE(bg)	((bg)->isbipartite)

/* Returns an odd-length cycle if the graph is not bipartite. */
#define BIGRAPHDFS_ODDCYCLE(bg)		((bg)->cycle)

#define BIGRAPHDFS_CLEAR(bg)	do {	\
	ALGFREE((bg)->color);		\
	ALGFREE((bg)->marked);		\
	ALGFREE((bg)->edgeto);		\
	(bg)->vertices = 0;		\
	(bg)->isbipartite = false;	\
} while (0)

/* 
 * Determines whether an undirected graph is bipartite 
 * and finds either a bipartition or an odd-length cycle.
 */
void bigraphdfs_get(struct bipartite_graph_dfs *bg, const struct graph *g);

/* eturns the side of the bipartite that vertex v is on. */
bool bigraphdfs_color(struct bipartite_graph_dfs *bg, unsigned int v);

#endif	/* _BIPARTITEGRAPHDFS_H_ */
