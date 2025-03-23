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
#ifndef _DIGRAPHGENERATOR_H_
#define _DIGRAPHGENERATOR_H_

#include "digraph.h"

/* 
 * Generates a random simple digraph containing
 * vs vertices and es edges, output a digraph g. 
 * parameter vs is the number of vertices, 
 * es is the number of edges.
 */
void simple_digraph1(unsigned int vs, unsigned int es, struct digraph *g);

/* Generates a random simple digraph on vs vertices, with
 * an edge between any two vertices with probability p.
 * This is sometimes referred to as the Erdos-Renyi random
 * digraph model. Parameter vs is the number of vertices,
 * p is the probability of choosing an edge.
 */
void simple_digraph2(unsigned int vs, double p,	struct digraph *g);

/* 
 * Generates the complete digraph on vs vertices.
 * In a complete digraph, every pair of distinct 
 * vertices is connected by two antiparallel edges. 
 * There are vs*(vs-1) edges.
 */
void complete_digraph(unsigned int vs, struct digraph *g);

/* 
 * Generates a random simple DAG containing 
 * vs vertices and es edges. 
 */
void simple_acyclic_digraph(unsigned int vs, unsigned int es,
							struct digraph *g);

/* 
 * Generates a random tournament digraph on vs vertices.
 * A tournament digraph is a digraph in which, 
 * for every pair of vertices, there is one and only one
 * directed edge connecting them. 
 * A tournament is an oriented complete graph. 
 */
void tournament_digraph(unsigned int vs, struct digraph *g);

/* 
 * Generates a complete rooted-in DAG on vs vertices. 
 * A rooted-in tree is a DAG in which there is a single 
 * vertex reachable from every other vertex. 
 * A complete rooted-in DAG has V*(V-1)/2 edges.
 */
void complete_rootedin_dag(unsigned int vs, struct digraph *g);

/* 
 * Generates a random rooted-in DAG on 
 * vs vertices and es edges.
 * A rooted-in tree is a DAG in which there is a single
 * vertex reachable from every other vertex. 
 */
void rootedin_dag(unsigned int vs, unsigned int es, struct digraph *g);

/* 
 * Generates a complete rooted-out DAG on vs vertices.
 * A rooted-out tree is a DAG in which every vertex is
 * reachable from a single vertex. 
 * A complete rooted-out DAG has V*(V-1)/2 edges.
 */
void complete_rootedout_dag(unsigned int vs, struct digraph *g);

/* 
 * Generates a random rooted-out DAG on
 * vs vertices and es edges.
 * A rooted-out tree is a DAG in which every vertex 
 * is reachable from a single vertex.
 */
void rootedout_dag(unsigned int vs, unsigned int es, struct digraph *g);

/* 
 * Generates a random rooted-in tree on vs vertices.
 * A rooted-in tree is an oriented tree in which 
 * there is a single vertex reachable from every
 * other vertex.
 */
#define ROOTEDIN_TREE(vs, g)	\
	rootedin_dag(vs, vs - 1, g);

/* 
 * Generates a random rooted-out tree on vs vertices. 
 * A rooted-out tree is an oriented tree in which each
 * vertex is reachable from a single vertex.
 * It is also known as a arborescence or branching. 
 */
#define ROOTEDOUT_TREE(vs, g)	\
	rootedout_dag(vs, vs - 1, g);

/* 
 * Generates a path digraph on vs vertices. 
 * vs the number of vertices in the path. 
 */
void path_digraph(unsigned int vs, struct digraph *g);

/* 
 * Generates a complete binary tree digraph on vs vertices.
 * vs the number of vertices in the binary tree.
 */
void binary_tree_digraph(unsigned int vs, struct digraph *g);
/* 
 * Generates a cycle digraph on vs vertices. 
 * vs the number of vertices in the cycle. 
 */
void cycle_digraph(unsigned int vs, struct digraph *g);

/* 
 * Generates an Eulerian cycle digraph on vs vertices.
 * vs the number of vertices in the cycle,
 * es the number of edges in the cycle.
 */
void eulerian_cycle_digraph(unsigned int vs, unsigned int es,
						struct digraph *g);

/* 
 * Generates an Eulerian path digraph on vs vertices.
 * vs the number of vertices in the path,
 * es the number of edges in the path.
 */
void eulerian_path_digraph(unsigned int vs, unsigned int es,
						struct digraph *g);

/* 
 * Generates a random simple digraph on vs vertices,
 * es edges and (at least) cs strong components.
 * The vertices are randomly assigned integer labels
 * between 0 and cs - 1 
 * (corresponding to strong components). 
 * Then, a strong component is creates among the vertices 
 * with the same label. Next, random edges (either between
 * two vertices with the same labels or from a vertex with
 * a smaller label to a vertex with a larger label).
 * The number of components will be equal to the number of
 * distinct labels that are assigned to vertices.
 */
void strong_component_digraph(unsigned int vs, 	unsigned int es,
							unsigned int cs, struct digraph *g);

#endif	/* _DIGRAPHGENERATOR_H_ */
