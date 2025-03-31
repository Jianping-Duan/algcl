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
#ifndef _FIBONACCIPQ_H_
#define _FIBONACCIPQ_H_

#include "algcomm.h"

struct fibonacci_node {
	void *key;				/* key contained by the Node */
	unsigned int degree;	/* degree of the tree rooted by this Node */
	struct fibonacci_node *prev;	/* previous node */ 
	struct fibonacci_node *next;	/* next node */
	struct fibonacci_node *child;	/* child of this Node */
};

struct fibonaccipq {
	struct fibonacci_node *head;	/* head of the circular root list */
	struct fibonacci_node *result;	/* min or max Node of the root list */
	unsigned long size;				/* number of keys in the heap */
	unsigned int keysize;			/* the bytes of the key */
	algcomp_ft *cmp;				/* comparator over the keys */
};

/* Whether the priority queue is empty. */
#define FIBPQ_ISEMPTY(fpq)	((fpq)->size == 0)

/* 
 * Number of elements currently on the priority queue,
 * worst case is O(1). 
 */
#define FIBPQ_SIZE(fpq)		((fpq)->size)

/* 
 * Gets the minimum or maximum key currently in the queue,
 * worst case is O(1). 
 */
#define FIBPQ_GETKEY(fpq)	((fpq)->size != 0 ? (fpq)->result->key : NULL)

struct queue;

/* Initializes an empty fibonacci priority queue. */
void fibpq_init(struct fibonaccipq *fpq, unsigned int ksize, algcomp_ft *cmp);

/* 
 * Merges two heaps together, this operation is destructive. 
 */
struct fibonaccipq * 
fibpq_unionfpq(struct fibonaccipq *sfpq, const struct fibonaccipq *tfpq);

/* Insert a key in the queue. */
void fibpq_insert(struct fibonaccipq *fpq, const void *key);

/* Deletes the minimum or maximum key. */
void * fibpq_delete(struct fibonaccipq *fpq);

/* Gets all keys from this heap. */
void fibpq_keys(const struct fibonaccipq *fpq, struct queue *keys);

/* Clears this fibonacci heap. */
void fibpq_clear(struct fibonaccipq *fpq);

#endif /* _FIBONACCIPQ_H_ */
