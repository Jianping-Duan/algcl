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
#ifndef _DIGRAPH_H_
#define _DIGRAPH_H_

#include "algcomm.h"

struct single_list;

struct digraph {
	unsigned int vertices;			/* number of vertices in this digraph */
	unsigned int edges;				/* number of edges in this digraph */
	struct single_list **adjlist;	/* adj[v] = adjacency list for vertex v */
	int *indegree;					/* indegree[v] = indegree of vertex v */
};

/* Returns the number of vertices in this digraph. */
#define DIGRAPH_VERTICES(g)		((g)->vertices)

/* Returns the number of edges in this digraph. */
#define DIGRAPH_EDGES(g)	((g)->edges)

/* Returns the vertices adjacent to vertex v. */
#define DIGRAPH_ADJLIST(g, v)	\
	((v) >= (g)->vertices ? NULL : (g)->adjlist[v])

/* Returns the number of directed edges incident from vertex v. */
#define DIGRAPH_OUTDEGREE(g, v)	\
	((v) >= (g)->vertices ? (-1) : (long)SLIST_LENGTH((g)->adjlist[v]))

/* Returns the number of directed edges incident to vertex v. */
#define DIGRAPH_INDEGREE(g, v)	\
	((v) >= (g)->vertices ? (-1) : (g)->indegree[v])

/* Initializes an empty digraph with n vertices and 0 edges. */
void digraph_init(struct digraph *g, unsigned int n);

/* 
 * Adds the directed edge v-w to this digraph. 
 * v one vertex in the edge, 
 * w the other vertex in the edge. 
 */
void digraph_add_edge(struct digraph *g, unsigned int v, unsigned int w);

/* 
 * Initializes a digraph from the specified file input stream.
 * The format is the number of vertices V,
 * followed by the number of edges E,
 * followed by E pairs of vertices,
 * with each entry separated by whitespace.
 */
void digraph_init_fistream(struct digraph *g, FILE *fin);

/* Prints this digraph of everyone vertex and them composed the edges. */
void digraph_print(const struct digraph *g);

/* Clones other digraph from the digraph */
void digraph_clone(const struct digraph *sg, struct digraph *tg);

/* Outputs the reverse of the digraph. */
void digraph_reverse(const struct digraph *sg, struct digraph *tg);

/* Clears this digraph. */
void digraph_clear(struct digraph *g);

#endif	/* _DIGRAPH_H_ */
