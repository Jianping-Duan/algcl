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
#ifndef _PAIRINGHEAP_H_
#define _PAIRINGHEAP_H_

#include "algcomm.h"

struct pairing_node {
	void *key;		/* key contained by the node */
	unsigned long degree;	/* the degree of the tree rooted by this node */
	struct pairing_node *child;	/* child of this node */
	struct pairing_node *sibling;	/* next sibling node */
	struct pairing_node *prev;	/* previous sibling node */
};

struct pairing_heap {
	struct pairing_node *root;	/* root of this tree */
	unsigned long size;		/* number of keys in this heap */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* Whether the priority queue is empty. */
#define PHEAP_ISEMPTY(ph)	((ph)->size == 0)

/* 
 * Gets the minimum or maximum key currently 
 * in the queue. 
 */
#define PHEAP_GETKEY(ph)	((ph)->root != NULL ? (ph)->root->key : NULL)

/* Number of keys currently on the priority queue. */
#define PHEAP_SIZE(ph)		((ph)->size)

struct single_list;

/* 
 * Initializes an empty priority queue using the given compare function 
 * and key-size. 
 */
void pheap_init(struct pairing_heap *ph, unsigned int ksize, algcomp_ft *cmp);

/* 
 * Inserts a key in the heap, 
 * and maintaining the heap order. 
 */
void pheap_insert(struct pairing_heap *ph, const void *key);

/* 
 * Deletes the minimum or maximum key 
 * from this pairing-heap.
 */
void * pheap_delete(struct pairing_heap *ph);

/* Gets all keys for this paring heap */
void pheap_keys(const struct pairing_heap *ph, struct single_list *keys);

/* Clears this pairing heap */
void pheap_clear(struct pairing_heap *ph);

#endif	/* _PAIRINGHEAP_H_ */
