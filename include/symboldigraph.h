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
#ifndef _SYMBOLDIGRAPH_H_
#define _SYMBOLDIGRAPH_H_

#include "digraph.h"

struct redblack_bst;

struct symbol_digraph {
	struct redblack_bst *st;	/* symbol table, string -> index */
	char **keys;				/* index -> string */
	struct digraph *g;			/* the underlying digraph */
};

/* Does the symbol digraph contain the vertex named s */
#define SYBDIGRAPH_CONTAINS(sg, s)	\
	(rbbst_get((sg)->st, s) != NULL ? 1 : 0)

/* 
 * Returns the name of the vertex
 * associated with the integer v.
 */
#define SYBDIGRAPH_NAMEOF(sg, v)	\
	((v) < DIGRAPH_VERTICES((sg)->g) ? (sg)->keys[v] : NULL)

/* Returns the symbol digraph of all the name of vertices */
#define SYBDIGRAPH_KEYS(sg)	((sg)->keys)

/* 
 * Returns the digraph associated with the symbol digraph.
 * It is the client's responsibility not to mutate 
 * the digraph.
 */
#define SYBDIGRAPH_DIGRAPH(sg)	((sg)->g)

/* 
 * Initializes a digraph from a file using 
 * the specified delimiter. 
 * Each line in the file contains the name of a vertex,
 * followed by a list of the names of the vertices
 * adjacent to that vertex, separated by the delimiter.
 */
void sybdigraph_init(struct symbol_digraph *sg, const char *filename,
					const char *delimiter);

/* Returns the integer associated with the vertex named s */
long sybdigraph_indexof(const struct symbol_digraph *sg, const char *s);

/* Prints this symbollic directed graph. */
void sybdigraph_print(const struct symbol_digraph *sg);

/* Clears this symbollic directed graph. */
void sybdigraph_clear(struct symbol_digraph *sg);

#endif	/* _SYMBOLDIGRAPH_H_ */
