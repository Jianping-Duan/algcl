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
#ifndef _INDEXBINOMPQ_H_
#define _INDEXBINOMPQ_H_

#include "algcomm.h"

struct index_binom_node {
	void *key;	/* key contained by the node */
	unsigned long index;	/* Index associated  with the Key */
	unsigned int degree;	/* the degree of the Binomial Tree rooted by this
							   node */
	struct index_binom_node *parent;	/* parent of this node */
	struct index_binom_node *child;		/* child of this node */
	struct index_binom_node *sibling;	/* sibling of this node */
};

struct index_binompq {
	struct index_binom_node *head;	/* head of the list of roots */
	unsigned int keysize;			/* the bytes of the key */
	algcomp_ft *cmp;				/* comparator over the keys */
	unsigned long capacity;			/* maximum size of the tree */
	struct index_binom_node **nodes; /* array of indexed Nodes of the heap */
};

/* Whether the indexed binomial priority queue is empty. */
#define IBINOMPQ_ISEMPTY(pq)	((pq)->head == NULL)

/* 
 * Does the indexed binomial priority queue 
 * contains the index i? worst case is O(1). 
 */
#define IBINOMPQ_CONTAINS(pq, i)	\
	(((i) < (pq)->capacity && ((pq)->nodes[i] != NULL)) ? 1 : 0)

/* 
 * Gets the key associated with index i, 
 * worst case is O(1) 
 */
#define IBINOMPQ_KEYOF(pq, i)	\
	(((i) >= 0 && (i) < (pq)->capacity && ibinompq_contains(pq, i)) \
		? pq->nodes[i] : NULL)

struct queue;

/* 
 * Initializes an empty indexed binomial priority queue 
 * with indicates between 0 and n - 1. 
 */
void ibinompq_init(struct index_binompq *pq, unsigned long n,
				unsigned int ksize,	algcomp_ft *cmp);

/* 
 * Number of elements currently on 
 * the indexed binomial priority queue.
 */
unsigned long ibinompq_size(const struct index_binompq *pq);

/* Associates a key with an index. */
int ibinompq_insert(struct index_binompq *pq, unsigned long i, const void *key);

/* 
 * Gets the minimum or maximum key currently in 
 * the binomial priority queue.
 */
void * ibinompq_get_key(const struct index_binompq *pq);

/* 
 * Gets the index associated with 
 * the minimum or maximum key. 
 */
unsigned long ibinompq_get_index(const struct index_binompq *pq);

/* Traverses this indexed binomial heap. */
void ibinompq_traverse(const struct index_binompq *pq, struct queue *keys,
						struct queue *indexes);

/* Clears this indexed binomial heap. */
void ibinompq_clear(struct index_binompq *pq);

/* 
 * Deletes the minimum or maximum key 
 * and returns its index. 
 */
unsigned long ibinompq_delete(struct index_binompq *pq);

/* Deletes the key associated the given index. */
int ibinompq_remove(struct index_binompq *pq, unsigned long i);

/* 
 * Decreases the key associated with 
 * index i to the given key. 
 */
int ibinompq_decrkey(struct index_binompq *pq, unsigned long i,
					const void *key);

/* 
 * Increases the key associated with 
 * index i to the given key. 
 */
int ibinompq_incrkey(struct index_binompq *pq, unsigned long i,
					const void *key);

/* 
 * Changes the key associated with 
 * index i to the given key. 
 */
int ibinompq_change(struct index_binompq *pq, unsigned long i, const void *key);

#endif	/* _INDEXBINOMPQ_H_ */
