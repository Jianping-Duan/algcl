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
#include "indexpqueue.h"

static void ipqueue_exch(struct index_pqueue *, unsigned long, unsigned long);
static int ipqcmp(const struct index_pqueue *, unsigned long, unsigned long);
static inline void swim(struct index_pqueue *, unsigned int);
static inline void sink(struct index_pqueue *, unsigned int);

/* 
 * Initializes an empty indexed priority queue with
 * the given capacity and key-size, using the given 
 * compare function. 
 */
void 
ipqueue_init(struct index_pqueue *ipq, unsigned long cap, unsigned short ksize,
	algcomp_ft *cmp)
{
	unsigned long i;

	if (ksize == 0) {
		errmsg_exit("The key size of the indexed binary priority queue "
			"not equal 0.\n");
	}

	ipq->capacity = cap;
	ipq->size = 0;
	ipq->keysize = ksize;
	ipq->cmp = cmp;
	
	ipq->pq = (long *)algmalloc(cap * sizeof(long));	
	ipq->qp = (long *)algmalloc(cap * sizeof(long));
	ipq->keys = algmalloc(cap * ksize);

	for (i = 0; i < cap; i++) {
		ipq->keys[i] = algmalloc(ksize);
		ipq->pq[i] = 0;
		ipq->qp[i] = -1;
	}
}

/* 
 * Associates key with index i, i an index, 
 * key the key to associate with index i.
 */
void 
ipqueue_insert(struct index_pqueue *ipq, unsigned long i, const void *key)
{
	assert(i < ipq->capacity);
	assert(!IPQUEUE_CONTAINS(ipq, i));
	
	ipq->pq[ipq->size] = i;
	ipq->qp[i] = ipq->size;
	memcpy(ipq->keys[i], key, ipq->keysize);
	swim(ipq, ipq->size);
	ipq->size++;
}

/* 
 * Removes a minimum or maximum key and returns its associated index,
 * return an index associated with a minimum or maximum key.
 */
unsigned long 
ipqueue_delete(struct index_pqueue *ipq)
{
	unsigned long i;

	assert(!IPQUEUE_ISEMPTY(ipq));
	
	i = ipq->pq[0];
	ipqueue_exch(ipq, 0, --ipq->size);
	sink(ipq, 0);

	/* check index */
	assert(i == (unsigned)ipq->pq[ipq->size]);
	ipq->qp[i] = -1;	/* delete */

	/* clear memory */
	memset(ipq->keys[i], 0, ipq->keysize);
	ipq->pq[ipq->size] = -1;	/* not needed */
	
	return i;
}

/* 
 * Remove the key associated with index, 
 * i the index of the key to remove. 
 */
void 
ipqueue_remove(struct index_pqueue *ipq, unsigned long i)
{
	unsigned long k;
	
	if (!IPQUEUE_CONTAINS(ipq, i))
		return;
	
	k = ipq->qp[i];	/* find the index of the pq array. */
	ipqueue_exch(ipq, k, --ipq->size);
	swim(ipq, k);
	sink(ipq, k);
	memset(ipq->keys[i], 0, ipq->keysize);
	ipq->qp[i] = -1;
}

/* 
 * Change the key associated with index i to 
 * the specified key. 
 */
void 
ipqueue_change(struct index_pqueue *ipq, unsigned long i, const void *key)
{
	assert(i < ipq->capacity);
	assert(IPQUEUE_CONTAINS(ipq, i) && key != NULL);
	
	ipq->keys[i] = (void *)key;
	swim(ipq, ipq->qp[i]);
	sink(ipq, ipq->qp[i]);
}

/* 
 * Decrease the key associated with index i to the specified value. 
 */
void
ipqueue_decrkey(struct index_pqueue *ipq, unsigned long i, const void *key)
{
	if (!IPQUEUE_CONTAINS(ipq, i))
		return;
	
	if (ipq->cmp(key, ipq->keys[i]))
		return;
	
	ipq->keys[i] = (void *)key;
	swim(ipq, ipq->qp[i]);
}

/* 
 * Increase the key associated with index i to the specified value. 
 */
void
ipqueue_incrkey(struct index_pqueue *ipq, unsigned long i, const void *key)
{
	if (!IPQUEUE_CONTAINS(ipq, i))
		return;
	
	if (ipq->cmp(ipq->keys[i], key))
		return;
	
	ipq->keys[i] = (void *)key;
	sink(ipq, ipq->qp[i]);
}

/* Clears this indexed priority queue. */
void 
ipqueue_clear(struct index_pqueue *ipq)
{
	unsigned long i;
	
	for (i = 0; i < ipq->capacity; i++)
		ALGFREE(ipq->keys[i]);
	ALGFREE(ipq->keys);
	ALGFREE(ipq->pq);
	ALGFREE(ipq->qp);
	ipq->size = 0;
	ipq->capacity = 0;
}

/******************** static function boundary ********************/

/* 
 * Exchanges index is i and j on this 
 * indexed priority queue. 
 */
static void 
ipqueue_exch(struct index_pqueue *ipq, unsigned long i, unsigned long j)
{
	long swap;
	
	swap = ipq->pq[i];
	ipq->pq[i] = ipq->pq[j];
	ipq->pq[j] = swap;
	
	/* reset QP array */
	ipq->qp[ipq->pq[i]] = i;
	ipq->qp[ipq->pq[j]] = j;
}

/* transform compare function */
static int 
ipqcmp(const struct index_pqueue *ipq, unsigned long i, unsigned long j)
{
	return ipq->cmp(ipq->keys[ipq->pq[i]], ipq->keys[ipq->pq[j]]);
}

static inline void 
swim(struct index_pqueue *ipq, unsigned int k)
{
	while (k > 0 && ipqcmp(ipq, k/2, k)) {
		ipqueue_exch(ipq, k/2, k);
		k /= 2;
	}
}

static inline void 
sink(struct index_pqueue *ipq, unsigned int k)
{
	unsigned int j;
	
	j = 2 * k + 1;	/* k start with 0 */
	while (j < ipq->size) {
		/* 
		 * find smallest or largest key among children node.
		 */
		if (j < ipq->size - 1 && ipqcmp(ipq, j, j + 1))
			j++;
		
		/* 
		 * occurs break if parent node not less or greater
		 * children node.
		 */
		if (!ipqcmp(ipq, k, j))
			break;
		
		ipqueue_exch(ipq, k, j);
		k = j;
		j = 2 * k + 1;
	}
}
