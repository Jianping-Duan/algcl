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
#include "fibonaccipq.h"
#include "queue.h"
#include <math.h>	/* log2(), ceil() */

static struct fibonacci_node * make_node(const void *, unsigned int);
static struct fibonacci_node * insert_node(struct fibonacci_node *,
	struct fibonacci_node *);
static struct fibonacci_node * cut_node(struct fibonacci_node *,
	struct fibonacci_node *);
static struct fibonacci_node * meld_node(struct fibonacci_node *,
	struct fibonacci_node *);
static inline void link_node(struct fibonacci_node *, struct fibonacci_node *);
static void consolidate(struct fibonaccipq *);
static void traverse(const struct fibonacci_node *, struct queue *);
static void release_node(struct fibonacci_node *, unsigned int);

/* 
 * Initializes an empty fibonacci priority queue, 
 * worst case is O(1). 
 */
void 
fibpq_init(struct fibonaccipq *fpq, unsigned int ksize, algcomp_ft *cmp)
{
	fpq->head = NULL;
	fpq->result = NULL;
	fpq->size = 0;
	fpq->keysize = ksize;
	fpq->cmp = cmp;
}

/* 
 * Merges two heaps together, this operation is 
 * destructive, worst case is O(1). 
 */
struct fibonaccipq * 
fibpq_unionfpq(struct fibonaccipq *sfpq, const struct fibonaccipq *tfpq)
{
	/* concatenate the two root lists */
	sfpq->head = meld_node(sfpq->head, tfpq->head);
	
	/* update the minimum or maximum pointer */
	sfpq->result = sfpq->cmp(sfpq->result->key, tfpq->result->key) 
		? tfpq->result : sfpq->result;
	
	/* update the size heap */
	sfpq->size += tfpq->size;
	
	return sfpq;
}

/* Insert a key in the queue, worst case is O(1). */
void 
fibpq_insert(struct fibonaccipq *fpq, const void *key)
{
	struct fibonacci_node *current;
	
	current = make_node(key, fpq->keysize);

	/* 
	 * Create a new singleton tree,
	 * the head is always a new node.
	 */
	fpq->head = insert_node(current, fpq->head);
	fpq->size++;
	
	/* update minimum or maximum pointer */
	if(fpq->result == NULL)
		fpq->result = fpq->head;
	else {
		fpq->result = fpq->cmp(fpq->result->key, key)
			? fpq->head : fpq->result;
	}
}

/* 
 * Deletes the minimum or maximum key, 
 * worst case is O(log(n)) (amortized) 
 */
void * 
fibpq_delete(struct fibonaccipq *fpq)
{
	struct fibonacci_node *child;
	void *key;
	
	if(FIBPQ_ISEMPTY(fpq))
		return NULL;
	
	fpq->head = cut_node(fpq->result, fpq->head);
	key = fpq->result->key;
	child = fpq->result->child;
	
	if(child != NULL) {
		/* concatenate its children nodes into root list */
		fpq->head = meld_node(fpq->head, child);
		/* detach child pointer */
		fpq->result->child = NULL;
	}
	
	ALGFREE(fpq->result);
	fpq->size--;
	
	if(!FIBPQ_ISEMPTY(fpq))
		consolidate(fpq);	/* reshapes the tree */
	else
		fpq->result = NULL;	/* only one node */

	return key;
}

void 
fibpq_keys(const struct fibonaccipq *fpq, struct queue *keys)
{
	if(FIBPQ_ISEMPTY(fpq))
		return;
	traverse(fpq->head, keys);
}

void
fibpq_clear(struct fibonaccipq *fpq)
{
	if(FIBPQ_ISEMPTY(fpq))
		return;
	
	release_node(fpq->head, fpq->keysize);
	fpq->size = 0;
}

/******************** static function boundary ********************/

/* 
 * Creates fibonacci priority queue node using 
 * the specified key and its bytes. 
 */
