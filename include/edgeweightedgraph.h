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
#ifndef _EDGEWEIGHTEDGRAPH_H_
#define _EDGEWEIGHTEDGRAPH_H_

#include "algcomm.h"
#include <math.h>	/* isnan() */

struct single_list;

struct edge {
	unsigned int v;		/* one vertex */
	unsigned int w;		/* other vertex */
	float weight;		/* weight of this edge */
};

/* edge-weighted graph */
struct ewgraph {
	unsigned int vertices;	/* edge-weighted graph of vertices */
	unsigned int edges;		/* edge-weighted graph of edges */
	struct single_list **adjlist;	/* adjacent list */
};

#define FLOATING_POINT_EPSILON	1.OE-6

/* 
 * Initializes an edge between vertices 
 * v and w of the given weight. 
 */
static inline struct edge * 
make_edge(unsigned int v, unsigned int w, float wt)
{
	struct edge *e;

	if(isnan(wt))
		errmsg_exit("Weight is NaN.\n");

	e = (struct edge *)algmalloc(sizeof(struct edge));
	e->v = v;
	e->w = w;
	e->weight = wt;

	return e;
}

/* Returns the weight of this edge */
#define EDGE_WEIGHT(edge)	((edge)->weight)

/* Returns either endpoint of this edge. */
#define EDGE_EITHER(edge)	((edge)->v)

/* 
 * Returns the endpoint of this edge that is 
 * different from the given vertex. 
 */
static inline unsigned int 
edge_other(const struct edge *e, unsigned int v)
{
	if(e->v == v)
		return e->w;
	else if(e->w == v)
		return e->v;
	else
		errmsg_exit("Illegal endpoint, %u\n", v);
	
	return 1;
}

/* 
 * Two edges are whether equals? 
 * (it used the single linked list)
 */
static inline int 
edge_equals(const void *k1, const void *k2)
{
	struct edge *e1, *e2;
	char w1[48], w2[48];

	e1 = (struct edge *)k1;
	e2 = (struct edge *)k2;

	if((e1->v == e2->v && e1->w == e2->w) || 
	   (e1->w == e2->v && e1->v == e2->w)) {
		sprintf(w1, "%.5f", (double)(e1->weight));
		sprintf(w2, "%.5f", (double)(e2->weight));
		return strcmp(w1, w2) == 0;
	}
	else
		return 0;
}

/* Returns a string representation of this edge. */
#define EDGE_STRING(edge, str)	\
	do {						\
		sprintf((str), "%u-%u %.5f", (edge)->v, (edge)->w, \
			(double)((edge)->weight));	\
} while(0)

/* 
 * Returns the number of vertices in 
 * this edge-weighted graph. 
 */
#define EWGRAPH_VERTICES(g)	((g)->vertices)

/* 
 * Returns the number of edges in 
 * this edge-weighted graph.
 */
#define EWGRAPH_EDGES(g)	((g)->edges)

/* Returns the edges incident on vertex. */
#define EWGRAPH_ADJLIST(g, v)	\
	((v) >= (g)->vertices ? NULL : (g)->adjlist[v])

/* Returns the degree of vertex */
#define EWGRAPH_DEGREE(g, v)	\
	((v) >= (g)->vertices \
		? (-1) : (long)SLIST_LENGTH((g)->adjlist[v]))

/* 
 * Initializes an empty edge-weighted graph 
 * with n vertices and 0 edges. 
 */
void ewgraph_init(struct ewgraph *g, unsigned int vs);

/* 
 * Adds the undirected edge e to 
 * this edge-weighted graph. 
 */
void ewgraph_add_edge(struct ewgraph *g, const struct edge *e);

/* 
 * Initializes a random edge-weighted graph 
 * with vs vertices and es edges.
 */
void ewgraph_init_randomly(struct ewgraph *g, unsigned int vs,
						unsigned int es);

/* 
 * Initializes an edge-weighted graph from 
 * an file input stream.
 * The format is the number of vertices v,
 * followed by the number of edges e,
 * followed by e pairs of vertices and edge weights,
 * with each entry separated by whitespace.
 */
void ewgraph_init_fistream(struct ewgraph *g, FILE *fin);

/* 
 * Initializes a new edge-weighted graph 
 * that is a deep copy of tg. 
 */
void ewgraph_clone(const struct ewgraph *sg, struct ewgraph *tg);

/* Prints this edge-weighted graph. */
void ewgraph_print(const struct ewgraph *g);

/* Gets all edges in this edge-weighted graph. */
void ewgraph_edges_get(const struct ewgraph *g, struct single_list *edgeset);

/* Clears this edge-weighted graph. */
void ewgraph_clear(struct ewgraph *g);

#endif	/* _EDGEWEIGHTEDGRAPH_H_ */
