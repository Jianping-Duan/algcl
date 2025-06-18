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
#ifndef _DIJKSTRAPAIRSP_H_
#define _DIJKSTRAPAIRSP_H_

#include "dijkstrasp.h"

struct dijkstra_pair_sp {
	unsigned int vertices;		/* edge-weight digraph of vertices */
	struct dijkstra_sp **allsp;	/* all vertices of shortest path */
};

/* Is there a path from the vertex s to vertex t? */
#define DJKR_PAIRSP_HAS_PATH(psp, s, t)	\
	(djkr_pairsp_dist(psp, s, t) < INFINITY)

struct single_list;

/* 
 * Computes a shortest paths tree from each vertex to every other vertex in
 * the edge-weighted digraph g. 
 */
void djkr_pairsp_init(struct dijkstra_pair_sp *psp, const struct ewdigraph *g);

/* Returns a shortest path from vertex s to vertex t. */
struct single_list * 
djkr_pairsp_path(const struct dijkstra_pair_sp *psp, unsigned int s,
		unsigned int t);

/* Returns the length of a shortest path from vertex s to vertex t. */
float djkr_pairsp_dist(const struct dijkstra_pair_sp *psp, unsigned int s,
		unsigned int t);

/* Clear operstion. */
void djkr_pairsp_clear(struct dijkstra_pair_sp *psp);

#endif	/* _DIJKSTRAPAIRSP_H_ */
