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
#include "digraphgenerator.h"
#include "avltree.h"
#include "skiplist.h"
#include <math.h>

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
 * Generates a random simple digraph containing vs vertices and es edges,
 * output a digraph g.
 */
void 
simple_digraph1(unsigned int vs, unsigned int es, struct digraph *g)
{
	struct avl_tree bst;
	unsigned int v, w;
	long key;
	char skey[21];
	
	if (es > vs * (vs - 1) / 2)
		errmsg_exit("Too many edges.\n");
	
	if (es < vs - 1)
		errmsg_exit("Too few edges.\n");
	
	avlbst_init(&bst, sizeof(long), vcmp);
	digraph_init(g, vs);
	while (DIGRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0 ,vs);
		
		sprintf(skey, "%u%u", v, w);
		sscanf(skey, "%ld", &key);
		if (v != w && avlbst_get(&bst, &key) == NULL) {
			avlbst_put(&bst, &key);
			digraph_add_edge(g, v, w);
		}
	}
	avlbst_clear(&bst);
}

/* 
 * Generates a random simple digraph on vs vertices, with an edge between any
 * two vertices with probability p. 
 * This is sometimes referred to as the Erdos-Renyi random digraph model.
 */
void 
simple_digraph2(unsigned int vs, double p, struct digraph *g)
{
	unsigned int v, w;
	
	if (p < 0.0 || p > 1.0)
		errmsg_exit("Probability must be between 0.0 and 1.0.\n");
	
	digraph_init(g, vs);
	for (v = 0; v < vs; v++)
		for (w = v + 1; w < vs; w++)
			if (bernoulli_distribution(p))
				digraph_add_edge(g, v, w);
}

/* 
 * Generates the complete digraph on vs vertices.
 * In a complete digraph, every pair of distinct vertices is connected by two
 * antiparallel edges. There are vs*(vs-1) edges.
 */
void
complete_digraph(unsigned int vs, struct digraph *g)
{
	unsigned int v, w;
	
	digraph_init(g, vs);
	for (v = 0; v < vs; v++)
		for (w = 0; w < vs; w++)
			if (v != w)
				digraph_add_edge(g, v, w);
}

/* Generates a random simple DAG containing vs vertices and es edges. */
void 
simple_acyclic_digraph(unsigned int vs, unsigned int es, struct digraph *g)
{
	unsigned int v, w, *vertices, i;
	struct skip_list sl;
	long key;
	char skey[21];
	
	if (es > vs * (vs - 1) / 2)
		errmsg_exit("Too many edges.\n");
	
	if (es < vs - 1) 
		errmsg_exit("Too few edges.\n");
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	skipl_init(&sl, (int)ceil(log2((double)vs)), sizeof(long), vcmp);
	digraph_init(g, vs);
	while (DIGRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0 ,vs);
		
		sprintf(skey, "%u%u", v, w);
		sscanf(skey, "%ld", &key);
		if (v < w && skipl_get(&sl, &key) == NULL) {
			skipl_put(&sl, &key);
			digraph_add_edge(g, vertices[v], vertices[w]);
		}
	}
	skipl_clear(&sl);
	ALGFREE(vertices);
}

/* 
 * Generates a random tournament digraph on vs vertices.
 * A tournament digraph is a digraph in which, for every pair of vertices,
 * there is one and only one directed edge connecting them. 
 * A tournament is an oriented complete graph.
 */
void 
tournament_digraph(unsigned int vs, struct digraph *g)
{
	unsigned int v, w;
	
	digraph_init(g, vs);
	for (v = 0; v < vs; v++)
		for (w = v + 1; w < vs; w++)
			if (bernoulli_distribution(0.5))
				digraph_add_edge(g, v, w);
			else
				digraph_add_edge(g, w, v);
}

/* 
 * Generates a complete rooted-in DAG on vs vertices.
 * A rooted-in tree is a DAG in which there is a single vertex reachable from
 * every other vertex. A complete rooted-in DAG has V*(V-1)/2 edges.
 */
