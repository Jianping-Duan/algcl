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
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "algcomm.h"

struct queue_node {
	void *key;
	struct queue_node *next;
};

struct queue {
	struct queue_node *front;	/* beginning of queue */
	struct queue_node *rear;	/* end of queue */
	unsigned long size;			/* number of keys on queue */
	unsigned short keysize;
};

/* Returns the number of keys in this queue. */
#define	QUEUE_SIZE(q)	((q)->size)

/* Is this queue empty? */
#define QUEUE_ISEMPTY(q)	\
	((q)->front == NULL && (q)->rear == NULL)

/* Initializes an empty queue */
#define QUEUE_INIT(qp, ksize)	do {	\
	(qp)->front = NULL;					\
	(qp)->rear = NULL;					\
	(qp)->size = 0;						\
	(qp)->keysize = ksize;				\
} while(0)

/* Adds the key to this queue. */
void enqueue(struct queue *qp, const void *key);

/* 
 * Removes and returns the key on this queue that 
 * was least recently added. 
 */
void dequeue(struct queue *qp, void **key);

/* Clears this queue. */
void queue_clear(struct queue *qp);

#endif /* _QUEUE_H_ */
