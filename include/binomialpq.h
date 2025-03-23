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
#ifndef _BINOMIALPQ_H_
#define _BINOMIALPQ_H_

#include "algcomm.h"

struct binomial_node {
	void *key;				/* key contained by the node */
	unsigned int degree;	/* the degree of the Binomial Tree rooted by
							   this	node */
	struct binomial_node *child;	/* child of this node */
	struct binomial_node *sibling;	/* sibling of this node */
};

struct binomialpq {
	struct binomial_node *head;	/* head of the list of roots */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;			/* comparator over the keys */
};

/* Whether the priority queue is empty. */
#define BINOMPQ_ISEMPTY(pq)	((pq)->head == NULL)

struct single_list;

/* 
 * Initializes an empty priority queue using the given compare function
 * and key-size. 
 */
void binompq_init(struct binomialpq *pq, unsigned int ksize, algcomp_ft *cmp);

/* Number of elements currently on the priority queue. */
unsigned long binompq_size(const struct binomialpq *pq);

/* 
 * Merges two Binomial heaps together, 
 * this operation is destructive. 
 */
struct binomialpq * binompq_union(struct binomialpq *spq,
								const struct binomialpq *tpq);

/* Puts a Key in the heap. */
void binompq_insert(struct binomialpq *pq, const void *key);

/* 
 * Gets the minimum or maximum key currently 
 * in the binomial heap. 
 */
void * binompq_get(const struct binomialpq *pq);

/* Deletes the minimum key. */
void * binompq_delete(struct binomialpq *pq);

/* Gets all keys from this binomial heap. */
void binompq_keys(const struct binomialpq *pq, struct single_list *keys);

/* Clears this binomial heap. */
void binompq_clear(struct binomialpq *pq);

#endif /* _BINOMIALPQ_H_ */
