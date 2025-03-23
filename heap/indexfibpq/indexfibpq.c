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
#include "indexfibpq.h"
#include "queue.h"
#include <math.h>		/* log2(), ceil() */

static struct index_fibpq_node * make_node(unsigned long, const void *,
	unsigned int);
static struct index_fibpq_node * insert_node(struct index_fibpq_node *,
	struct index_fibpq_node *);
static struct index_fibpq_node * cut_node(struct index_fibpq_node *, 
	struct index_fibpq_node *);
static void cut_index(struct index_fibpq *, unsigned long);
static struct index_fibpq_node * meld_node(struct index_fibpq_node *,
	struct index_fibpq_node *);
static inline void link_node(struct index_fibpq_node *,
	struct index_fibpq_node *);
static void consolidate(struct index_fibpq *);
static void traverse(const struct index_fibpq_node *, struct queue *,
	struct queue *);
static void release_node(struct index_fibpq_node *, unsigned int);

/* 
 * Initializes an empty indexed fibonacci priority queue, 
 * worst case is O(1). 
 */
void 
ifibpq_init(struct index_fibpq *fpq, unsigned long n, unsigned int ksize,
			algcomp_ft *cmp)
{
	unsigned long i;

	fpq->head = NULL;
	fpq->result = NULL;
	fpq->capacity = n;
	fpq->size = 0;
	fpq->keysize = ksize;
	fpq->cmp = cmp;

	fpq->nodes = (struct index_fibpq_node **)
		algmalloc(n * sizeof(struct index_fibpq_node *));
	for(i = 0; i < n; i++)
		fpq->nodes[i] = NULL;
}

/* Associates a key with an index, worst case is O(1). */
void 
ifibpq_insert(struct index_fibpq *fpq, unsigned long i, const void *key)
{
	struct index_fibpq_node *current;
	
	current = make_node(i, key, fpq->keysize);
	fpq->head = insert_node(current, fpq->head);
	fpq->nodes[i] = current;
	fpq->size++;
	
	if(fpq->result == NULL)
		fpq->result = fpq->head;
	else {
		fpq->result = fpq->cmp(fpq->result->key, key)
			? fpq->head : fpq->result;
	}
}

/* 
 * Delete the minimum or maximum key, 
 * worst case is O(log(n)) (amortized). 
 */
unsigned long 
ifibpq_delete(struct index_fibpq *fpq)
{
	struct index_fibpq_node *current;
	unsigned long ind;

	fpq->head = cut_node(fpq->result, fpq->head);
	current = fpq->result->child;
	ind = fpq->result->index;

	if(fpq->keysize != 0)
		ALGFREE(fpq->result->key);
	
	if(current != NULL) {
		do {
			/* detach everyone child's parent pointer */
			current->parent = NULL;
			current = current->next;
		} while(current != NULL && 
			current != fpq->result->child);

		fpq->head = meld_node(fpq->head, current);
		fpq->result->child = NULL;
	}
	
	fpq->size--;
	fpq->nodes[ind] = NULL;
	ALGFREE(fpq->result);
	
	if(!IFIBPQ_ISEMPTY(fpq))
		consolidate(fpq);
	else
		fpq->result = NULL;
	
	return ind;
}

void 
ifibpq_traverse(const struct index_fibpq *fpq, struct queue *keys,
				struct queue *indexes)
{
	if(!IFIBPQ_ISEMPTY(fpq))
		traverse(fpq->head,	keys, indexes);
}

void 
ifibpq_clear(struct index_fibpq *fpq)
{
	if(IFIBPQ_ISEMPTY(fpq))
		return;
	
	release_node(fpq->head, fpq->keysize);
	ALGFREE(fpq->nodes);
	fpq->size = 0;
	fpq->capacity = 0;
}

/* 
 * Deletes the key associated the given index, 
 * worst case is O(log(n)) (amortized). 
 */
