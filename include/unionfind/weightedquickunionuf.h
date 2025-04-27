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
#ifndef _WEIGHTEDQUICKUNIONUF_H_
#define _WEIGHTEDQUICKUNIONUF_H_

#include "algcomm.h"

/* The union–find data type models a collection of sets containing N elements, 
 * with each element in exactly one set.
 * The elements are named 0 through N–1.
 * Initially, there are N sets, with each element in its own set. 
 * The canonical element of a set (also known as the Root, Identifier, Leader,
 * or Set representative) is one distinguished element in the set. 
 * Here is a summary of the operations:
 *	find(P) returns the canonical element of the set containing P. 
 *	The find operation returns the same value for two elements if and only if
 *	they are in the same set.
 *  union(P, Q) merges the set containing element P with the set containing
 *  element Q. That is, if P and Q are in different sets, replace these two
 *  sets with a new set that is the union of the two.
 *  count() returns the number of sets.
 */

struct wquick_union_uf {
	long *parent;	/* parent[i] = parent of i */
	long *size;		/* size[i] = number of elements in subtree rooted at i */
	unsigned long len;		/* the parent array of length */
	unsigned long count;	/* number of components */
};

/* Returns the number of sets. */
#define WQUUF_COUNT(uf)	((uf)->count)

#define WQUUF_CLEAR(uf)		do {	\
	ALGFREE((uf)->parent);			\
	ALGFREE((uf)->size);			\
	(uf)->len = 0;					\
	(uf)->count = 0;				\
} while (0)


/* 
 * Initializes an empty union-find data structure with N elements 0 through
 * N - 1. Initially, each element is in its own set. 
 */
void wquuf_init(struct wquick_union_uf *uf, unsigned long n);

/* Returns the canonical element of the set containing element p. */
long wquuf_find(const struct wquick_union_uf *uf, long p);

/* Returns true if the two elements are in the same set. */
int wquuf_connected(const struct wquick_union_uf *uf, long p, long q);

/* Merges the set containing element P with the set containing element Q. */
void wquuf_union(struct wquick_union_uf *uf, long p, long q);

#endif	/* _WEIGHTEDQUICKUNIONUF_H_ */ 
