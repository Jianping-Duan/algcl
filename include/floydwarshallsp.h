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
#ifndef _FLOYDWARSHALLSP_H_
#define _FLOYDWARSHALLSP_H_

#include "adjmatrixewdigraph.h"
#include <math.h>	/* INFINITY */

/* Floyd-Warshall shortest path */
struct floyd_warshall_sp {
	float **distto;			/* distTo[v][w] = length of shortest v->w path */
	struct diedge **edgeto;	/* edgeTo[v][w] = last edge on shortest v->w path */
	bool negcycle;			/* is there a negative cycle? */
	unsigned int vertices;	/* graph of vertices */
};

/* Is there a negative cycle? */
#define FWSP_HAS_NEGATIVE_CYCLE(sp)		((sp)->negcycle)

/* 
 * Returns the length of a shortest path from 
 * vertex s to vertex t.
 */
static inline float 
fwsp_distto(const struct floyd_warshall_sp *sp, 
			unsigned int s, unsigned int t)
{
	if(s >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", s,
			sp->vertices - 1);
	}
	
	if(t >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", t,
			sp->vertices - 1);
	}

	if(FWSP_HAS_NEGATIVE_CYCLE(sp))
		errmsg_exit("Negative cost cycle exists.\n");
	
	return sp->distto[s][t];
}

/* Is there a path from the vertex s to vertex t. */
#define FWSP_HAS_PATH(sp, s, t)	\
	(fwsp_distto(sp, s, t) < INFINITY)

struct single_list;

/* 
 * Computes a shortest paths tree from each vertex to every
 * other vertex in the edge-weighted digraph g. 
 * If no such shortest path exists for some pair of
 * vertices, it computes a negative cycle. 
 */
void fwsp_init(struct floyd_warshall_sp *sp, 
			const struct adjmatrix_ewdigraph *g);

/* Returns a shortest path from vertex s to vertex t. */
void fwsp_path_get(const struct floyd_warshall_sp *sp, unsigned int s,
				unsigned int t, struct single_list *paths);

/* Clears */
void fwsp_clear(struct floyd_warshall_sp *sp);

#endif	/* _FLOYDWARSHALLSP_H_ */
