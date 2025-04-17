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
#include "pairingheap.h"
#include "singlelist.h"

static struct pairing_node * make_node(const void *, unsigned int);
static struct pairing_node * compare_link(const struct pairing_heap *,
	struct pairing_node *, struct pairing_node *);
static struct pairing_node * combine_siblings(const struct pairing_heap *, 
	struct pairing_node *, unsigned long);
static void traverse(const struct pairing_node *, struct single_list *);
static void release_node(struct pairing_node *, unsigned int);

/* 
 * Initializes an empty priority queue using 
 * the given compare function and key-size. 
 */
void 
pheap_init(struct pairing_heap *ph, unsigned int ksize, algcomp_ft *cmp)
{
	ph->root = NULL;
	ph->size = 0;
	ph->keysize = ksize;
	ph->cmp = cmp;
}

/* 
 * Inserts a key in the heap, 
 * and maintaining the heap order. 
 */
void 
pheap_insert(struct pairing_heap *ph, const void *key)
{
	struct pairing_node *current;

	current = make_node(key, ph->keysize);
	if (ph->root == NULL)
		ph->root = current;
	else
		ph->root = compare_link(ph, ph->root, current);
	
	ph->size++;
}

/* 
 * Deletes the minimum or maximum key 
 * from this pairing-heap.
 */
void *
pheap_delete(struct pairing_heap *ph)
{
	struct pairing_node *current;
	void *key;

	if (PHEAP_ISEMPTY(ph))
		errmsg_exit("The pairing heap is empty.\n");
	
	current = ph->root;
	key = current->key;

	if (current->child != NULL)
		ph->root = combine_siblings(ph, current->child,	current->degree);
	else
		ph->root = NULL;

	ALGFREE(current);
	ph->size--;

	return key;
}

/* Gets all keys for this paring heap */
void 
pheap_keys(const struct pairing_heap *ph, struct single_list *keys)
{
	slist_init(keys, ph->keysize, NULL);
	if (!PHEAP_ISEMPTY(ph))
		traverse(ph->root, keys);
}

/* Clears this pairing heap */
void 
pheap_clear(struct pairing_heap *ph)
{
	if (!PHEAP_ISEMPTY(ph)) {
		release_node(ph->root, ph->keysize);
		ph->root = NULL;
		ph->size = 0;
		ph->keysize = 0;
		ph->cmp = NULL;
	}
}

/******************** static function boundary ********************/

/* 
 * Creates pairing priority queue node using 
 * the specified key and its bytes. 
 */
static struct pairing_node * 
make_node(const void *key, unsigned int ksize)
{
	struct pairing_node *current;

	current = (struct pairing_node *)algmalloc(sizeof(struct pairing_node));
	
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;

	current->degree = 0;
	current->child = NULL;
	current->sibling = NULL;
	current->prev = NULL;
	
	return current;
}

/* 
 * Links first and second together to satisfy heap order. 
 * Returns the resulting tree.
 * First is assumed NOT NULL.
 * First->sibling MUST be NULL on entry. 
 */
static struct pairing_node * 
compare_link(const struct pairing_heap *ph, struct pairing_node *first,
		  struct pairing_node *second)
{
	if (second == NULL)
		return first;
	else {
		if (ph->cmp(first->key, second->key) == 1 || 
		   ph->cmp(first->key, second->key) == 0) {
			/* 
			 * attach second as the leftmost child of first.
			 */
			second->prev = first;
			first->sibling = second->sibling;
			if (first->sibling != NULL)
				first->sibling->prev = first;
			second->sibling = first->child;
			if (second->sibling != NULL)
				second->sibling->prev = second;
			first->child = second;
			first->degree++;
			return first;
		} else {
			/* 
			 * attach first as the leftmost child of second.
			 */
			second->prev = first->prev;
			first->prev = second;
			first->sibling = second->child;
			if (first->sibling != NULL)
				first->sibling->prev = first;
			second->child = first;
			second->degree++;
			return second;
		}
	}
}

/* 
 * Two-pass merging 
 * Assumes First Sibling is NOT NULL
 */
static struct pairing_node * 
combine_siblings(const struct pairing_heap *ph, struct pairing_node *fsib,
				unsigned long deg)
{
	unsigned long i, j;
	struct pairing_node **forest, *current;

	/* If only one tree, return it */
	if (fsib->sibling == NULL)
		return fsib;

	forest = (struct pairing_node **)
		algmalloc(deg * sizeof(struct pairing_node *));
	for (i = 0; i < deg; i++)
		forest[i] = NULL;

	/* Place each subtree to forest */
	for (current = fsib, i = 0; current != NULL; i++) {
		forest[i] = current;
		current->prev->sibling = NULL;	/* break links */
		current = current->sibling;
	}
	assert(current == NULL && i == deg);

	/* 
	 * Combine the subtrees two at a time.
	 * going left to right.
	 */
	for (i = 0; i + 1 < deg; i += 2)
		forest[i] = compare_link(ph, forest[i], forest[i + 1]);

	/* 
	 * j has the result of the last compare_link
	 * if an odd number of trees, get the last one. 
	 */
	if ((j = i - 2) == deg - 3)
		forest[j] = compare_link(ph, forest[j], forest[j + 2]);

	/* 
	 * now go right to left, 
	 * merging last one tree with next to last.
	 * The result becomes the new last. 
	 */
	while (j >= 2) {
		forest[j - 2] = compare_link(ph, forest[j - 2], forest[j]);
		j -= 2;
	}

	current = forest[0];
	ALGFREE(forest);
	
	return current;
}

/* Traverses the sub-heap rooted at Node */
static void 
traverse(const struct pairing_node *node, struct single_list *keys)
{
	if (node != NULL) {
		slist_append(keys, node->key);
		traverse(node->child, keys);
		traverse(node->sibling, keys);
	}
}

/* Releases the sub-heap rooted at Node */
static void 
release_node(struct pairing_node *node, unsigned int ksize)
{
	if (node != NULL) {
		release_node(node->child, ksize);
		release_node(node->sibling, ksize);
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
	}
}
