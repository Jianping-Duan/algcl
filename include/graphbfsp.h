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
#ifndef _GRAPHBFSP_H_
#define _GRAPHBFSP_H_

#include "graph.h"

/* graph breadth-first search paths */
struct graph_bfsp {
	bool *marked;	/* marked[v] = is there an s-v path */
	long *edgeto;	/* edgeTo[v] = previous edge on shortest s-v path */
	long *distto;	/* distTo[v] = number of edges shortest s-v path */
	unsigned int vertices;	/* graph of vertices */
};

/* Is there a path between the source vertex s and vertex v? */
#define GRAPH_BFSP_HASPATH(bfs, v)	\
	((v) >= (bfs)->vertices ? false : (bfs)->marked[v])

/* 
 * Returns the number of edges in a shortest path between the source vertex s
 * and vertex v? 
 */
#define GRAPH_BFSP_DISTTO(bfs, v)	\
	((v) >= (bfs)->vertices ? (-1) : (bfs)->distto[v])

/* Clears this breadth-first search paths. */
#define GRAPH_BFSP_CLEAR(bfs)	do {	\
	ALGFREE((bfs)->marked);				\
	ALGFREE((bfs)->edgeto);				\
	ALGFREE((bfs)->distto);				\
	(bfs)->vertices = 0;				\
} while (0)

struct stack;

/* 
 * Computes the shortest path between the source vertex s and every other
 * vertex in the graph g. 
 */
void graph_bfsp_init(struct graph_bfsp *bfs, unsigned int s,
					const struct graph *g);

/* 
 * Gets a shortest path between the source vertex s and v, 
 * or if no such path. 
 */
void graph_bfsp_paths(const struct graph_bfsp *bfs, unsigned int v,
					struct stack *paths);

#endif	/* _GRAPHBFSP_H_ */
