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
#include "graphgenerator.h"
#include "avltree.h"
#include "skiplist.h"
#include "pairingheap.h"
#include <math.h>	/* log2(), ceil() */

static int
vcmp(const void *v, const void *w)
{
	long *x = (long *)v, *y = (long *)w;
	if (*x < *y)
		return 1;
	else if (*x == *y)
		return 0;
	else
		return -1;
}

/* 
 * Generates a random simple graph containing vs vertices and es edges,
 * output a graph g.
 */
void 
simple_graph1(unsigned int vs, unsigned int es, struct graph *g)
{
	unsigned int v, w;
	struct avl_tree bst;
	long key;
	char strkey[21];
	
	if (es > vs * (vs - 1) / 2)
		return;
	
	avlbst_init(&bst, sizeof(long), vcmp);
	graph_init(g, vs);
	while (GRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0 ,vs);
		
		sprintf(strkey, "%u%u", v, w);
		sscanf(strkey, "%ld", &key);
		if (v != w && avlbst_get(&bst, &key) == NULL) {
			avlbst_put(&bst, &key);
			graph_add_edge(g, v, w);
		}
	}
	avlbst_clear(&bst);
}

/* 
 * Generates a random simple graph on vs vertices, 
 * with an edge between any two vertices with probability p.
 * This is sometimes referred to as the Erdos-Renyi 
 * random graph model. 
 */
void 
simple_graph2(unsigned int vs, double p, struct graph *g)
{
	unsigned int v, w;
	
	if (p < 0.0 || p > 1.0)
		errmsg_exit("Probability must be between 0.0 and 1.0.\n");
	
	graph_init(g, vs);
	for (v = 0; v < vs; v++)
		for (w = v + 1; w < vs; w++)
			if (bernoulli_distribution(p))
				graph_add_edge(g, v, w);
}

/* 
 * Generates a random simple bipartite graph on 
 * vs1 and vs2 vertices with es edges. 
 */
void 
bipartite_graph1(unsigned int vs1, unsigned int vs2, unsigned int es,
		struct graph *g)
{
	unsigned int *vertices, i, j;
	struct avl_tree bst;
	long key;
	char strkey[21];
	
	if (es > vs1 * vs2)
		errmsg_exit("The simple bipartite graph have too "
			"many edges.\n");
	
	vertices = (unsigned int *)algmalloc(vs1 * vs2 * sizeof(int));
	
	for (i = 0; i < vs1 + vs2; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs1 + vs2);
	
	graph_init(g, vs1 + vs2);
	avlbst_init(&bst, sizeof(long), vcmp);
	while (GRAPH_EDGES(g) < es) {
		i = rand_range_integer(0, vs1);
		j = vs1 + rand_range_integer(0, vs2);
		
		sprintf(strkey, "%u%u", vertices[i], vertices[j]);
		sscanf(strkey, "%ld", &key);
		if (avlbst_get(&bst, &key) == NULL) {
			avlbst_put(&bst, &key);
			graph_add_edge(g, vertices[i], vertices[j]);
		}
	}
	avlbst_clear(&bst);
	ALGFREE(vertices);
}

/* 
 * Generates a random simple bipartite graph on 
 * vs1 and vs2 vertices, 
 * containing each possible edge with probability p.
 */
void 
bipartite_graph2(unsigned int vs1, unsigned int vs2, double p, struct graph *g)
{
	unsigned int *vertices, i, j;
	
	vertices = (unsigned int *)algmalloc(vs1 * vs2 * sizeof(int));
	for (i = 0; i < vs1 + vs2; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs1 + vs2);
	
	graph_init(g, vs1 + vs2);
	for (i = 0; i < vs1; i++)
		for (j = 0; j < vs2; j++)
			if (bernoulli_distribution(p)) {
				graph_add_edge(g, vertices[i], 
					vertices[vs1 + j]);
			}
	
	ALGFREE(vertices);
}