static struct fibonacci_node * 
make_node(const void *key, unsigned int ksize)
{
	struct fibonacci_node *current;

	current = (struct fibonacci_node *)algmalloc(sizeof(struct fibonacci_node));
	
	if(ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
	else
		current->key = (void *)key;
	
	current->degree = 0;
	current->prev = NULL;
	current->next = NULL;
	current->child = NULL;
	
	return current;
}

/* 
 * Inserts a Node in a circular list containing head, 
 * returns a new head.
 */
static struct fibonacci_node * 
insert_node(struct fibonacci_node *node, struct fibonacci_node *head)
{
	if(head == NULL) {		/* links itself */
		node->prev = node;
		node->next = node;
	}
	else {
		if(head->prev == NULL && head->next == NULL) {
			node->prev = head;
			node->next = head;
			head->prev = node;
			head->next = node;
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
static struct fibonacci_node * 
cut_node(struct fibonacci_node *node, struct fibonacci_node *head)
{
	struct fibonacci_node *ret;
	
	if(node->next == node) {	/* only one node */
		node->prev = NULL;
		node->next = NULL;
		return NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		
		ret = node->next;
		/* detach node */
		node->prev = NULL;
		node->next = NULL;
		
		if(head == node) {
			/* 
			 * ALGFREE(node);
			 * free delay in order to get its children nodes.
			 */
			return ret;
		}
		else {
			/* ALGFREE(node); */
			return head;
		}
	}
}

/* 
 * Merges two root lists together, 
 * root1 becomes new the root. 
 */
static struct fibonacci_node * 
meld_node(struct fibonacci_node *root1, struct fibonacci_node *root2)
{
	if(root1 == NULL)
		return root2;
	else if(root2 == NULL)
		return root1;
	else {
		if(root2->next == NULL && root2->prev == NULL) {
			if(root1->prev != NULL) {
				assert(root1->next != NULL);
				root1->prev->next = root2;
				root2->next = root1;
				root2->prev = root1->prev;
				root1->prev = root2;
			}
			else {
				root1->next = root2;
				root2->prev = root1;
				root1->prev = root2;
				root2->next = root1;
			}
			return root1;
		}

		if(root1->prev == NULL && root1->next == NULL) {
			if(root2->prev != NULL) {
				assert(root2->next != NULL);
				root2->prev->next = root1;
				root1->next = root2;
				root1->prev = root2->prev;
				root2->prev = root1;
			}
			else {
				root2->next = root1;
				root1->prev = root2;
				root2->prev = root1;
				root1->next = root2;
			}
			return root2;
		}

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
link_node(struct fibonacci_node *root1, struct fibonacci_node *root2)
{
	root2->child = insert_node(root1, root2->child);
	root2->degree++;
}

/* Coalesce the roots, thus reshapes the tree. */
static void 
consolidate(struct fibonaccipq *fpq)
{
	unsigned int maxdegrees = 0, i, d;
	struct fibonacci_node **rootlist;
	struct fibonacci_node *current, *rootptr, *root, *node;
	
	maxdegrees = (int)ceil(log2((double)fpq->size));
	rootlist = (struct fibonacci_node **)
		algmalloc((maxdegrees + 1) * sizeof(struct fibonacci_node *));
	for(i = 0; i <= maxdegrees; i++)
		rootlist[i] = NULL;
	
	current = fpq->head;
	fpq->result = fpq->head;
	rootptr = NULL;
	root = NULL;
	do {
		if((rootptr = current) == NULL)
			break;
		current = current->next;
		
		/* 
		 * Consolidate trees so that no two 
		 * roots have same degree.
		 */
		d = rootptr->degree;
		while(rootlist[d] != NULL) {
			root = rootlist[d];
			if(fpq->cmp(rootptr->key, root->key)) {
				node = rootptr;
				rootptr = root;
				root = node;
			}
			link_node(root, rootptr);
			rootlist[d++] = NULL;
		}
		
		rootlist[d] = rootptr;
	} while(current != NULL && current != fpq->head);

	/* reshapes the tree using root lists heap */
	fpq->head = NULL;
	for(i = 0; i <= maxdegrees; i++)
		if((node = rootlist[i]) != NULL) {
			fpq->result = fpq->cmp(fpq->result->key, node->key)
				? node : fpq->result;
			fpq->head = insert_node(node, fpq->head);
		}
		
	ALGFREE(rootlist);
}

static void 
traverse(const struct fibonacci_node *node, struct queue *keys)
{
	const struct fibonacci_node *current;

	if(node == NULL)
		return;
	
	current = node;
	do {
		enqueue(keys, current->key);
		if(current->child != NULL)
			traverse(current->child, keys);
		current = current->next;
	} while(current != NULL && current != node);
}

static void 
release_node(struct fibonacci_node *node, unsigned int ksize)
{
	struct fibonacci_node *current, *next;

	if(node == NULL)
		return;
	
	current = node;
	do {
		next = current->next;
		if(ksize != 0)
			ALGFREE(current->key);
		if(current->child != NULL)
			release_node(current->child, ksize);
		ALGFREE(current);
		current = next;
	} while(current != NULL && node != next);
}
