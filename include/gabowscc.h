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
#ifndef _GABOWSCC_H_
#define _GABOWSCC_H_

#include "digraph.h"

struct gabow_scc {
	bool *marked;	/* marked[v] = has v been visited? */
	long *id;	/* id[v] = id of strong component containing v */
	unsigned int *preorder;		/* preorder[v] = preorder of v */
	unsigned int precounter;	/* preorder number counter */
	unsigned int count;		/* number of strongly-connected components */
	unsigned int vertices;	/* digraph of vertices */
};

/* Returns the number of strong components. */
#define GBSCC_COUNT(scc)	((scc)->count)

/* 
 * Returns the component id of the strong component 
 * containing vertex.
 */
#define GBSCC_ID(scc, v)	\
	((v) > (scc)->vertices ? (-1) : (scc)->id[v])

/* Are vertices v and w in the same strong component? */
#define GBSCC_STRONGLY_CONNECTED(scc, v, w)	\
	((v) < (scc)->vertices && (w) < (scc)->vertices ? \
	(scc)->id[v] == (scc)->id[w] : false)

#define GBSCC_CLEAR(scc)	do {	\
	ALGFREE((scc)->marked);			\
	ALGFREE((scc)->id);				\
	ALGFREE((scc)->preorder);		\
	(scc)->precounter = 0;			\
	(scc)->count = 0;				\
	(scc)->vertices = 0;			\
} while(0)

/* Computes the strong components of the digraph g */
void gbscc_init(struct gabow_scc *scc, const struct digraph *g);

#endif	/* _GABOWSCC_H_ */