void 
ifibpq_remove(struct index_fibpq *fpq, unsigned long i)
{
	struct index_fibpq_node *current, *pchild, *pnext;
	
	if(i >= fpq->capacity)
		return;
	
	if(!IFIBPQ_CONTAINS(fpq, i))
		return;
	
	current = fpq->nodes[i];
	if(fpq->keysize != 0)
		ALGFREE(current->key);

	/* 
	 * Current was not in the roots list. 
	 * Move the index of the specified node and associated
	 * its child to the roots list. 
	 */
	if(current->parent != NULL) 
		cut_index(fpq, i); 
		
	fpq->head = cut_node(current, fpq->head);
	if((pchild = current->child) != NULL) {
		current->child = NULL;
		pnext = pchild;
		do {
			pnext->parent = NULL;
			pnext = pnext->next;
		} while(pnext != pchild);
		
		fpq->head = meld_node(fpq->head, pchild);
	}

	fpq->size--;
	
	if(!IFIBPQ_ISEMPTY(fpq))
		consolidate(fpq);
	else
		fpq->result = NULL;
	
	ALGFREE(current);
	fpq->nodes[i] = NULL;
}

/* 
 * Decreases the key associated with index i to 
 * the given key, worst case is O(1) (amortized). 
 */
void 
ifibpq_decrkey(struct index_fibpq *fpq, unsigned long i, const void *key)
{
	struct index_fibpq_node *current;
	
	if(i >= fpq->capacity)
		return;
	
	if(!IFIBPQ_CONTAINS(fpq, i))
		return;
	
	current = fpq->nodes[i];
	if(fpq->cmp(key, current->key))
		return;

	if(fpq->keysize != 0)
		memcpy(current->key, key, fpq->keysize);
	else
		current->key = (void *)key;

	if(current->parent != NULL && 
	   fpq->cmp(current->parent->key, key))
		cut_index(fpq, i);
}

/* 
 * Increases the key associated with index i to 
 * the given key. worst case is O(log(n)). 
 */
void 
ifibpq_incrkey(struct index_fibpq *fpq, unsigned long i, const void *key)
{
	struct index_fibpq_node *current;
	
	if(i >= fpq->capacity)
		return;
	
	if(!IFIBPQ_CONTAINS(fpq, i))
		return;
	
	current = fpq->nodes[i];
	if(fpq->cmp(current->key, key))
		return;
	
	ifibpq_remove(fpq, i);
	ifibpq_insert(fpq, i, key);
}

/* 
 * Changes the key associated with index i 
 * to the given key.
 * If the given key is greater, Worst case is O(log(n)).
 * If the given key is lower, 
 * Worst case is O(1) (amortized).
 */
void 
ifibpq_change(struct index_fibpq *fpq, unsigned long i, const void *key)
{
	struct index_fibpq_node *current;
	
	if(i >= fpq->capacity)
		return;
	
	if(!IFIBPQ_CONTAINS(fpq, i))
		return;
	
	current = fpq->nodes[i];
	if(fpq->cmp(current->key, key))
		ifibpq_decrkey(fpq, i, key);
	else
		ifibpq_incrkey(fpq, i, key);
}

/******************** static function boundary ********************/

/* Creates fibonacci priority queue node using 
 * the specified key and its bytes. 
 */
