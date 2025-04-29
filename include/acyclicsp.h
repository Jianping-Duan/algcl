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
#ifndef _ACYCLICSP_H_
#define _ACYCLICSP_H_

#include "edgeweighteddigraph.h"
#include <math.h>	/* INFINITY */

struct acyclicsp {
	float *distto;	/* distto[v] = distance of shortest s->v path */
	struct diedge **edgeto;	/* edgeTo[v] = last edge on shortest s->v path */
	unsigned int vertices;	/* edge-weighted digraph of vertices */
};

/* 
 * Returns the length of a shortest path from the source vertex s to vertex v.
 */
static inline float 
acycsp_distto(struct acyclicsp *sp, unsigned int v)
{
	if (v >= sp->vertices) {
		errmsg_exit("vertex %u is not between 0 and %u.\n", v,
			sp->vertices - 1);
	}

	return sp->distto[v];
}

/* Is there a path from the source vertex s to vertex v ? */
#define ACYCSP_HAS_PATHTO(sp, v)	\
	(acycsp_distto(sp, v) < INFINITY)

#define ACYCSP_CLEAR(sp)	do {	\
	ALGFREE((sp)->distto);			\
	ALGFREE((sp)->edgeto);			\
	(sp)->vertices = 0;				\
} while(0)

struct single_list;

/* 
 * Computes a shortest paths tree from s to every other vertex in 
 * the directed acyclic graph g. 
 */
void acycsp_init(struct acyclicsp *sp, const struct ewdigraph *g,
				unsigned int s);

/* Gets a shortest path from the source vertex s to vertex v. */
void acycsp_paths_get(const struct acyclicsp *sp, unsigned int v, 
					struct single_list *paths);

#endif	/* _ACYCLICSP_H_ */
