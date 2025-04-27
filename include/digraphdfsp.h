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
#ifndef _DIGRAPHDFSP_H_
#define _DIGRAPHDFSP_H_

#include "digraph.h"

struct digraph_dfsp {
	bool *marked;	/* marked[v] = is there an sv-v path? */
	long *edgeto;	/* edgeto[v] = last edge on sv-v path,
					   default edgeto[v] = -1 */
	unsigned int sv;		/* source vertex */
	unsigned int vertices;	/* digraph of vertices */
};

/* Is there a path between the source vertex sv and vertex v? */
#define DIGRAPH_DFSP_HASPATH(dfs, v)	\
	((v) >= (dfs)->vertices ? false : (dfs)->marked[v])

#define DIGRAPH_DFSP_CLEAR(dfs)	do {	\
	ALGFREE((dfs)->marked);				\
	ALGFREE((dfs)->edgeto);				\
	(dfs)->vertices = 0;				\
} while (0)

struct stack;

/* Computes a path between s and every other vertex in digraph g. */
void digraph_dfsp_init(struct digraph_dfsp *dfs, unsigned int s,
					const struct digraph *g);

/* 
 * Returns a path between the source vertex sv and vertex v,
 * or if no such path. 
 */
void digraph_dfsp_paths(struct digraph_dfsp *dfs, unsigned int v, 
						struct stack *paths);

#endif	/* _DIGRAPHDFSP_H_ */
