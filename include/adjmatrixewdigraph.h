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
#ifndef _ADJMATRIXEWDIGRAPH_H_
#define _ADJMATRIXEWDIGRAPH_H_

#include "algcomm.h"
#include <math.h>	/* isnan() */

struct diedge {
	long v;		/* one vertex */
	long w;		/* other vertex */
	float weight;	/* weight of this edge */
};

#define AMEWDG_EPSILON	1.OE-12

/* Initializes an edge between vertices v and w of the given weight. */
static inline void 
diedge_set_value(struct diedge *e, long v, long w, float wt)
{
	if (isnan(wt))
		errmsg_exit("Weight is NaN.\n");

	e->v = v;
	e->w = w;
	e->weight = wt;
}

/* Returns the weight of this edge */
#define DIEDGE_WEIGHT(edge)	((edge)->weight)

/* Returns the tail vertex of the directed edge. */
#define DIEDGE_FROM(edge)	((edge)->v)

/* Returns the head vertex of the directed edge. */
#define DIEDGE_TO(edge)		((edge)->w)

/* Determines a directed edge is whether valid. */
static inline int 
diedge_isvalid(const struct diedge *e)
{
	long v, w;

	v = DIEDGE_FROM(e);
	w = DIEDGE_TO(e);

	return v >= 0 && w >= 0;
}

/* Returns a string representation of this edge. */
#define DIEDGE_STRING(edge, str)	do {				\
	sprintf((str), "%ld->%ld %5.3f",				\
		(edge)->v, (edge)->w, (double)((edge)->weight));	\
} while (0)

struct adjmatrix_ewdigraph {
	unsigned int edges;
	unsigned int vertices;
	struct diedge **adjmat;
};

/* Returns the number of vertices in this edge-weighted graph. */
#define ADJMATEWDG_VERTICES(g)	((g)->vertices)

/* Returns the number of edges in this edge-weighted graph. */
#define ADJMATEWDG_EDGES(g)	((g)->edges)

/* Returns the directed edges incident from vertex v */
#define ADJMATEWDG_ADJMAT(g, v)	\
	((v) >= ADJMATEWDG_VERTICES(g) ? NULL : (g)->adjmat[v])

/* 
 * Initializes an empty edge-weighted digraph with vs vertices and 0 edges.
 */
void adjmatewdg_init(struct adjmatrix_ewdigraph *g, unsigned int vs);

/* 
 * Adds the directed edge e to the edge-weighted digraph
 * (if there is not already an edge with the same endpoints)
 */
void adjmatewdg_add_edge(struct adjmatrix_ewdigraph *g, const struct diedge *e);

/* 
 * Initializes a random edge-weighted digraph with vs vertices and es edges.
 */
void adjmatewdg_init_randomly(struct adjmatrix_ewdigraph *g, unsigned int vs,
			unsigned int es);

/* Prints this graph. */
void adjmatewdg_print(const struct adjmatrix_ewdigraph *g);

/* Clears this graph. */
void adjmatewdg_clear(struct adjmatrix_ewdigraph *g);

#endif	/* _ADJMATRIXEWDIGRAPH_H_ */
