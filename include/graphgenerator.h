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
#ifndef _GRAPHGENERATOR_H_
#define _GRAPHGENERATOR_H_

#include "graph.h"

/* 
 * Generates the complete graph on vs vertice. 
 * parameter vs is the number of vertices.
 */
#define complete_graph(vs, g)	simple_graph2(vs, 1.0, g)

/* 
 * Generates a random simple graph containing vs vertices 
 * and es edges, output a graph g.
 * parameter vs is the number of vertices, 
 * es is the number of edges. 
 */
void simple_graph1(unsigned int vs, unsigned int es, struct graph *g);


/* 
 * Generates a random simple graph on vs vertices, 
 * with an edge between any two vertices with probability p.
 * This is sometimes referred to as the Erdos-Renyi random
 * graph model. parameter vs is the number of vertices, 
 * p is the probability of choosing an edge. 
 */
void simple_graph2(unsigned int vs, double p, struct graph *g);


/* 
 * Generates a random simple bipartite graph on 
 * vs1 and vs2 vertices with es edges.
 * parameter vs1 the number of vertices in one partition.
 * parameter vs2 the number of vertices in 
 * the other partition.
 * parameter es the number of edges. 
 */
void bipartite_graph1(unsigned int vs1, unsigned int vs2, unsigned int es,
					struct graph *g);


/* 
 * Generates a random simple bipartite graph on 
 * vs1 and vs2 vertices, 
 * containing each possible edge with probability p.
 * parameter vs1 the number of vertices in one partition.
 * parameter vs2 the number of vertices in 
 * the other partition.
 * parameter p the probability that the graph contains
 * an edge with one endpoint in either side.
 */
void bipartite_graph2(unsigned int vs1, unsigned int vs2, double p,
					struct graph *g);


/* 
 * Generates a path graph on vs vertices. 
 * vs the number of vertices in the path. 
 */
void path_graph(unsigned int vs, struct graph *g);


/* 
 * Generates a complete binary tree graph on vs vertices.
 * vs the number of vertices in the binary tree. 
 */
void binary_tree_graph(unsigned int vs, struct graph *g);


/* Generates a cycle graph on vs vertices. 
   vs the number of vertices in the cycle. */
void cycle_graph(unsigned int vs, struct graph *g);


/* 
 * Generates an Eulerian cycle graph on vs vertices.
 * vs the number of vertices in the cycle,
 * es the number of edges in the cycle.
 */
void eulerian_cycle_graph(unsigned int vs, unsigned int es,	struct graph *g);


/* 
 * Generates an Eulerian path graph on vs vertices.
 * vs the number of vertices in the path,
 * es the number of edges in the path.
 */
void eulerian_path_graph(unsigned int vs, unsigned int es, struct graph *g);


/* 
 * Generates a wheel graph on vs vertices.
 * vs the number of vertices in the wheel.
 */
void wheel_graph(unsigned int vs, struct graph *g);


/* 
 * Generates a star graph on vs vertices.
 * vs the number of vertices in the star.
 */
void star_graph(unsigned int vs, struct graph *g);


/* 
 * Generates a random k-regular graph on vs vertices 
 * (not necessarily simple). 
 * The graph is simple with probability 
 * only about e^(-k^2/4), which is tiny when k = 14.
 * vs the number of vertices in the graph,
 * k degree of each vertex.
 */
void regular_graph(unsigned int vs, unsigned int k, struct graph *g);


/* 
 * Generates a random tree on vs vertices.
 * This algorithm uses a Prufer sequence and takes time
 * proportional to vs log vs. 
 */
void tree_graph(unsigned int vs, struct graph *g);

#endif	/* _GRAPHGENERATOR_H_ */
