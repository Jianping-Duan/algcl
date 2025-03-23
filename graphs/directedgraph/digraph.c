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
#include "digraph.h"
#include "singlelist.h"

#define BUFSIZE		64

static int vequal(const void *, const void *);
static void adjlist_init(struct digraph *);

/* 
 * Initializes an empty digraph with 
 * n vertices and 0 edges. 
 */
void 
digraph_init(struct digraph *g, unsigned int n)
{
	unsigned int v;
	
	g->vertices = n;
	g->edges = 0;
	adjlist_init(g);
	
	g->indegree = (int *)algcalloc(n, sizeof(int));
	for(v = 0; v < n; v++)
		g->indegree[v] = 0;
}

/* 
 * Adds the directed edge v-w to this digraph. 
 * v one vertex in the edge, 
 * w the other vertex in the edge. 
 */
void 
digraph_add_edge(struct digraph *g, unsigned int v, unsigned int w)
{
	if(v >= g->vertices)
		return;
	
	if(w >= g->vertices)
		return;
	
	/* directed edge v -> w */
	slist_put(g->adjlist[v], &w);
	g->indegree[w]++;
	g->edges++;
}

/* 
 * Initializes a digraph from the specified 
 * file input stream.
 * The format is the number of vertices V,
 * followed by the number of edges E,
 * followed by E pairs of vertices, 
 * with each entry separated by whitespace. 
 */
void 
digraph_init_fistream(struct digraph *g, FILE *fin)
{
	unsigned int i, v, w, es, vs;
	char buf[BUFSIZE];
	
	fscanf(fin, "%u", &vs);
	fscanf(fin, "%u", &es);
	digraph_init(g, vs);
	
	fseek(fin, 2L, SEEK_CUR);
	for(i = 0; i < es; i++) {
		fgets(buf, BUFSIZE, fin);
		sscanf(buf, "%u %u", &v, &w);
		digraph_add_edge(g, v, w);
		memset(buf, 0, BUFSIZE);
	}
}

/* 
 * Prints this digraph of everyone vertex and 
 * them composed the edges.
 */
void 
digraph_print(const struct digraph *g)
{
	unsigned int v, *w;
	struct single_list *slist;
	struct slist_node *nptr;
	
	printf("%u vertices, %u edges.\n", g->vertices, g->edges);
	for(v = 0; v < g->vertices; v++) {
		printf("%u: ", v);
		slist = DIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			printf("%u ", *w);
		}
		printf("\n");
	}
}

/* Clones other digraph from the digraph */
void 
digraph_clone(const struct digraph *sg, struct digraph *tg)
{
	unsigned int v;
	
	tg->vertices = sg->vertices;
	tg->edges = sg->edges;
	
	tg->indegree = (int *)algcalloc(sg->vertices, sizeof(int));
	for(v = 0; v < sg->vertices; v++)
		tg->indegree[v] = sg->indegree[v];
	
	adjlist_init(tg);
	for(v = 0; v < tg->vertices; v++)
		slist_clone(sg->adjlist[v], tg->adjlist[v]);
}

/* Outputs the reverse of the digraph. */
void 
digraph_reverse(const struct digraph *sg, struct digraph *tg)
{
	unsigned int v, *w;
	struct single_list *slist;
	struct slist_node *nptr;
	
	digraph_init(tg, sg->vertices);
	for(v = 0; v < sg->vertices; v++) {
		slist = DIGRAPH_ADJLIST(sg, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			digraph_add_edge(tg, *w, v);
		}
	}
}

void 
digraph_clear(struct digraph *g)
{
	unsigned int v;
	
	for(v = 0; v < g->vertices; v++)
		slist_clear(g->adjlist[v]);
		
	ALGFREE(g->adjlist);
	ALGFREE(g->indegree);

	g->vertices = 0;
	g->edges = 0;
}

/******************** static function boundary ********************/

static int 
vequal(const void *k1, const void *k2)
{
	int *v = (int *)k1, *w = (int *)k2;
	return *v == *w;
}

/* Initializes the graph of adjacent lists */
static void 
adjlist_init(struct digraph *g)
{
	unsigned int v, n;

	n = g->vertices;
	g->adjlist = (struct single_list **)
		algmalloc(n * sizeof(struct single_list *));
	
	for(v = 0; v < n; v++) {
		g->adjlist[v] = (struct single_list *)
			algmalloc(sizeof(struct single_list));
		slist_init(g->adjlist[v], sizeof(int), vequal);
	}
}
