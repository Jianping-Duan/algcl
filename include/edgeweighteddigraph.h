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
#ifndef _EDGEWEIGHTEDDIGRAPH_H_
#define _EDGEWEIGHTEDDIGRAPH_H_

#include "algcomm.h"
#include <math.h>	/* isnan() */

struct diedge {
	unsigned int v;		/* one vertex */
	unsigned int w;		/* other vertex */
	float weight;		/* weight of this edge */
};

struct single_list;

/* edge-weighted digraph */
struct ewdigraph {
	unsigned int vertices;	/* number of vertices in this digraph */
	unsigned int edges;		/* number of edges in this digraph */
	struct single_list **adjlist;	/* ajdlist[v] = adjacency,
									   list for vertex v */
	int *indegree;	/* indegree[v] = indegree of vertex v */
};

#define FLOATING_POINT_EPSILON	1.OE-6

/* 
 * Initializes an edge between vertices 
 * v and w of the given weight. 
 */
static inline struct diedge * 
make_diedge(unsigned int v, unsigned int w, float wt)
{
	struct diedge *e;

	if(isnan(wt))
		errmsg_exit("Weight is NaN.\n");

	e = (struct diedge *)algmalloc(sizeof(struct diedge));
	e->v = v;
	e->w = w;
	e->weight = wt;

	return e;
}

/* Returns the weight of this edge */
#define DIEDGE_WEIGHT(edge)	((edge)->weight)

/* Returns the tail vertex of the directed edge. */
#define DIEDGE_FROM(edge)	((edge)->v)

/* Returns the head vertex of the directed edge. */
#define DIEDGE_TO(edge)		((edge)->w)

/* 
 * Two directed edges are whether equals? 
 * (it used the single linked list)
 */
static inline int 
diedge_equals(const void *k1, const void *k2)
{
	struct diedge *e1, *e2;
	char w1[48], w2[48];

	e1 = (struct diedge *)k1;
	e2 = (struct diedge *)k2;

	if((e1->v == e2->v && e1->w == e2->w)) {
		sprintf(w1, "%.5f", (double)(e1->weight));
		sprintf(w2, "%.5f", (double)(e2->weight));
		return strcmp(w1, w2) == 0;
	}
	else
		return 0;
}

/* Returns a string representation of this edge. */
#define DIEDGE_STRING(edge, str)	do {	\
	sprintf((str), "%u->%u %5.3f", \
	(edge)->v, (edge)->w, (double)((edge)->weight));	\
} while(0)

/* 
 * Returns the number of vertices in
 * this edge-weighted graph.
 */
#define EWDIGRAPH_VERTICES(g)	((g)->vertices)

/* 
 * Returns the number of edges in 
 * this edge-weighted graph.
 */
#define EWDIGRAPH_EDGES(g)	((g)->edges)

/* Returns the edges incident on vertex. */
#define EWDIGRAPH_ADJLIST(g, v)	\
	((v) >= (g)->vertices ? NULL : (g)->adjlist[v])

/* 
 * Returns the number of directed edges 
 * incident from vertex v.
 */
#define EWDIGRAPH_OUTDEGREE(g, v)	\
	((v) >= (g)->vertices ? (-1) : \
	(long)SLIST_LENGTH((g)->adjlist[v]))

/* 
 * Returns the number of directed edges
 * incident to vertex v.
 */
#define EWDIGRAPH_INDEGREE(g, v)	\
	((v) >= (g)->vertices ? (-1) : (g)->indegree[v])

/* 
 * Initializes an empty edge-weighted digraph 
 * with n vertices and 0 edges.
 */
void ewdigraph_init(struct ewdigraph *g, unsigned int vs);

/* 
 * Adds the directed edge e to
 * this edge-weighted digraph.
 */
void ewdigraph_add_edge(struct ewdigraph *g, const struct diedge *e);

/* 
 * Initializes a random edge-weighted digraph 
 * with vs vertices and es edges.
 */
void ewdigraph_init_randomly(struct ewdigraph *g, unsigned int vs,
							unsigned int es);

/* 
 * Initializes an edge-weighted digraph from 
 * an file input stream.
 * The format is the number of vertices v,
 * followed by the number of edges e,
 * followed by e pairs of vertices and edge weights,
 * with each entry separated by whitespace.
 */
void ewdigraph_init_fistream(struct ewdigraph *g, FILE *fin);

/* 
 * Initializes a new edge-weighted digraph that 
 * is a deep copy of tg. 
 */
void ewdigraph_clone(const struct ewdigraph *sg, struct ewdigraph *tg);

/* Prints this edge-weighted digraph. */
void ewdigraph_print(const struct ewdigraph *g);

/* Gets all edges in this edge-weighted graph. */
void ewdigraph_edges_get(const struct ewdigraph *g, 
						struct single_list *edgeset);

/* Clears this edge-weighted digraph. */
void ewdigraph_clear(struct ewdigraph *g);

#endif	/* _EDGEWEIGHTEDDIGRAPH_H_ */