static struct index_fibpq_node * 
make_node(unsigned long ind, const void *key, unsigned int ksize)
{
	struct index_fibpq_node *current;
	
	current = (struct index_fibpq_node *)
		algmalloc(sizeof(struct index_fibpq_node));
	
	if(ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
	else
		current->key = (void *)key;
	
	current->index = ind;
	current->degree = 0;
	current->prev = NULL;
	current->next = NULL;
	current->parent = NULL;
	current->child = NULL;
	current->mark = 0;
	
	return current;
}

/* 
 * Inserts a Node in a circular list containing head, 
 * returns a new head.
 */
static struct index_fibpq_node * 
insert_node(struct index_fibpq_node *node, struct index_fibpq_node *head)
{
	if(head == NULL) {		/* links itself */
		node->prev = node;
		node->next = node;
	}
	else {
		if(head->prev == NULL && head->next == NULL) {
			node->prev = node;
			node->next = node;
			return node;
		}

		head->prev->next = node;
		node->next = head;
		node->prev = head->prev;
		head->prev = node;
	}
	
	return node;
}

/* 
 * Removes a tree from the list defined by 
 * the head pointer. 
 */
static struct index_fibpq_node * 
cut_node(struct index_fibpq_node *node, struct index_fibpq_node *head)
{
	struct index_fibpq_node *ret;

	if(node->next == node) {
		node->prev = NULL;
		node->next = NULL;
		return NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		
		ret = node->next;
		node->prev = NULL;
		node->next = NULL;
		
		if(head == node)
			return ret;
		else
			return head;
	}
}

/* 
 * Removes a Node from its parent's child list and 
 * insert it in the root list. 
 * If the parent Node already lost a child, 
 * reshapes the heap accordingly. 
 */
static void 
cut_index(struct index_fibpq *fpq, unsigned long i)
{
	struct index_fibpq_node *current, *pparent;

	current = fpq->nodes[i];
	pparent = current->parent;
	pparent->child = cut_node(current, pparent->child);
	current->parent = NULL;
	pparent->degree--;
	fpq->head = insert_node(current, fpq->head);
	pparent->mark = !pparent->mark;

	if(!pparent->mark && pparent->parent != NULL)
		cut_index(fpq, pparent->index);
}

/* 
 * Merges two root lists together, 
 * root1 becomes new the root. 
 */
static struct index_fibpq_node * 
meld_node(struct index_fibpq_node *root1, struct index_fibpq_node *root2)
{
	if(root1 == NULL)
		return root2;
	else if(root2 == NULL)
		return root1;
	else {
		if(root2->next == NULL && root2->prev == NULL)
			return root1;
		if(root1->prev == NULL && root1->next == NULL)
			return root2;

		root1->prev->next = root2->next;
		root2->next->prev = root1->prev;
		root1->prev = root2;
		root2->next = root1;
		return root1;
	}
}

/* 
 * Assuming root1 holds a greater or less key than root2,
 * root2 becomes the new root.
 */
static inline void 
link_node(struct index_fibpq_node *root1, struct index_fibpq_node *root2)
{
	root1->parent = root2;
	root2->child = insert_node(root1, root2->child);
	root2->degree++;
}

/* Coalesce the roots, thus reshapes the tree. */
static void 
consolidate(struct index_fibpq *fpq)
{
	unsigned int maxdegrees = 0, i, d;
	struct index_fibpq_node **rootab;
	struct index_fibpq_node *current, *rootptr;
	struct index_fibpq_node *root, *node;

	maxdegrees = (int)ceil(log2((double)fpq->size));
	rootab = (struct index_fibpq_node **)
		algmalloc((maxdegrees + 1) * sizeof(struct index_fibpq_node *));
	for(i = 0; i <= maxdegrees; i++)
		rootab[i] = NULL;
	
	current = fpq->head;
	fpq->result = fpq->head;
	rootptr = NULL;
	root = NULL;
	do {
		rootptr = current;
		current = current->next;
		
		/* 
		 * Consolidate trees so that no two 
		 * roots have same degree.
		 */
		d = rootptr->degree;
		while(rootab[d] != NULL) {
			root = rootab[d];
			if(fpq->cmp(rootptr->key, root->key)) {
				node = rootptr;
				rootptr = root;
				root = node;
			}
			link_node(root, rootptr);
			rootab[d++] = NULL;
		}
		
		rootab[d] = rootptr;
	} while(current != fpq->head);
	
	/* reshapes the tree using root lists heap */
	fpq->head = NULL;
	for(i = 0; i <= maxdegrees; i++)
		if((node = rootab[i]) != NULL) {
			fpq->result = fpq->cmp(fpq->result->key, node->key)
				? node : fpq->result;
			fpq->head = insert_node(node, fpq->head);
		}
		
	ALGFREE(rootab);
}

static void 
traverse(const struct index_fibpq_node *node, struct queue *keys,
		struct queue *indexes)
{
	const struct index_fibpq_node *current;
	
	current = node;
	do {
		enqueue(keys, current->key);
		enqueue(indexes, &(current->index));
		if(current->child != NULL)
			traverse(current->child, keys, indexes);
		current = current->next;
	} while(current != node);
}

static void 
release_node(struct index_fibpq_node *node, unsigned int ksize)
{
	struct index_fibpq_node *current, *next;
	
	current = node;
	do {
		next = current->next;
		if(ksize != 0)
			ALGFREE(current->key);
		if(current->child != NULL)
			release_node(current->child, ksize);
		ALGFREE(current);
		current = next;
	} while(node != next);
}
