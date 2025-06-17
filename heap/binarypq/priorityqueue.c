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
#include "priorityqueue.h"

static void pqueue_exch(struct priority_queue *, int, int);
static inline void swim(struct priority_queue *, unsigned int);
static inline void sink(struct priority_queue *, unsigned int);

/* 
 * Initializes an empty priority queue with the given 
 * capacity and key-size, using the given compare
 * function. 
 */
void 
pqueue_init(struct priority_queue *pq, unsigned long cap, unsigned int ksize,
	algcomp_ft *cmp)
{
	unsigned long i;

	if (ksize == 0) {
		errmsg_exit("The key size of the binary priority queue "
			"not equal 0.\n");
	}

	pq->keys = algmalloc(cap * ksize);
	for (i = 0; i < cap; i++)
		pq->keys[i] = algmalloc(ksize);
		
	pq->keysize = ksize;
	pq->size = 0;
	pq->capacity = cap;
	pq->cmp = cmp;
}

/* Adds a new key to this priority queue. */
void 
pqueue_insert(struct priority_queue *pq, const void *key)
{
	memcpy(pq->keys[pq->size], key, pq->keysize);
	swim(pq, pq->size);
	pq->size++;
}

/* 
 * Removes and returns a smallest or largest key 
 * on this priority queue. 
 */
void * 
pqueue_delete(struct priority_queue *pq)
{
	void *key;

	key = algmalloc(pq->keysize);
	memcpy(key, pq->keys[0], pq->keysize);

	pqueue_exch(pq, 0, --pq->size);
	sink(pq, 0);
	memset(pq->keys[pq->size], 0, pq->keysize);
	
	return key;
}

/* Clears this priority queue. */
void 
pqueue_clear(struct priority_queue *pq)
{
	unsigned long i;
	
	if (pq->keysize != 0)
		for (i = 0; i < pq->capacity; i++) 
			pq->keys[i] = NULL;

	ALGFREE(pq->keys);
	pq->size = 0;
	pq->capacity = 0;
}

/******************** static function boundary ********************/

static void 
pqueue_exch(struct priority_queue *pq, int i, int j)
{
	void *swap;
	
	swap = pq->keys[i];
	pq->keys[i] = pq->keys[j];
	pq->keys[j] = swap;
}

static inline void 
swim(struct priority_queue *pq, unsigned int k)
{
	while (k > 0 && pq->cmp(pq->keys[k/2], pq->keys[k])) {
		pqueue_exch(pq, k/2, k);
		k /= 2;
	}
}

static inline void 
sink(struct priority_queue *pq, unsigned int k)
{
	unsigned int j;
	
	j = 2 * k + 1;	/* k start with 0 */
	while (j < pq->size) {
		/* 
		 * find smallest or largest key among children node.
		 */
		if (j < pq->size - 1 && pq->cmp(pq->keys[j], pq->keys[j + 1]))
			j++;
		
		/* 
		 * occurs break if parent node not less or greater
		 * children node.
		 */
		if (!pq->cmp(pq->keys[k], pq->keys[j]))
			break;
		
		pqueue_exch(pq, j, k);
		k = j;
		j = 2 * k + 1;
	}
}
