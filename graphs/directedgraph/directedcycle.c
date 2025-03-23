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
#include "directedcycle.h"
#include "linearlist.h"

/* 
 * Gets a directed cycle if the digraph has a 
 * directed cycle, and null otherwise. 
 */
void 
directed_cycle_get(const struct digraph *g, struct stack *cycle)
{
	int *indegrees;
	unsigned int v, *w, *x;
	long *edgeto, root;
	bool *visited;
	struct queue qu;
	struct single_list *slist;
	struct slist_node *nptr;

	/* indegrees of remaining vertices */
	indegrees = (int *)algcalloc(DIGRAPH_VERTICES(g), sizeof(int));
	for(v = 0; v < DIGRAPH_VERTICES(g); v++)
		indegrees[v] = DIGRAPH_INDEGREE(g, v);

	/* 
	 * Initialize queue to contain all 
	 * vertices with indegree = 0.
	 */
	QUEUE_INIT(&qu, sizeof(int));
	for(v = 0; v < DIGRAPH_VERTICES(g); v++)
		if(indegrees[v] == 0)
			enqueue(&qu, &v);
	
	w = (unsigned int *)algmalloc(sizeof(int));
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&w);
		slist = DIGRAPH_ADJLIST(g, *w);
		SLIST_FOREACH(slist, nptr, unsigned int, x) {
			indegrees[*x]--;
			if(indegrees[*x] == 0)
				enqueue(&qu, x);
		}
	}
	ALGFREE(w);
	
	/* 
	 * There is a directed cycle in subgraph of
	 * vertices with indegree >= 1. 
	 */
	edgeto = (long *)algcalloc(DIGRAPH_VERTICES(g), sizeof(long));
	for(v = 0; v < DIGRAPH_VERTICES(g); v++)
		edgeto[v] = -1;

	root = -1; /* any vertex with indegree >= -1 */
	for(v = 0; v < DIGRAPH_VERTICES(g); v++) {
		if(indegrees[v] == 0)
			continue;
		else
			root = v;

		slist = DIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(slist, nptr, unsigned int, w) {
			if(indegrees[*w] > 0)
				edgeto[*w] = v;
		}
	}
	
	/* initialize empty stack */
	STACK_INIT(cycle, sizeof(int));

	if(root != -1) {
		visited = (bool *)algcalloc(DIGRAPH_VERTICES(g), sizeof(bool));
		for(v = 0; v < DIGRAPH_VERTICES(g); v++)
			visited[v] = false;
		
		/* find any vertex on cycle */
		while(root != -1 && !visited[root]) {
			visited[root] = true;
			root = edgeto[root];
		}

		/* extract cycle */
		v = root;
		do {
			stack_push(cycle, &v);
			v = edgeto[v];
		} while(v != root);
		stack_push(cycle, &root);
	}
}
