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
#ifndef _INDEXPQUEUE_H_
#define _INDEXPQUEUE_H_

#include "algcomm.h"

struct index_pqueue {
	void **keys;		/* keys[i] = priority of i */
	unsigned short keysize;	/* the bytes of the key */
	unsigned long capacity;	/* maximum number of keys on priority queue */
	unsigned long size;	/* number of keys on priority queue */
	long *pq;		/* binary heap using 0-based indexing */
	long *qp;		/* inverse of pq, qp[pq[i]] = pq[qp[i]]] = i */
	algcomp_ft *cmp;	/* key compare function */	
};

/* Returns true if this priority queue is empty. */
#define IPQUEUE_ISEMPTY(ipq)	((ipq)->size == 0)

/* Returns the number of keys on this priority queue. */
#define IPQUEUE_SIZE(pq)	((pq)->size)

/* Returns true if this priority queue is full */
#define IPQUEUE_ISFULL(pq)	((pq)->size < (pq)->capacity ? 0 : 1)

/* Is I an index on this priority queue? */
#define IPQUEUE_CONTAINS(ipq, i)	\
	(((i) < (ipq)->capacity) && ((ipq)->qp[i] != (-1)))

/* 
 * Returns an index associated with 
 * a minimum or maximum key.
 */
#define IPQUEUE_FIRST_INDEX(ipq)	((ipq)->pq[0])

/* Returns a minimum or maximum key. */
#define IPQUEUE_FIRST_KEY(ipq)		((ipq)->keys[0])

/* Returns the key associated with index. */
#define IPQUEUE_KEYOF(ipq, i)	\
	(((i) < 0 || (i) >= (ipq)->capacity) ? NULL : (ipq)->keys[i])

/* Returns all indexes on this priority queue. */
#define IPQUEUE_INDEXES(ipq)	((ipq)->pq)

/* Returns all keys on this priority queue. */
#define IPQUEUE_KEYS(ipq)	((ipq)->keys)

/* 
 * Initializes an empty indexed priority queue with the given capacity and
 * key-size, using the given compare function. 
 */
void ipqueue_init(struct index_pqueue *ipq, unsigned long cap,
		unsigned short ksize, algcomp_ft *cmp);

/* 
 * Associates key with index i, i an index, 
 * key the key to associate with index i.
 */
void ipqueue_insert(struct index_pqueue *ipq, unsigned long i, const void *key);

/* 
 * Removes a minimum or maximum key and returns its associated index,
 * return an index associated with a minimum or maximum key. 
 */
unsigned long ipqueue_delete(struct index_pqueue *ipq);

/* 
 * Remove the key associated with index, 
 * i the index of the key to remove. 
 */
void ipqueue_remove(struct index_pqueue *ipq, unsigned long i);

/* 
 * Change the key associated with index i to the specified key. 
 */
void ipqueue_change(struct index_pqueue *ipq, unsigned long i, const void *key);

/* 
 * Decrease the key associated with index i to the specified value. 
 */
void ipqueue_decrkey(struct index_pqueue *ipq, unsigned long i,
		const void *key);

/* 
 * Increase the key associated with index i to 
 * the specified value. 
 */
void ipqueue_incrkey(struct index_pqueue *ipq, unsigned long i,
		const void *key);

/* Clears this indexed priority queue. */
void ipqueue_clear(struct index_pqueue *ipq);

#endif /* _INDEXPQUEUE_H_ */
