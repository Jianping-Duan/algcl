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
#ifndef _INDEXFIBPQ_H_
#define _INDEXFIBPQ_H_

#include "algcomm.h"

struct index_fibpq_node {
	void *key;				/* key contained by the Node */
	unsigned long index;	/* index associated with the key */
	unsigned int degree;	/* degree of the tree rooted by this Node */
	struct index_fibpq_node *prev;	/* previous sibling of the Node */
	struct index_fibpq_node *next;	/* next sibling of the Node */
	struct index_fibpq_node *parent; /* parent of his Node */
	struct index_fibpq_node *child;	/* child of this Node */
	int mark;	/* indicates if this Node already lost a child */
};

struct index_fibpq {
	struct index_fibpq_node *head;	/* head of the circular root list */
	struct index_fibpq_node *result; /* min or max Node of the root list */
	struct index_fibpq_node **nodes; /* Array of Nodes in the heap */
	unsigned long capacity;	/* Maximum number of keys in the heap */
	unsigned long size;		/* Number of keys in the heap */
	unsigned int keysize;	/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* 
 * Whether the indexed fibonacci priority queue is empty, 
 * worst case is O(1). 
 */
#define IFIBPQ_ISEMPTY(fpq)		((fpq)->size == 0)

/* 
 * Whether the indexed fibonacci priority queue is full, 
 * worst case is O(1). 
 */
#define IFIBPQ_ISFULL(fpq)	\
	((fpq)->size >= (fpq)->capacity ? 1 : 0)

/* 
 * Number of elements currently on the priority queue, 
 * worst case is O(1). 
 */
#define IFIBPQ_SIZE(fpq)	((fpq)->size)

/* 
 * Does the priority queue contains the index i? 
 * worst case is O(1). 
 */
#define IFIBPQ_CONTAINS(fpq, i)	\
	((i) < (fpq)->capacity && (fpq)->nodes[i] != NULL ? 1 : 0)

/* 
 * Get the index associated with 
 * the minimum or maximum key, worst case is O(1). 
 */
#define IFIBPQ_GETINDEX(fpq)	((fpq)->result->index)

/* 
 * Get the minimum key currently in the queue, 
 * worst case is O(1). 
 */
#define IFIBPQ_GETKEY(fpq)	\
	((fpq)->size != 0 ? (fpq)->result->key : NULL)

/* 
 * Get the key associated with index i, 
 * worst case is O(1). 
 */
#define IFIBPQ_KEYOF(fpq, i)	\
	((i) >= 0 && (i) < (fpq)->capacity && IFIBPQ_CONTAINS(fpq, i) \
		? (fpq)->nodes[i] : NULL)

struct queue;

/* 
 * Initializes an empty indexed fibonacci 
 * priority queue. 
 */
void ifibpq_init(struct index_fibpq *fpq, unsigned long n, unsigned int ksize,
				algcomp_ft *cmp);

/* Associates a key with an index. */
int ifibpq_insert(struct index_fibpq *fpq, unsigned long i, const void *key);

/* Delete the minimum or maximum key. */
unsigned long ifibpq_delete(struct index_fibpq *fpq);

/* Traverses this indexed fibonacci heap. */
void ifibpq_traverse(const struct index_fibpq *fpq, struct queue *keys, 
					struct queue *indexes);

/* Clears this heap. */
void ifibpq_clear(struct index_fibpq *fpq);

/* Deletes the key associated the given index. */
int ifibpq_remove(struct index_fibpq *fpq, unsigned long i);

/* 
 * Decreases the key associated with 
 * index i to the given key. 
 */
int ifibpq_decrkey(struct index_fibpq *fpq, unsigned long i, const void *key);

/* 
 * Increases the key associated with 
 * index i to the given key. 
 */
int ifibpq_incrkey(struct index_fibpq *fpq, unsigned long i, const void *key);

/* 
 * Changes the key associated with 
 * index i to the given key. 
 */
int ifibpq_change(struct index_fibpq *fpq, unsigned long i, const void *key);

#endif /* _INDEXFIBPQ_H_ */
