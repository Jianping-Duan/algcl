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
#ifndef _INDEXPAIRHEAP_H_
#define _INDEXPAIRHEAP_H_

#include "algcomm.h"

struct index_pheap_node {
	void *key;		/* key contained by the node */
	unsigned long index;	/* index associated with the key */
	unsigned int degree;	/* the degree of the tree rooted by this node */
	struct index_pheap_node *child;		/* child of this node */
	struct index_pheap_node *sibling;	/* next sibling node */
	struct index_pheap_node *prev;		/* previous sibling node */
};

struct index_pheap {
	struct index_pheap_node *root;		/* root of this tree */
	struct index_pheap_node **nodes;	/* Array of Nodes in the heap */
	unsigned long capacity;		/* Maximum number of keys in the heap */
	unsigned long size;		/* number of keys in this heap */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* 
 * Whether the indexed pairing heap is empty,
 * worst case is O(1). 
 */
#define IPHEAP_ISEMPTY(iph)	((iph)->size == 0)

/* 
 * Whether the indexed pairing heap is full,
 * worst case is O(1). 
 */
#define IPHEAP_ISFULL(iph)	((iph)->size == (iph)->capacity ? 1 : 0)

/* 
 * Number of elements currently on the indexed pairing heap,
 * worst case is O(1). 
 */
#define IPHEAP_SIZE(iph)	((iph)->size)

/* 
 * Does the priority queue contains the index i? 
 * worst case is O(1). 
 */
#define IPHEAP_CONTAINS(iph, i)	\
	((i) < (iph)->capacity && (iph)->nodes[i] != NULL ? 1 : 0)

/* 
 * Get the index associated with the minimum or maximum key.
 * worst case is O(1). 
 */
#define IPHEAP_GETINDEX(iph)	((iph)->root->index)

/* 
 * Get the minimum key currently in the queue, 
 * worst case is O(1). 
 */
#define IPHEAP_GETKEY(iph)	((iph)->size != 0 ? (iph)->root->key : NULL)

/* 
 * Get the key associated with index i, 
 * worst case is O(1). 
 */
#define IPHEAP_KEYOF(iph, i)	\
	(IPHEAP_CONTAINS(iph, i) ? (iph)->root->key : NULL)

struct single_list;

/* Initializes an empty indexed pairing heap. */
void ipheap_init(struct index_pheap *iph, unsigned long sz, unsigned int ksize,
		algcomp_ft *cmp);

/* Associates a key with an index. */
void ipheap_insert(struct index_pheap *iph, unsigned long ind, const void *key);

/* Deletes the minimum or maximum key from this pairing-heap. */
unsigned long ipheap_delete(struct index_pheap *iph);

/* Traverses this indexed pairing heap. */
void ipheap_traverse(const struct index_pheap *iph, struct single_list *keys,
		struct single_list *indexes);

/* Clears this indexed pairing heap. */
void ipheap_clear(struct index_pheap *iph);

/* 
 * Deletes the key associated the given index, 
 * worst case is O(log(n)) (amortized). 
 */
int ipheap_remove(struct index_pheap *iph, unsigned long ind);

/* 
 * Decreases the key associated with index i to the given key. 
 * worst case is O(1) (amortized).
 */
int ipheap_decrkey(struct index_pheap *iph, unsigned long ind, const void *key);

/* 
 * Increases the key associated with index i to the given key.
 * worst case is O(log(n)).
 */
int ipheap_incrkey(struct index_pheap *iph, unsigned long ind, const void *key);

/* 
 * Changes the key associated with index i to the given key.
 * worst case is O(1) (amortized) -- O(log(n)).
 */
int ipheap_change(struct index_pheap *iph, unsigned long ind, const void *key);


#endif	/* _INDEXPAIRHEAP_H_ */
