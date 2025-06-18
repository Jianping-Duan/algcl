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
#ifndef _EWDIGRAPHDFSO_H_ 
#define _EWDIGRAPHDFSO_H_

#include "edgeweighteddigraph.h"

struct single_list;

struct ewdigraph_dfso {
	bool *marked;		/* marked[v] = has v been marked in dfs? */
	unsigned int *pre;	/* pre[v] = preorder number of v */
	unsigned int *post;	/* post[v] = postorder number of v */
	struct single_list *preorder;	/* vertices in preorder */
	struct single_list *postorder;	/* vertices in postorder */
	unsigned int precounter;	/* counter for preorder numbering */
	unsigned int postcounter;	/* counter for postorder numbering */
	unsigned int vertices;		/* digraph of vertices */
};

/* Returns the preorder number of vertex */
#define EWDIGRAPH_DFSO_PRE(dfs, v)	\
	((v) >= (dfs)->vertices ? 0 : (dfs)->pre[v])

/* Returns the postorder number of vertex */
#define EWDIGRAPH_DFSO_POST(dfs, v)	\
	((v) >= (dfs)->vertices ? 0 : (dfs)->post[v])

/* Returns the vertices set in postorder */
#define EWDIGRAPH_DFSO_POSTORDER(dfs)	((dfs)->postorder)

/* Returns the vertices set in preorder */
#define EWDIGRAPH_DFSO_PREORDER(dfs)	((dfs)->preorder)

/* Returns the vertices in reverse postorder */
#define EWDIGRAPH_DFSO_REVERSEPOST(dfs, order)	do {	\
	slist_clone((dfs)->postorder, order);		\
	slist_reverse(order);				\
} while (0)

#define EWDIGRAPH_DFSO_CLEAR(dfs)	do {	\
	ALGFREE((dfs)->marked);			\
	ALGFREE((dfs)->pre);			\
	ALGFREE((dfs)->post);			\
	slist_clear((dfs)->preorder);		\
	ALGFREE((dfs)->preorder);		\
	slist_clear((dfs)->postorder);		\
	ALGFREE((dfs)->postorder);		\
	(dfs)->precounter = 0;			\
	(dfs)->postcounter = 0;			\
	(dfs)->vertices = 0;			\
} while (0)


/* Determines a depth-first order for the ewdigraph */
void ewdigraph_dfso_init(struct ewdigraph_dfso *dfs, const struct ewdigraph *g);

#endif	/* _EWDIGRAPHDFSO_H_ */
