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
#include "indexbinompq.h"
#include "queue.h"

/* 
 * Assuming root1 holds a greater or less key than root2, 
 * root2 becomes the new root. 
 */
#define IBINOMPQ_LINK(root1, root2)		do {	\
	root1->sibling = root2->child;				\
	root1->parent = root2;						\
	root2->child = root1;						\
	root2->degree++;							\
} while (0)

static struct index_binom_node * merge_node(struct index_binom_node *,
	struct index_binom_node *, struct index_binom_node *);
static struct index_binom_node * make_node(unsigned long, const void *,
	unsigned int);
static struct index_binompq * ibinompq_union(struct index_binompq *, 
	const struct index_binompq *);
static void traverse(const struct index_binom_node *, struct queue *,
	struct queue *);
static void release_node(struct index_binom_node *, unsigned int);
static struct index_binom_node * get_node1(struct index_binompq *);
static struct index_binom_node * get_node2(struct index_binompq *,
	unsigned long);
static void exchange_node(struct index_binompq *, struct index_binom_node *, 
	struct index_binom_node *);
static void swim(struct index_binompq *, unsigned long);
static void toroot(struct index_binompq *, unsigned long);

/* 
 * Initializes an empty indexed binomial priority queue 
 * with indicates between 0 and n - 1. 
 */
void 
ibinompq_init(struct index_binompq *pq,	unsigned long n, unsigned int ksize,
			algcomp_ft *cmp)
{
	unsigned long i;

	pq->head = NULL;
	pq->keysize = ksize;
	pq->cmp = cmp;
	pq->capacity = n;

	pq->nodes = (struct index_binom_node **)
		algmalloc(n * sizeof(struct index_binom_node *));
	for (i = 0; i < n; i++)
		pq->nodes[i] = NULL;
}

/* 
 * Number of elements currently on the indexed binomial 
 * priority queue, worst case is O(log(n)). 
 */
unsigned long 
ibinompq_size(const struct index_binompq *pq)
{
	unsigned long sz = 0, tmp;
	struct index_binom_node *current;
	
	current = pq->head;
	while (current != NULL) {
		/* 2^k the degrees of the root list.*/
		tmp = 1 << current->degree;	
		sz |= tmp;
		current = current->sibling;
	}
	
	return sz;
}

/* 
 * Associates a key with an index, 
 * worst case is O(log(n)) 
 */
int
ibinompq_insert(struct index_binompq *pq, unsigned long i, const void *key)
{
	struct index_binom_node *current;
	struct index_binompq tpq, *spq;
	
	if (i >= pq->capacity)
		return -1;
	if (IBINOMPQ_CONTAINS(pq, i))
		return -2;
	
	current = make_node(i, key, pq->keysize);
	pq->nodes[i] = current;
	tpq.head = current;
	
	spq = ibinompq_union(pq, &tpq);
	pq->head = spq->head;

	return 0;
}

/* 
 * Gets the minimum or maximum key currently in the binomial priority queue.
 * worst case is O(log(n)) 
 */
void * 
ibinompq_get_key(const struct index_binompq *pq)
{
	struct index_binom_node *result, *current;
	
	if (IBINOMPQ_ISEMPTY(pq))
		return NULL;
	
	result = pq->head;
	current = pq->head;
	while (current->sibling != NULL) {
		if (pq->cmp(result->key, current->sibling->key))
			result = current->sibling;
		current = current->sibling;
	}
	
	return result->key;
}

/* 
 * Gets the index associated with the minimum or maximum key.
 * worst case is O(log(n)) 
 */
unsigned long 
ibinompq_get_index(const struct index_binompq *pq)
{
	struct index_binom_node *result, *current;
	
	assert(!IBINOMPQ_ISEMPTY(pq));
	
	result = pq->head;
	current = pq->head;
	while (current->sibling != NULL) {
		if (pq->cmp(result->key, current->sibling->key))
			result = current->sibling;
		current = current->sibling;
	}
	
	return result->index;
}

void 
ibinompq_traverse(const struct index_binompq *pq, struct queue *keys,
				struct queue *indexes)
{
	if (!IBINOMPQ_ISEMPTY(pq))
		traverse(pq->head, keys, indexes);
}

void 
ibinompq_clear(struct index_binompq *pq)
{
	if (!IBINOMPQ_ISEMPTY(pq))
		release_node(pq->head, pq->keysize);
	
	ALGFREE(pq->nodes);
	pq->capacity = 0;
}

/* 
 * Deletes the minimum or maximum key and returns its index.
 * Worst case is O(log(n)) 
 */
