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
#ifndef _DIJKSTRASP_H_
#define _DIJKSTRASP_H_

#include "edgeweighteddigraph.h"
#include <math.h>	/* INFINITY */

struct index_pheap;
struct single_list;

struct dijkstra_sp {
	float *distto;		/* distTo[v] = distance of shortest s->v path */
	struct diedge **edgeto;	/* edgeTo[v] = last edge on shortest
				s->v path */
	struct index_pheap *pq;	/* priority queue of vertices */
	unsigned int vertices;	/* edge-weighted digraph of vertices */
};

/* 
 * Returns the length of a shortest path from the source vertex s
 * to vertex v.
 */
#define DIJKSTRASP_DISTTO(sp, v)	\
	((v) >= (sp)->vertices ? INFINITY : (sp)->distto[v])

/* Returns true if there is a path from the source vertex s to vertex v. */
#define DIJKSTRASP_HAS_PATHTO(sp, v)	\
	((v) >= (sp)->vertices ? false : (sp)->distto[v] < INFINITY)

#define DIJKSTRASP_CLEAR(sp)	do {	\
	ALGFREE((sp)->distto);		\
	ALGFREE((sp)->edgeto);		\
	ipheap_clear((sp)->pq);		\
	ALGFREE((sp)->pq);		\
	(sp)->vertices = 0;		\
} while (0)

/* 
 * Computes a shortest-paths tree from the source vertex s
 * to every other vertex in the edge-weighted digraph g.
 */
void dijkstrasp_init(struct dijkstra_sp *sp, const struct ewdigraph *g,
		unsigned int s);

/* Gets a shortest path from the source vertex s to vertex v. */
void dijkstrasp_pathto(const struct dijkstra_sp *sp, unsigned int v, 
		struct single_list *paths);

#endif	/* _DIJKSTRASP_H_ */
