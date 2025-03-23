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
#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

#include "algcomm.h"

struct priority_queue {
	void **keys;			/* store keys at indicates 0 to size - 1 */
	unsigned int keysize;	/* the bytes of the key */
	unsigned long capacity;	/* priority queue capacity */
	unsigned long size;		/* number of key on priority queue */
	algcomp_ft *cmp;		/* compare function for keys */
};

/* Returns true if this priority queue is empty. */
#define PQUEUE_ISEMPTY(pq)	((pq)->size == 0)

/* Returns the number of keys on this priority queue. */
#define PQUEUE_SIZE(pq)		((pq)->size)

/* Returns true if this priority queue is full */
#define PQUEUE_ISFULL(pq)	\
	((pq)->size < (pq)->capacity ? 0 : 1)

/* Returns the array of keys from this priority queue. */
#define PQUEUE_KEYS(pq)		((pq)->keys)

/* 
 * Initializes an empty priority queue with the given
 * capacity and key-size, using the given compare 
 * function. 
 */
void pqueue_init(struct priority_queue *pq, unsigned long cap,
				unsigned int ksize, algcomp_ft *cmp);

/* Adds a new key to this priority queue. */
void pqueue_insert(struct priority_queue *pq, const void *key);

/* 
 * Removes and returns a smallest or largest key 
 * on this priority queue. 
 */
void * pqueue_delete(struct priority_queue *pq);

/* Clears this priority queue. */
void pqueue_clear(struct priority_queue *pq);

#endif /* _PRIORITYQUEUE_H_ */
