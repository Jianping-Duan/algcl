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
#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "algcomm.h"

struct single_list;

struct graph {
	unsigned int vertices;			/* graph of vertices */
	unsigned int edges;				/* graph of edges */
	struct single_list **adjlist;	/* graph of adjacent list */
};

/* Returns the number of vertices in this graph. */
#define GRAPH_VERTICES(g)	((g)->vertices)

/* Returns the number of edges in this graph. */
#define GRAPH_EDGES(g)	((g)->edges)

/* Returns the vertices adjacent to vertex v. */
#define GRAPH_ADJLIST(g, v)	\
	((v) >= (g)->vertices ? NULL : (g)->adjlist[v])

/* Returns the degree of vertex v. */
#define GRAPH_DEGREE(g, v)	\
	((v) >= (g)->vertices \
	? (-1) : (long)SLIST_LENGTH((g)->adjlist[v]))

/* 
 * Initializes an empty graph with 
 * n vertices and 0 edges. 
 */
void graph_init(struct graph *g, unsigned int n);

/* 
 * Adds the undirected edge v-w to this graph. 
 * v one vertex in the edge, 
 * w the other vertex in the edge. 
 */
int graph_add_edge(struct graph *g, unsigned int v, unsigned int w);

/* 
 * Initializes a graph from the specified 
 * file input stream.
 * The format is the number of vertices V,
 * followed by the number of edges E,
 * followed by E pairs of vertices, 
 * with each entry separated by whitespace. 
 */
void graph_init_fistream(struct graph *g, FILE *fin);

/* 
 * Prints this graph of everyone vertex and 
 * them composed the edges.
 */
void graph_print(const struct graph *g);

/* Clones other graph from the graph */
void graph_clone(const struct graph *sg, struct graph *tg);

/* Clears this graph. */
void graph_clear(struct graph *g);

#endif	/* _GRAPH_H_ */
