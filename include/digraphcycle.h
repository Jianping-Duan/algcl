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
#ifndef _DIGRAPHCYCLE_H_
#define _DIGRAPHCYCLE_H_

#include "digraph.h"

struct stack;

struct digraph_cycle {
	bool *marked;	/* marked[v] = has vertex v been marked? */
	long *edgeto;	/* edgeto[v] = last edge on digraph cycle path,
			   default edgeto[v] = -1 */
	bool *onstack;	/* onStack[v] = is vertex on the stack? */
	struct stack *cycle;	/* digraph of cycle */
};

/* Returns true if the digraph g has a cycle */
#define DIGRAPH_HASCYCLE(gc)	(!STACK_ISEMPTY((gc)->cycle))

/* Returns a cycle in the digraph g. */
#define DIGRAPH_CYCLE_GET(gc)	((gc)->cycle)

#define DIGRAPH_CYCLE_CLEAR(gc)		do {	\
	ALGFREE((gc)->marked);			\
	ALGFREE((gc)->edgeto);			\
	ALGFREE((gc)->onstack);			\
	stack_clear((gc)->cycle);		\
	ALGFREE((gc)->cycle);			\
} while (0)

/* 
 * Determines whether the digraph G has a directed cycle and,
 * if so, finds such a cycle.
 */
void digraph_cycle_init(struct digraph_cycle *gc, const struct digraph *g);

#endif	/* _DIGRAPHCYCLE_H_ */