void 
complete_rootedin_dag(unsigned int vs, struct digraph *g)
{
	unsigned int i, j, *vertices;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	digraph_init(g, vs);
	for (i = 0; i < vs; i++)
		for (j = i + 1; j < vs; j++)
			digraph_add_edge(g, vertices[i], vertices[j]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a random rooted-in DAG on vs vertices and es edges.
 * A rooted-in tree is a DAG in which there is a single vertex reachable from
 * every other vertex.
 */
void
rootedin_dag(unsigned int vs, unsigned int es, struct digraph *g)
{
	unsigned int v, w, *vertices, i;
	struct avl_tree bst;
	long key;
	char skey[21];
	
	if (es > vs * (vs - 1) / 2)
		errmsg_exit("Too many edges.\n");
	
	if (es < vs - 1) 
		errmsg_exit("Too few edges.\n");
	
	/* fix a topological order */
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	/* 
	 * One edge pointing from each vertex,
	 * other than the root = vertices[vs-1].
	 */
	avlbst_init(&bst, sizeof(long), vcmp);
	digraph_init(g, vs);
	for (v = 0; v < vs - 1; v++) {
		w = rand_range_integer(v + 1, vs);
		
		sprintf(skey, "%u%u", v, w);
		sscanf(skey, "%ld", &key);
		avlbst_put(&bst, &key);
		digraph_add_edge(g, v, w);
	}
	
	while (DIGRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		
		sprintf(skey, "%u%u", v, w);
		sscanf(skey, "%ld", &key);
		if (v < w && avlbst_get(&bst, &key) == NULL) {
			avlbst_put(&bst, &key);
			digraph_add_edge(g, vertices[v], vertices[w]);
		}
	}
	avlbst_clear(&bst);
	ALGFREE(vertices);
}

/* 
 * Generates a complete rooted-out DAG on vs vertices. 
 * A rooted-out tree is a DAG in which every vertex is reachable from a single
 * vertex. A complete rooted-out DAG has V*(V-1)/2 edges.
 */
void 
complete_rootedout_dag(unsigned int vs, struct digraph *g)
{
	unsigned int i, j, *vertices;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	digraph_init(g, vs);
	for (i = 0; i < vs; i++)
		for (j = i + 1; j < vs; j++)
			digraph_add_edge(g, vertices[j], vertices[i]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a random rooted-out DAG on vs vertices and es edges.
 * A rooted-out tree is a DAG in which every vertex is reachable from a single
 * vertex.
 */
void
rootedout_dag(unsigned int vs, unsigned int es, struct digraph *g)
{
	unsigned int v, w, *vertices, i;
	struct skip_list sl;
	long key;
	char skey[21];
	
	if (es > vs * (vs - 1) / 2)
		errmsg_exit("Too many edges.\n");
	
	if (es < vs - 1)
		errmsg_exit("Too few edges.\n");
	
	/* fix a topological order */
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	/* 
	 * One edge pointing from each vertex,
	 * other than the root = vertices[vs-1].
	 */
	skipl_init(&sl, (int)ceil(log2((double)vs)), sizeof(long), vcmp);
	digraph_init(g, vs);
	for (v = 0; v < vs - 1; v++) {
		w = rand_range_integer(v + 1, vs);
		
		sprintf(skey, "%u%u", w, v);
		sscanf(skey, "%ld", &key);
		skipl_put(&sl, &key);
		digraph_add_edge(g, w, v);
	}
	
	while (DIGRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		
		sprintf(skey, "%u%u", w, v);
		sscanf(skey, "%ld", &key);
		if (v < w && skipl_get(&sl, &key) == NULL) {
			skipl_put(&sl, &key);
			digraph_add_edge(g, vertices[w], vertices[v]);
		}
	}
	
	skipl_clear(&sl);
	ALGFREE(vertices);
}

/* Generates a path digraph on vs vertices. */
void 
path_digraph(unsigned int vs, struct digraph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	digraph_init(g, vs);
	for (i = 0; i < vs - 1; i++)
		digraph_add_edge(g, vertices[i], vertices[i + 1]);
	
	ALGFREE(vertices);
}

/* 
 * Generates a complete binary tree digraph on vs vertices.
 * vs the number of vertices in the binary tree.
 */
void 
binary_tree_digraph(unsigned int vs, struct digraph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	digraph_init(g, vs);
	for (i = 1; i < vs; i++)
		digraph_add_edge(g, vertices[i], vertices[(i - 1)/2]);
	
	ALGFREE(vertices);
}

/* Generates a cycle digraph on vs vertices. */
void 
cycle_digraph(unsigned int vs, struct digraph *g)
{
	unsigned int *vertices, i;
	
	vertices = (unsigned int *)algmalloc(vs * sizeof(int));
	for (i = 0; i < vs; i++)
		vertices[i] = i;
	shuffle_uint_array(vertices, vs);
	
	digraph_init(g, vs);
	for (i = 0; i < vs - 1; i++)
		digraph_add_edge(g, vertices[i], vertices[i + 1]);
	digraph_add_edge(g, vertices[vs - 1], vertices[0]);
	
	ALGFREE(vertices);
}

/* Generates an Eulerian cycle digraph on vs vertices. */
void 
eulerian_cycle_digraph(unsigned int vs, unsigned int es, struct digraph *g)
{
	unsigned int *vertices, i, v, cnt;
	struct avl_tree bst;
	
	vertices = (unsigned int *)algmalloc(es * sizeof(int));
	
	avlbst_init(&bst, sizeof(long), vcmp);
	for (i = 0, cnt = 0; i < es; i++) {
		v = rand_range_integer(0, vs);
		
		if (avlbst_get(&bst, &v) == NULL) {
			avlbst_put(&bst, &v);
			vertices[cnt++] = v;
		}
	}
	avlbst_clear(&bst);
	
	digraph_init(g, vs);
	for (i = 0; i < cnt - 1; i++)
		digraph_add_edge(g, vertices[i], vertices[i + 1]);
	digraph_add_edge(g, vertices[cnt - 1], vertices[0]);
	
	ALGFREE(vertices);
}

/* Returns an Eulerian path digraph on vs vertices. */
void 
eulerian_path_digraph(unsigned int vs, unsigned int es, struct digraph *g)
{
	unsigned int *vertices, i, v, cnt;
	struct skip_list sl;
	
	vertices = (unsigned int *)algmalloc((es + 1) * sizeof(int));
	
	skipl_init(&sl, (int)ceil(log2((double)vs)), sizeof(long), vcmp);
	for (i = 0, cnt = 0; i < es + 1; i++) {
		v = rand_range_integer(0, vs);
		
		if (skipl_get(&sl, &v) == NULL) {
			skipl_put(&sl, &v);
			vertices[cnt++] = v;
		}
	}
	skipl_clear(&sl);
	
	digraph_init(g, vs);
	for (i = 0; i < cnt - 1; i++)
		digraph_add_edge(g, vertices[i], vertices[i + 1]);
	
	ALGFREE(vertices);
}

/* 
 * Returns a random simple digraph on vs vertices, es edges and (at least) cs
 * strong components. 
 *
 * The vertices are randomly assigned integer labels between 0 and cs - 1
 * (corresponding to strong components).
 * Then, a strong component is creates among the vertices with the same label.
 * Next, random edges (either between two vertices with the same labels or from
 * a vertex with a smaller label to a vertex with a larger label).
 * The number of components will be equal to the number of distinct labels that
 * are assigned to vertices.
 */
void 
strong_component_digraph(unsigned int vs, unsigned int es, unsigned int cs,
			struct digraph *g)
{
	unsigned int *label, *vertices, v, w, i, j;
	long cnt = 0;
	struct avl_tree bst;
	long key;
	char skey[21];
	
	if (cs >= vs)
		errmsg_exit("Number of components must be between 1 and vs.\n");
	
	if (es <= 2 * (vs - cs))
		errmsg_exit("Number of edges must be at least 2(vs - cs)\n");
	
	if (es > vs * (vs - 1) / 2)
		errmsg_exit("Too many edges.\n");
	
	label = (unsigned int *)algcalloc(vs, sizeof(int));
	for (v = 0; v < vs; v++)
		label[v] = rand_range_integer(0, cs);
	
	/* 
	 * Make all vertices with label cs a strong component 
	 * by combining a rooted in-tree and a rooted out-tree.
	 */
	digraph_init(g, vs);
	avlbst_init(&bst, sizeof(long), vcmp);
	for (i = 0; i < cs; i++) {
		/* how many vertices in component c */
		for (v = 0, cnt = 0; v < DIGRAPH_VERTICES(g); v++)
			if (label[v] == i)
				cnt++;
			
		vertices = (unsigned int *)algcalloc(cnt, sizeof(int));
		for (v = 0, j = 0; v < vs && j < cnt; v++, j++)
			if (label[v] == i)
				vertices[j] = v;
		shuffle_uint_array(vertices, cnt);
		
		/* rooted-in tree with root = vertices[cnt-1] */
		for (v = 0; v < cnt - 1; v++) {
			w = rand_range_integer(v + 1, vs);
			
			sprintf(skey, "%u%u", w, v);
			sscanf(skey, "%ld", &key);
			avlbst_put(&bst, &key);
			digraph_add_edge(g, w, v);
		}
		
		/* rooted-out tree with root = vertices[cnt-1] */
		for (v = 0; v < cnt - 1; v++) {
			w = rand_range_integer(v + 1, vs);
			
			sprintf(skey, "%u%u", v, w);
			sscanf(skey, "%ld", &key);
			avlbst_put(&bst, &key);
			digraph_add_edge(g, v, w);
		}
		
		ALGFREE(vertices);
	}

	while (DIGRAPH_EDGES(g) < es) {
		v = rand_range_integer(0, vs);
		w = rand_range_integer(0, vs);
		
		sprintf(skey, "%u%u", v, w);
		sscanf(skey, "%ld", &key);
	
		if (avlbst_get(&bst, &key) == NULL && v != w &&
			label[v] <= label[w]) {
			avlbst_put(&bst, &key);
			digraph_add_edge(g, v, w);
		}
	}
	
	avlbst_clear(&bst);
	ALGFREE(label);
}