unsigned long 
ibinompq_delete(struct index_binompq *pq)
{
	struct index_binom_node *result, *current, *prev, *next;
	struct index_binompq tpq, *spq;
	unsigned long index;
	
	assert(!IBINOMPQ_ISEMPTY(pq));
	
	result = get_node1(pq);
	index = result->index;
	current = result->child == NULL ? result : result->child;
	if (result->child != NULL) {
		/* 
		 * detach children nodes, now result is only itself.
		 */
		result->child = NULL;
		/* reverse roots list for the children of result */
		prev = NULL;
		next = current->sibling;
		while (next != NULL) {
			current->parent = NULL;
			current->sibling = prev;
			prev = current;
			current = next;
			next = next->sibling;
		}
		current->sibling = prev;
		current->parent = NULL;
		
		tpq.head = current;
		spq = ibinompq_union(pq, &tpq);
		pq->head = spq->head;
	}
	
	pq->nodes[index] = NULL;
	if (pq->keysize != 0)
		ALGFREE(result->key);
	ALGFREE(result);
	
	return index;
}

/* 
 * Deletes the key associated the given index, 
 * Worst case is O(log(n)). 
 */
int
ibinompq_remove(struct index_binompq *pq, unsigned long i)
{
	struct index_binom_node *prev, *current, *result, *next;
	struct index_binompq tpq, *spq;
	
	if (i >= pq->capacity)
		return -1;
	if (!IBINOMPQ_CONTAINS(pq, i))
		return -2;
	
	toroot(pq, i);
	result = get_node2(pq, i);
	pq->nodes[i] = NULL;
	
	if (result->child != NULL) {
		current = result->child;
		result->child = NULL;
		
		prev = NULL;
		next = current->sibling;
		while (next != NULL) {
			current->parent = NULL;
			current->sibling = prev;
			prev = current;
			current = next;
			next = next->sibling;
		}
		current->sibling = prev;	/* last node */
		current->parent = NULL;
		
		tpq.head = current;
		spq = ibinompq_union(pq, &tpq);
		pq->head = spq->head;
	}
	
	if (pq->keysize != 0)
		ALGFREE(result->key);
	ALGFREE(result);

	return 0;
}

/* 
 * Decreases the key associated with index i to the given key.
 * Worst case is O(log(n)). 
 */
int
ibinompq_decrkey(struct index_binompq *pq, unsigned long i, const void *key)
{
	struct index_binom_node *node;

	if (i >= pq->capacity)
		return -1;
	if (!IBINOMPQ_CONTAINS(pq, i))
		return -2;
	
	node = pq->nodes[i];
	if (pq->cmp(key, node->key))
		return -3;
	
	if (pq->keysize != 0)
		memcpy(node->key, key, pq->keysize);
	else
		node->key = (void *)key;

	swim(pq, i);

	return 0;
}

/* 
 * Increases the key associated with index i to the given key.
 * Worst case is O(log(n)). 
 */
int
ibinompq_incrkey(struct index_binompq *pq, unsigned long i, const void *key)
{
	struct index_binom_node *node;

	if (i >= pq->capacity)
		return -1;
	if (!IBINOMPQ_CONTAINS(pq, i))
		return -2;
	
	node = pq->nodes[i];
	if (pq->cmp(node->key, key))
		return -3;
	
	if (ibinompq_remove(pq, i) != 0)
		return -4;
	ibinompq_insert(pq, i, key);

	return 0;
}

/*
 * Changes the key associated with index i to 
 * the given key, worst case is O(log(n)). 
 */
int
ibinompq_change(struct index_binompq *pq, unsigned long i, const void *key)
{
	struct index_binom_node *node;

	if (i >= pq->capacity)
		return -1;
	if (!IBINOMPQ_CONTAINS(pq, i))
		return -2;
	
	node = pq->nodes[i];
	if (pq->cmp(node->key, key))
		return ibinompq_decrkey(pq, i, key);
	else
		return ibinompq_incrkey(pq, i, key);
}

/******************** static function boundary ********************/

/* 
 * Merges two root lists into one, there can be up to 2 
 * Binomial Trees of same degree. 
 */
static struct index_binom_node * 
merge_node(struct index_binom_node *hnode, 
			struct index_binom_node *xnode, 
			struct index_binom_node *ynode)
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

/* 
 * creates the node of the indexed binomial 
 * priority queue using the specified key and its bytes. 
 */
static struct index_binom_node * 
make_node(unsigned long ind, const void *key, unsigned int ksize)
{
	struct index_binom_node *current;

	current = (struct index_binom_node *)
		algmalloc(sizeof(struct index_binom_node));
	
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;
	
	current->index = ind;
	current->degree = 0;
	current->child = NULL;
	current->sibling = NULL;
	
	return current;
}

