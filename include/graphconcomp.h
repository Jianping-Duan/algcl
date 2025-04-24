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
#ifndef _GRAPHCONCOMP_H_
#define _GRAPHCONCOMP_H_

#include "graph.h"

/* graph connected components */
struct graph_cc {
	bool *marked;		/* marked[v] = has vertex v been marked? */
	unsigned int *ids;	/* id[v] = id of connected component containing v */
	unsigned int *sizes;	/* size[id] = number of vertices in given
							   component */
	unsigned int count;		/* number of connected components */
	unsigned int vertices;	/* graph of vertices */
};

/* Returns the component id of the connected component containing vertex v. */
#define GRAPHCC_ID(gc, v)	\
	((v) >= (gc)->vertices ? 0 : (gc)->ids[v])

/* 
 * Returns the number of vertices in the connected
 * component containing vertex v.
 */
#define GRAPH_SIZE(gc, v)	\
	((v) >= (gc)->vertices ? 0 : (gc)->sizes[v])

/* Returns the number of connected components in the graph g. */
#define GRAPHCC_COUNT(gc)	((gc)->count)

/* Returns true if vertices v and w are in the same connected component. */
#define GRAPHCC_CONNECTED(gc, v, w)		\
	((v) >= (gc)->vertices ? 0 : (w) >= (gc)->vertices	\
		? 0 : (gc)->ids[v] == (gc)->ids[w])

/* Clears this connected components. */
#define GRAPHCC_CLEAR(gc)	do {	\
	ALGFREE((gc)->marked);			\
	ALGFREE((gc)->ids);				\
	ALGFREE((gc)->sizes);			\
	(gc)->count = 0;				\
	(gc)->vertices = 0;				\
} while (0)

/* Computes the connected components of the undirected graph g. */
void graphcc_init(struct graph_cc *gc, const struct graph *g);

#endif	/* _GRAPHCONCOMP_H_ */
