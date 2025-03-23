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
#ifndef _GRAPHCYCLE_H_
#define _GRAPHCYCLE_H_

#include "graph.h"
#include "stack.h"

struct graph_cycle {
	bool *marked;	/* marked[v] = has vertex v been marked? */
	long *edgeto;	/* edgeto[v] = last edge on graph cycle path,
					   default edgeto[v] = -1 */
	struct stack cycle;	/* graph of cycle */	
};

/* Returns true if the graph g has a cycle */
#define GRAPH_HASCYCLE(gc)	\
	(!STACK_ISEMPTY(&(gc)->cycle))

/* Returns a cycle in the graph g. */
#define GRAPH_CYCLE(gc)	((gc)->cycle)

#define GRAPH_CYCLE_CLEAR(gc)	do {	\
	ALGFREE((gc)->marked);				\
	ALGFREE((gc)->edgeto);				\
} while(0)

/* Initialize an empty cycle. */
void graph_cycle_init(struct graph_cycle *gc, const struct graph *g);

/* 
 * Determines whether the undirected graph g has a cycle and,
 * if so, finds such a cycle.
 */
void graph_cycle_get(struct graph_cycle *gc, const struct graph *g);

/* 
 * Does this graph have a self loop? side effect: 
 * initialize cycle to be self loop.
 */
bool graph_self_loop(struct graph_cycle *gc, const struct graph *g);

/* 
 * Does this graph have two parallel edges? side effect:
 * initialize cycle to be two parallel edges.
 */
bool graph_parallel_edges(struct graph_cycle *gc, const struct graph *g);

#endif	/* _GRAPHCYCLE_H_ */
