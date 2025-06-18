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
#ifndef _SYMBOLGRAPH_H_
#define _SYMBOLGRAPH_H_

#include "graph.h"

struct avl_tree;

struct symbol_graph {
	struct avl_tree *st;	/* symbol table, string -> index */
	char **keys;		/* index -> string */
	struct graph *g;	/* the underlying graph */
};

/* Returns the name of the vertex associated with the integer v. */
#define SYBGRAPH_NAMEOF(sg, v)	\
	((v) < GRAPH_VERTICES((sg)->g) ? (sg)->keys[v] : NULL)

/* Returns the symbol graph of all the name of vertices */
#define SYBGRAPH_KEYS(sg)	((sg)->keys)

/* 
 * Returns the graph associated with the symbol graph.
 * It is the client's responsibility not to mutate the graph.
 */
#define SYBGRAPH_GRAPH(sg)	((sg)->g)

/* 
 * Initializes a graph from a file using the specified delimiter.
 * Each line in the file contains the name of a vertex,
 * followed by a list of the names of the vertices adjacent to that vertex,
 * separated by the delimiter. 
 */
void sybgraph_init(struct symbol_graph *sg, const char *filename,
		const char *delimiter);

/* Does the symbol graph contain the vertex named s */
int sybgraph_contains(const struct symbol_graph *sg, const char *s);

/* Returns the integer associated with the vertex named s. */
long sybgraph_indexof(const struct symbol_graph *sg, const char *s);

/* Prints this symbol graph. */
void sybgraph_print(const struct symbol_graph *sg);

/* Clears this symbol graph. */
void sybgraph_clear(struct symbol_graph *sg);

#endif	/* _SYMBOLGRAPH_H_ */