/* Generates a path graph on vs vertices. */
void 
path_graph(unsigned int vs, struct graph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	graph_init(g, vs);
	for (i = 0; i < vs - 1; i++)
		graph_add_edge(g, vertices[i], vertices[i + 1]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a complete binary tree graph on vs vertices.
 * vs the number of vertices in the binary tree. 
 */
void 
binary_tree_graph(unsigned int vs, struct graph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	graph_init(g, vs);
	for (i = 1; i < vs; i++)
		graph_add_edge(g, vertices[i], vertices[(i - 1)/2]);
	
	ALGFREE(vertices);
}

/* Generates a cycle graph on vs vertices. */
void 
cycle_graph(unsigned int vs, struct graph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	graph_init(g, vs);
	for (i = 0; i < vs - 1; i++)
		graph_add_edge(g, vertices[i], vertices[i + 1]);
	graph_add_edge(g, vertices[vs - 1], vertices[0]);
	
	ALGFREE(vertices);
}

/* Generates an Eulerian cycle graph on vs vertices. */
void 
eulerian_cycle_graph(unsigned int vs, unsigned int es, struct graph *g)
{
	unsigned int *vertices, i, v, cnt;
	struct skip_list sl;
	
	vertices = (unsigned int *)algmalloc(es * sizeof(int));
	
	skipl_init(&sl, (int)ceil(log2((double)vs)), sizeof(long), vcmp);
	for (i = 0, cnt = 0; i < es; i++) {
		v = rand_range_integer(0, vs);
		if (skipl_get(&sl, &v) != NULL)
			continue;
		else {
			skipl_put(&sl, &v);
			vertices[cnt++] = v;
		}
	}
	skipl_clear(&sl);
	
	graph_init(g, vs);
	for (i = 0; i < cnt - 1; i++)
		graph_add_edge(g, vertices[i], vertices[i + 1]);
	graph_add_edge(g, vertices[cnt - 1], vertices[0]);
	
	ALGFREE(vertices);
}

/* Returns an Eulerian path graph on vs vertices. */
void 
eulerian_path_graph(unsigned int vs, unsigned int es, struct graph *g)
{
	unsigned int *vertices, i, v, cnt;
	struct skip_list sl;
	
	vertices = (unsigned int *)algmalloc((es + 1) * sizeof(int));
	
	skipl_init(&sl, (int)ceil(log2((double)vs)), sizeof(long), vcmp);
	for(i = 0, cnt = 0; i < es + 1; i++) {
		v = rand_range_integer(0, vs);
		if(skipl_get(&sl, &v) != NULL)
			continue;
		else {
			skipl_put(&sl, &v);
			vertices[cnt++] = v;
		}
	}
	skipl_clear(&sl);
	
	graph_init(g, vs);
	for (i = 0; i < cnt - 1; i++)
		graph_add_edge(g, vertices[i], vertices[i + 1]);
	
	ALGFREE(vertices);
}

/* Generates a wheel graph on vs vertices.*/
void 
wheel_graph(unsigned int vs, struct graph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	graph_init(g, vs);
	
	/* simple cycle on vs-1 vertices */
	for (i = 1; i < vs - 1; i++)
		graph_add_edge(g, vertices[i], vertices[i + 1]);
	graph_add_edge(g, vertices[vs - 1], vertices[1]);
	
	/* connect vertices[0] to every vertex on cycle */
	for (i = 1; i < vs; i++)
		graph_add_edge(g, vertices[0], vertices[i]);
	
	ALGFREE(vertices);
}

/* Generates a star graph on vs vertices. */
void 
star_graph(unsigned int vs, struct graph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	graph_init(g, vs);
	for (i = 1; i < vs; i++)
		graph_add_edge(g, vertices[0], vertices[i]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a random k-regular graph on vs vertices
 * (not necessarily simple). 
 * The graph is simple with probability only about e^(-k^2/4),
 * which is tiny when k = 14.
 * vs the number of vertices in the graph
 * k degree of each vertex.
 */
void 
regular_graph(unsigned int vs, unsigned int k, struct graph *g)
{
	unsigned int *vertices, i, j;
	
	if ((vs * k) % 2 != 0)
		errmsg_exit("Number of vertices * k must be even.\n");
	
	vertices = (unsigned int *)algmalloc((vs * k) * sizeof(int));
	
	/* create k copies of each vertex */
	for (i = 0; i < vs; i++)
		for (j = 0; j < k; j++)
			vertices[i + vs * j] = i;
	shuffle_uint_array(vertices, vs * k);
	
	graph_init(g, vs);
	
	/* pick a random perfect matching */
	for (i = 0; i < vs * k / 2; i++)
		graph_add_edge(g, vertices[2 * i], vertices[2 * i + 1]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a random tree on vs vertices.
 * This algorithm uses a Prufer sequence and takes time proportional to vs log
 * vs. 
 * Cayley's theorem: there are vs^(vs-2) labeled trees on vs vertices.
 * Prufer sequence: sequence of vs-2 values between 0 and vs-1 Prufer's proof
 * of Cayley's theorem: Prufer sequences are in 1-1 with labeled trees on vs
 * vertices.
 */
void 
tree_graph(unsigned int vs, struct graph *g)
{
	unsigned int *prufer, *degree, i;
	unsigned int *v, *w;
	struct pairing_heap pq;
	
	if (vs == 1) {
		graph_init(g, 1);
		return;
	}
	
	prufer = (unsigned int *)algmalloc((vs - 2) * sizeof(int));
	degree = (unsigned int *)algmalloc(vs * sizeof(int));
	
	/* sequence of vs-2 values between 0 and vs-1 */
	for (i = 0; i < vs - 2; i++)
		prufer[i] = rand_range_integer(0, vs);
	
	/* 
	 * degree of vertex i = 1 + number of times it appears in
	 * Prufer sequence.
	 */
	for (i = 0; i < vs; i++)
		degree[i] = 1;	/* default degree for every vertex */
	for (i = 0; i < vs - 2; i++)
		degree[prufer[i]]++;
	
	/* Minimum root heap contains all vertices of degree 1. */
	pheap_init(&pq, sizeof(long), vcmp);
	for(i = 0; i < vs; i++)
		if(degree[i] == 1)
			pheap_insert(&pq, &i);
		
	/* repeatedly delete degree 1 vertex that has the minimum index. */
	graph_init(g, vs);
	for(i = 0; i < vs - 2; i++) {
		v = (unsigned int *)pheap_delete(&pq);
		graph_add_edge(g, *v, prufer[i]);
		degree[*v]--;
		degree[prufer[i]]--;
		if(degree[prufer[i]] == 1)
			pheap_insert(&pq, &prufer[i]);
	}
	v = (unsigned int *)pheap_delete(&pq);
	w = (unsigned int *)pheap_delete(&pq);
	graph_add_edge(g, *v, *w);
	
	ALGFREE(prufer);
	ALGFREE(degree);
	pheap_clear(&pq);
}
