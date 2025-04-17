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
#include "binomialpq.h"
#include "singlelist.h"

/* 
 * Assuming root1 holds a greater or less key than root2, 
 * root2 becomes the new root. 
 */
#define BINOMPQ_LINK(root1, root2)		do {	\
	root1->sibling = root2->child;				\
	root2->child = root1;						\
	root2->degree++;							\
} while (0)
	
static struct binomial_node * make_node(const void *, unsigned int);
static struct binomial_node * merge_node(struct binomial_node *,
	struct binomial_node *, struct binomial_node *);
static struct binomial_node * get_node(struct binomialpq *);
static void traverse(const struct binomial_node *, struct single_list *);
static void release_node(struct binomial_node *, unsigned int);

/* 
 * Initializes an empty priority queue using the given 
 * compare function and key-size. 
 */
void 
binompq_init(struct binomialpq *pq, unsigned int ksize,	algcomp_ft *cmp)
{
	pq->head = NULL;
	pq->keysize = ksize;
	pq->cmp = cmp;
}

/* 
 * Number of elements currently on the priority queue. 
 * Worst case is O(log(n))
 */
unsigned long 
binompq_size(const struct binomialpq *pq)
{
	unsigned long sz = 0, tmp;
	struct binomial_node *current;
	
	current = pq->head;
	while (current != NULL) {
		tmp = 1 << current->degree;	/* 2^k */
		sz |= tmp;
		current = current->sibling;
	}
	
	return sz;
}

/* 
 * Merges two Binomial heaps together, 
 * this operation is destructive, 
 * worst case is O(log(n)) 
 */
struct binomialpq * 
binompq_union(struct binomialpq *spq, const struct binomialpq *tpq)
{
	struct binomial_node *newnode, *mgrnode;
	struct binomial_node *current, *prev, *next;
	
	newnode = make_node(NULL, 0);
	mgrnode = merge_node(newnode, spq->head, tpq->head);
	/* because newnode is empty node */
	spq->head = mgrnode->sibling;
	
	current = spq->head;
	prev = NULL;
	next = current->sibling;
	while (next != NULL) {
		if (current->degree < next->degree || (next->sibling != NULL &&
		    next->sibling->degree == current->degree)) { /* Nothing do it. */
			prev = current;
			current = next;
		} else if (spq->cmp(next->key, current->key)) {
			current->sibling = next->sibling;
			BINOMPQ_LINK(next, current);	/* links next to current */
		} else {	/* !spq->com */
			if (prev == NULL)
				spq->head = next;	/* moves head pointer */
			else
				prev->sibling = next;
			BINOMPQ_LINK(current, next);	/* links current to next */
			current = next;		/* current becomes next pointer */
		}
		next = current->sibling;
	}
	
	return spq;
}

/* Puts a Key in the heap, worst case is O(log(n)) */
void 
binompq_insert(struct binomialpq *pq, const void *key)
{
	struct binomial_node *node;
	struct binomialpq tpq, *spq;
	
	node = make_node(key, pq->keysize);
	tpq.head = node;
	
	spq = binompq_union(pq, &tpq);
	pq->head = spq->head;
}

/* 
 * Gets the minimum or maximum key currently in 
 * the binomial priority queue,
 * worst case is O(log(n)) 
 */
void * 
binompq_get(const struct binomialpq *pq)
{
	struct binomial_node *result, *current;
	
	assert(!BINOMPQ_ISEMPTY(pq));
	
	result = pq->head;
	current = pq->head;
	while (current->sibling != NULL) {
		if (pq->cmp(result->key, current->sibling->key))
			result = current->sibling;
		current = current->sibling;
	}
	
	return result->key;
}

/* Deletes the minimum key, worst case is O(log(n)) */
void * 
binompq_delete(struct binomialpq *pq)
{
	struct binomial_node *result, *current, *prev, *next;
	struct binomialpq tpq, *spq;
	void *key;
	
	assert(!BINOMPQ_ISEMPTY(pq));
	
	result = get_node(pq);
	key = result->key;
	current = result->child == NULL ? result : result->child;
	if (result->child != NULL) {
		/* 
		 * detach children nodes, 
		 * now result is only itself.
		 */
		result->child = NULL;

		/* reverse roots list for the children of result */
		prev = NULL;
		next = current->sibling;
		while (next != NULL) {
			current->sibling = prev;
			prev = current;
			current = next;
			next = next->sibling;
		}
		current->sibling = prev;
		
		tpq.head = current;
		spq = binompq_union(pq, &tpq);
		pq->head = spq->head;
	}
	
	ALGFREE(result);
	
	return key;
}

void 
binompq_keys(const struct binomialpq *pq, struct single_list *keys)
{
	if (!BINOMPQ_ISEMPTY(pq))
		traverse(pq->head, keys);
}

void 
binompq_clear(struct binomialpq *pq)
{
	if (!BINOMPQ_ISEMPTY(pq))
		release_node(pq->head, pq->keysize);
}

/******************** static function boundary ********************/

/* 
 * Creates binomial priority queue node using 
 * the specified key and its bytes. 
 */
static struct binomial_node * 
make_node(const void *key, unsigned int ksize)
{
	struct binomial_node *current;

	current = (struct binomial_node *)algmalloc(sizeof(struct binomial_node));
	
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;

	current->degree = 0;
	current->child = NULL;
	current->sibling = NULL;
	
	return current;
}

/* 
 * Merges two root lists into one, there can be up to 2 
 * Binomial Trees of same degree. 
 */
static struct binomial_node * 
merge_node(struct binomial_node *hnode, struct binomial_node *xnode, 
		struct binomial_node *ynode)
{
	if (xnode == NULL && ynode == NULL)
		return hnode;
	else if (xnode == NULL)
		hnode->sibling = merge_node(ynode, NULL, ynode->sibling);
	else if (ynode == NULL)
		hnode->sibling = merge_node(xnode, xnode->sibling, NULL);
	else if (xnode->degree < ynode->degree)
		hnode->sibling = merge_node(xnode, xnode->sibling, ynode);
	else 
		hnode->sibling = merge_node(ynode, xnode, ynode->sibling);
	
	return hnode;
}

/* Returns the node containing the minimum or maximum key */
static struct binomial_node * 
get_node(struct binomialpq *pq)
{
	struct binomial_node *current, *result, *prev;
	
	assert(!BINOMPQ_ISEMPTY(pq));
	
	current = pq->head;
	result = pq->head;
	prev = NULL;
	while (current->sibling != NULL) {
		if (pq->cmp(result->key, current->sibling->key)) {
			prev = current;
			result = current->sibling;
		}
		current = current->sibling;
	}
	
	if (pq->head == result)	
		pq->head = result->sibling;	/* head is minimum or maximum key */
	/* the last node is minimum or maximum key */
	else if (result->sibling == NULL)
		prev->sibling = NULL;
	else
		prev->sibling = result->sibling;
	
	return result;
}

static void 
traverse(const struct binomial_node *node, struct single_list *keys)
{
	const struct binomial_node *current;
	
	current = node;
	while (current != NULL) {
		slist_append(keys, current->key);
		if (current->child != NULL)
			traverse(current->child, keys);
		current = current->sibling;
	}
}

static void 
release_node(struct binomial_node *node, unsigned int ksize)
{
	struct binomial_node *current, *next;
	
	current = node;
	while (current != NULL) {
		if (ksize != 0)
			ALGFREE(current->key);
		if (current->child != NULL)
			release_node(current->child, ksize);
		next = current->sibling;
		ALGFREE(current);
		current = next;
	}
}
