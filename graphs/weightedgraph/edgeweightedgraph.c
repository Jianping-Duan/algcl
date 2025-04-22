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
#include "edgeweightedgraph.h"
#include "linearlist.h"

#define BUFSIZE		128

static void adjlist_init(struct ewgraph *);

/* Initializes an empty edge-weighted graph with n vertices and 0 edges. */
void 
ewgraph_init(struct ewgraph *g, unsigned int vs)
{
	g->vertices = vs;
	g->edges = 0;
	adjlist_init(g);
}

/* 
 * Adds the undirected edge e to 
 * this edge-weighted graph. 
 */
void 
ewgraph_add_edge(struct ewgraph *g, const struct edge *e)
{
	unsigned int v, w;

	v = EDGE_EITHER(e);
	w = edge_other(e, v);

	if (v >= EWGRAPH_VERTICES(g)) {
		errmsg_exit("vertex %u is not between 0 and %u\n", v, 
			EWGRAPH_VERTICES(g) - 1);
	}
	if (w >= EWGRAPH_VERTICES(g)) {
		errmsg_exit("vertex %u is not between 0 and %u\n", w, 
			EWGRAPH_VERTICES(g) - 1);
	}

	slist_put(g->adjlist[v], (void *)e);
	slist_put(g->adjlist[w], (void *)e);
	g->edges++;
}

/* Initializes a random edge-weighted graph with vs vertices and es edges. */
void 
ewgraph_init_randomly(struct ewgraph *g, unsigned int vs, unsigned int es)
{
	unsigned int v, w, e;
	float weight;
	struct edge *se;

	ewgraph_init(g, vs);
	for (e = 0; e < es; e++) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		weight = (float)0.01 * rand_range_integer(1, 100);

		se = make_edge(v, w, weight);
		ewgraph_add_edge(g, se);
	}
}

/* 
 * Initializes an edge-weighted graph from an file input stream.
 * The format is the number of vertices v,
 * followed by the number of edges e,
 * followed by e pairs of vertices and edge weights,
 * with each entry separated by whitespace.
 */
void 
ewgraph_init_fistream(struct ewgraph *g, FILE *fin)
{
	unsigned int i, v, w, es, vs;
	float weight;
	char buf[BUFSIZE];
	struct edge *se;
	
	fscanf(fin, "%u", &vs);
	fscanf(fin, "%u", &es);
	ewgraph_init(g, vs);
	
	fseek(fin, 2L, SEEK_CUR);
	for (i = 0; i < es; i++) {
		fgets(buf, BUFSIZE, fin);
		sscanf(buf, "%u %u %f", &v, &w, &weight);
		se = make_edge(v, w,weight);
		ewgraph_add_edge(g, se);
		memset(buf, 0, BUFSIZE);
	}
}

/* Initializes a new edge-weighted graph that is a deep copy of tg. */
void 
ewgraph_clone(const struct ewgraph *sg, struct ewgraph *tg)
{
	unsigned int v;
	struct single_list *adj;
	struct slist_node *nptr;
	struct stack rg;
	struct edge *se, *te;

	tg->vertices = EWGRAPH_VERTICES(sg);
	tg->edges = EWGRAPH_EDGES(sg);
	adjlist_init(tg);

	STACK_INIT(&rg, 0);
	for (v = 0; v < EWGRAPH_VERTICES(sg); v++) {
		/* Reverse so that adjacency list is in same order as original. */
		adj = EWGRAPH_ADJLIST(sg, v);
		SLIST_FOREACH(adj, nptr, struct edge, se) {
			te = make_edge(se->v, se->w, se->weight);
			stack_push(&rg, te);
		}

		while (!STACK_ISEMPTY(&rg)) {
			stack_pop(&rg, (void **)&te);
			slist_put(tg->adjlist[v], te);
		}
		stack_clear(&rg);
	}
}

/* Gets all edges in this edge-weighted graph. */
void 
ewgraph_edges_get(const struct ewgraph *g, struct single_list *edgeset)
{
	unsigned int v, selfloops;
	struct single_list *adj;
	struct slist_node *nptr;
	struct edge *e;

	slist_init(edgeset, 0, edge_equals);
	for (v = 0; v < EWGRAPH_VERTICES(g); v++) {
		selfloops = 0;
		adj = EWGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(adj, nptr, struct edge, e) {
			if (edge_other(e, v) > v) /* avoid cycle */
				slist_append(edgeset, e);
			/* free only one copy of each self loop */
			else if (edge_other(e, v) == v) {
				if (selfloops % 2 == 0)
					slist_append(edgeset, e);
				selfloops++;
			}
		}
	}
}

void 
ewgraph_print(const struct ewgraph *g)
{
	unsigned int v;
	char se[128];
	struct single_list *adj;
	struct slist_node *nptr;
	struct edge *e;

	printf("%u vertices, %u edges.\n", EWGRAPH_VERTICES(g), EWGRAPH_EDGES(g));
	for (v = 0; v < EWGRAPH_VERTICES(g); v++) {
		printf("%u: ", v);
		adj = EWGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(adj, nptr, struct edge, e) {
			EDGE_STRING(e, se);
			printf("%s ", se);
		}
		printf("\n");
	}
}

void 
ewgraph_clear(struct ewgraph *g)
{
	unsigned int v;
	struct single_list edgeset;
	struct slist_node *nptr;
	struct edge *e;

	ewgraph_edges_get(g, &edgeset);
	SLIST_FOREACH(&edgeset, nptr, struct edge, e) {
		ALGFREE(e);
	}

	for (v = 0; v < EWGRAPH_VERTICES(g); v++)
		slist_clear(g->adjlist[v]);
	ALGFREE(g->adjlist);
	
	g->vertices = 0;
	g->edges = 0;
}

/******************** static function boundary ********************/

static void 
adjlist_init(struct ewgraph *g)
{
	unsigned int v;

	g->adjlist = (struct single_list **)
		algcalloc(g->vertices, sizeof(struct single_list *));
	for (v = 0; v < g->vertices; v++) {
		g->adjlist[v] = (struct single_list *)
			algmalloc(sizeof(struct single_list));
		slist_init(g->adjlist[v], 0, edge_equals);
	}
}