/* 
 * Merges two Binomial Heaps together and returns the resulting Binomial Heap.
 * It destroys the two Heaps in parameter, which should not be used any after.
 * To guarantee logarithmic time, this function assumes the arrays are 
 * up-to-date.
 */
static struct index_binompq * 
ibinompq_union(struct index_binompq *spq, const struct index_binompq *tpq)
{
	struct index_binom_node *newnode, *mgrnode;
	struct index_binom_node *current, *prev, *next;
	
	newnode = make_node(0, NULL, 0);
	mgrnode = merge_node(newnode, spq->head, tpq->head);
	/* because newnode is empty node */
	spq->head = mgrnode->sibling;
	
	current = spq->head;
	prev = NULL;
	next = current->sibling;
	while (next != NULL) {
		/* Nothing do it*/
		if (current->degree < next->degree || (next->sibling != NULL &&
		    next->sibling->degree == current->degree)) {
			prev = current;
			current = next;
		} else if (spq->cmp(next->key, current->key)) {
			current->sibling = next->sibling;
			IBINOMPQ_LINK(next, current);	/* links next to current */
		} else {	/* !spq->cmp */
			if (prev == NULL)
				spq->head = next;	/* moves head pointer */
			else
				prev->sibling = next;
			IBINOMPQ_LINK(current, next);	
			current = next;		/* current becomes next pointer */
		}
		next = current->sibling;
	}
	
	return spq;
}

static void 
traverse(const struct index_binom_node *node, struct queue *keys,
		struct queue *indexes)
{
	const struct index_binom_node *current;
	
	current = node;
	while (current != NULL) {
		enqueue(keys, current->key);
		enqueue(indexes, &(current->index));
		if (current->child != NULL)
			traverse(current->child, keys, indexes);
		current = current->sibling;
	}
}

static void 
release_node(struct index_binom_node *node, unsigned int ksize)
{
	struct index_binom_node *current, *next;
	
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

/* 
 * Returns the node containing 
 * the minimum or maximum key.
 */
static struct index_binom_node * 
get_node1(struct index_binompq *pq)
{
	struct index_binom_node *current, *result, *prev;
	
	assert(!IBINOMPQ_ISEMPTY(pq));
	
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
	
	/* head is minimum or maximum key */
	if (pq->head == result)
		pq->head = result->sibling;
	/* the last node is minimum or maximum key */
	else if (result->sibling == NULL)
		prev->sibling = NULL;
	else
		prev->sibling = result->sibling;
	
	return result;
}

/* 
 * Assuming the key associated with i is in the root list, 
 * returns the node of index i. 
 */
static struct index_binom_node * 
get_node2(struct index_binompq *pq, unsigned long i)
{
	struct index_binom_node *current, *ref, *prev;
	
	ref = pq->nodes[i];
	current = pq->head;
	prev = NULL;
	
	while (ref != current) {
		prev = current;
		current = current->sibling;
	}
	
	if (pq->head == ref)		/* equal to the head node */
		pq->head = pq->head->sibling;
	else
		prev->sibling = current->sibling;
	
	return ref;
}

/* Exchanges the positions of two nodes */
static void 
exchange_node(struct index_binompq *pq, struct index_binom_node *node1, 
			struct index_binom_node *node2)
{
	void *bytes;
	unsigned long tmpidx;
	
	/* exchanges two keys */
	bytes = node1->key;
	node1->key = node2->key;
	node2->key = bytes;
	
	/* exchanges two indexes */
	tmpidx = node1->index;
	node1->index = node2->index;
	node2->index = tmpidx;
	
	/* exchanges two indexed nodes */
	pq->nodes[node1->index] = node1;
	pq->nodes[node2->index] = node2;
}

/* Moves a Node upward */
static void 
swim(struct index_binompq *pq, unsigned long i)
{
	struct index_binom_node *current, *parent = NULL;
	
	current = pq->nodes[i];
	if (current != NULL)
		parent = current->parent;
	
	if (parent != NULL && pq->cmp(parent->key, current->key)) {
		exchange_node(pq, current, parent);
		swim(pq, i);
	}
}

/* 
 * The key associated with i becomes the root of 
 * its Binomial Tree, regardless of the order 
 * relation defined for the keys. 
 */
static void 
toroot(struct index_binompq *pq, unsigned long i)
{
	struct index_binom_node *current, *parent = NULL;
	
	current = pq->nodes[i];
	if (current != NULL)
		parent = current->parent;
	
	if (parent != NULL) {
		exchange_node(pq, current, parent);
		toroot(pq, i);
	}
}
