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
#ifndef _DIGRAPHDFS_H_
#define _DIGRAPHDFS_H_

#include "digraph.h"

/* Digraph depth-first search. */
struct digraph_dfs {
	bool *marked;	/* marked[v] = true iff v is reachable from source(s) */
	unsigned int count;		/* number of vertices reachable from source(s) */
	unsigned int vertices;	/* digraph of vertices */
};

/* 
 * Returns the number of vertices reachable from 
 * the source vertex (or source vertices). 
 */
#define DIGRAPH_DFS_COUNT(dfs)		((dfs)->count)

/* 
 * Is there a directed path from the source vertex 
 * (or any of the source vertices) and vertex v ? 
 */
#define DIGRAPH_DFS_MARKED(dfs, v)	\
   ((v) >= (dfs)->vertices ? false : (dfs)->marked[v])

#define DIGRAPH_DFS_CLEAR(dfs)		do {	\
	ALGFREE((dfs)->marked);					\
	(dfs)->count = 0;						\
	(dfs)->vertices = 0;					\
} while (0)

struct single_list;

/* 
 * Computes the vertices in digraph g that are reachable from the source
 * vertex s.
 */
void digraph_dfs_init(struct digraph_dfs *ddfs, const struct digraph *g,
					unsigned int s);

/* 
 * Computes the vertices in digraph G that are connected to any of the source
 * vertices svset. 
 */
void digraph_dfs_svset(struct digraph_dfs *ddfs, const struct digraph *g,
					const struct single_list *svset);

#endif	/*	_DIGRAPHDFS_H_	*/
