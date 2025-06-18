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
#ifndef _BELLMANFORDSP_H_
#define _BELLMANFORDSP_H_

#include "edgeweighteddigraph.h"
#include "stack.h"
#include <math.h>	/* INFINITY */

struct queue;
struct singlelist;

struct bellmanford_sp {
	float *distto;		/* distTo[v] = distance of shortest s->v path */
	struct diedge **edgeto;	/* edgeTo[v] = last edge on shortest
				s->v path */
	bool *onqueue;		/* onQueue[v] = is v currently on the queue? */
	struct queue *quvr;	/* queue of vertices to relax */
	unsigned int cost;	/* number of calls to relax() */
	struct stack *cycle;	/* negative cycle (or null if no such cycle) */
	unsigned int vertices;	/* edge-weighted digraph of vertices */
};

/* Is there a negative cycle reachable from the source vertex s? */
#define BMFSP_HAS_NEGATIVE_CYCLE(sp)	\
	((sp)->cycle != NULL && !STACK_ISEMPTY((sp)->cycle))

/* Returns a negative cycle reachable from the source vertex s. */
#define BMFSP_NEGATIVE_CYCLE(sp)	((sp)->cycle)

/* 
 * Returns the length of a shortest path from the source vertex s to vertex v.
 */
static inline float 
bmfsp_distto(const struct bellmanford_sp *sp,
			unsigned int v)
{
	if (v >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			sp->vertices - 1);
	}
	
	if (BMFSP_HAS_NEGATIVE_CYCLE(sp))
		errmsg_exit("Negative cost cycle exists.\n");

	return sp->distto[v];
}

/* Is there a path from the source s to vertex v? */
#define BMFSP_HAS_PATHTO(sp, v)		(bmfsp_distto(sp, v) < INFINITY)

#define BMFSP_CLEAR(sp)		do {	\
	ALGFREE((sp)->distto);		\
	ALGFREE((sp)->edgeto);		\
	ALGFREE((sp)->onqueue);		\
	queue_clear((sp)->quvr);	\
	ALGFREE((sp)->quvr);		\
	stack_clear((sp)->cycle);	\
	ALGFREE((sp)->cycle);		\
} while (0)

/* 
 * Computes a shortest paths tree from s to every other vertex in the
 * edge-weighted digraph g. 
 */
void bmfsp_init(struct bellmanford_sp *sp, const struct ewdigraph *g,
		unsigned int s);

/* Gets a shortest path from the source s to vertex v. */
void bmfsp_paths_get(const struct bellmanford_sp *sp, unsigned int v,
		struct single_list *paths);

#endif	/* _BELLMANFORDSP_H_ */
