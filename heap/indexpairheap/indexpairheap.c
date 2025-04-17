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
#include "indexpairheap.h"
#include "singlelist.h"

static struct index_pheap_node * make_node(unsigned long, const void *,
	unsigned int);
static struct index_pheap_node * compare_link(const struct index_pheap *,
	struct index_pheap_node *, struct index_pheap_node *);
static struct index_pheap_node * combine_siblings(const struct index_pheap *,
	struct index_pheap_node *, unsigned long);
static void traverse(const struct index_pheap_node *, struct single_list *,
	struct single_list *);
static void release_node(struct index_pheap_node *, unsigned int);
static inline void detach_node(struct index_pheap_node *);

/* Initializes an empty indexed pairing heap. */
void 
ipheap_init(struct index_pheap *iph, unsigned long sz, unsigned int ksize,
			algcomp_ft *cmp)
{
	unsigned long i;

	iph->root = NULL;
	iph->capacity = sz;
	iph->size = 0;
	iph->keysize = ksize;
	iph->cmp = cmp;

	iph->nodes = (struct index_pheap_node **)
		algmalloc(sz * sizeof(struct index_pheap_node *));
	for (i = 0; i < sz; i++)
		iph->nodes[i] = NULL;
}

/* Associates a key with an index. */
void
ipheap_insert(struct index_pheap *iph, unsigned long ind, const void *key)
{
	struct index_pheap_node *curr;

	curr = make_node(ind, key, iph->keysize);
	if (iph->root == NULL)
		iph->root = curr;
	else
		iph->root = compare_link(iph, iph->root, curr);
	iph->nodes[ind] = curr;
	iph->size++;
}

/* Deletes the minimum or maximum key from this pairing-heap. */
unsigned long
ipheap_delete(struct index_pheap *iph)
{
	struct index_pheap_node *curr;
	unsigned long ind;

	if (IPHEAP_ISEMPTY(iph))
		errmsg_exit("The indexed pairing heap is empty.\n");

	curr = iph->root;
	ind = curr->index;

	if (curr->child != NULL) {
		iph->root = combine_siblings(iph, curr->child, curr->degree);
		if (iph->keysize != 0)
			ALGFREE(curr->key);
		ALGFREE(curr);
	}
	/* else only root node */

	iph->nodes[ind] = NULL;
	iph->size--;

	return ind;
}

/* Traverses this indexed pairing heap. */
void 
ipheap_traverse(const struct index_pheap *iph, struct single_list *keys,
				struct single_list *indexes)
{
	if (IPHEAP_ISEMPTY(iph))
		return;

	slist_init(keys, iph->keysize, NULL);
	slist_init(indexes, iph->keysize, NULL);
	traverse(iph->root, keys, indexes);
}

/* Clears this indexed pairing heap. */
void
ipheap_clear(struct index_pheap *iph)
{
	if (IPHEAP_ISEMPTY(iph))
		return;

	release_node(iph->root, iph->keysize);
	iph->capacity = 0;
	iph->size = 0;
	ALGFREE(iph->nodes);
}

/* 
 * Deletes the key associated the given index, 
 * worst case is O(log(n)) (amortized). 
 */
int
ipheap_remove(struct index_pheap *iph, unsigned long ind)
{
	struct index_pheap_node *curr, *subroot;

	if (IPHEAP_ISEMPTY(iph))
		return -1;
	if (!IPHEAP_CONTAINS(iph, ind))
		return -2;

	curr = iph->nodes[ind];
	if (curr == iph->root) {
		assert(ipheap_delete(iph) == ind);
		return 0;
	}

	/* detach the node to be deleted. */
	detach_node(curr);

	/* Merge the separated subtree heap. */
	if (curr->child != NULL) {
		subroot = combine_siblings(iph, curr->child, curr->degree);
		iph->root = compare_link(iph, iph->root, subroot);
	}

	if (iph->keysize != 0)
		ALGFREE(curr->key);
	ALGFREE(curr);
	iph->nodes[ind] = NULL;
	iph->size--;

	return 0;
}

/* 
 * Decreases the key associated with index i to the given key. 
 * worst case is O(1) (amortized).
 */
int
ipheap_decrkey(struct index_pheap *iph, unsigned long ind, const void *key)
{
	struct index_pheap_node *curr;

	if (IPHEAP_ISEMPTY(iph))
		return -1;
	if (!IPHEAP_CONTAINS(iph, ind))
		return -2;

	curr = iph->nodes[ind];
	if (iph->cmp(curr->key, key) == 1 || iph->cmp(curr->key, key) == 0)
		return -3;

	detach_node(curr);

	if (iph->keysize != 0)
		memcpy(curr->key, key, iph->keysize);
	else
		curr->key = (void *)key;

	iph->root = compare_link(iph, iph->root, curr);

	return 0;
}

/* 
 * Increases the key associated with index i to the given key.
 * worst case is O(1) (amortized).
 */
int
ipheap_incrkey(struct index_pheap *iph, unsigned long ind, const void *key)
{
	struct index_pheap_node *curr;

	if (IPHEAP_ISEMPTY(iph))
		return -1;
	if (!IPHEAP_CONTAINS(iph, ind))
		return -2;

	curr = iph->nodes[ind];
	if (iph->cmp(key, curr->key) == 1 || iph->cmp(curr->key, key) == 0)
		return -3;

	if (ipheap_remove(iph, ind) != 0)
		return -4;
	ipheap_insert(iph, ind, key);

	return 0;
}

/* 
 * Changes the key associated with index i to the given key.
 * worst case is O(1) (amortized) -- O(log(n)).
 */
int
ipheap_change(struct index_pheap *iph, unsigned long ind, const void *key)
{
	struct index_pheap_node *curr;

	if (IPHEAP_ISEMPTY(iph))
		return -1;
	if (!IPHEAP_CONTAINS(iph, ind))
		return -2;

	curr = iph->nodes[ind];

	if (iph->cmp(curr->key, key) == 0)
		return -3;
	if (iph->cmp(key, curr->key) == 1)
		return ipheap_decrkey(iph, ind, key);
	if (iph->cmp(curr->key, key) == 1) 
		return ipheap_incrkey(iph, ind, key);

	return 1;
}

/******************** static function boundary ********************/

static struct index_pheap_node *
make_node(unsigned long ind, const void *key, unsigned int ksize)
{
	struct index_pheap_node *curr;

	curr = (struct index_pheap_node *)
		algmalloc(sizeof(struct index_pheap_node));

	if (ksize != 0) {
		curr->key = algmalloc(ksize);
		memcpy(curr->key, key, ksize);
	} else
		curr->key = (void *)key;

	curr->degree = 0;
	curr->index = ind;
	curr->child = NULL;
	curr->sibling = NULL;
	curr->prev = NULL;

	return curr;
}

/* 
 * Links first and second together to satisfy heap order.
 * Returns the resulting tree.
 * First is assumed NOT NULL.
 * First->sibling MUST be NULL on entry. 
 */
static struct index_pheap_node * 
compare_link(const struct index_pheap *ph, struct index_pheap_node *first,
			struct index_pheap_node *second)
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
 * Two-pass merging.
 * Assumes First Sibling is NOT NULL.
 */
static struct index_pheap_node * 
combine_siblings(const struct index_pheap *ph, struct index_pheap_node *fsib,
				unsigned long maxdeg)
{
	unsigned long i, j, num = 0;
	struct index_pheap_node **forest, *curr;

	/* If only one tree, return it */
	if (fsib->sibling == NULL)
		return fsib;

	forest = (struct index_pheap_node **)
		algmalloc(maxdeg * sizeof(struct index_pheap_node *));
	for (i = 0; i < maxdeg; i++)
		forest[i] = NULL;

	/* Place each subtree to forest */
	for (curr = fsib, i = 0; curr != NULL; i++) {
		forest[i] = curr;
		curr->prev->sibling = NULL;	/* break links */
		curr = curr->sibling;
		num++;
	}

	/* 
	 * Combine the subtrees two at a time.
	 * going left to right.
	 */
	for (i = 0; i + 1 < num; i += 2)
		forest[i] = compare_link(ph, forest[i], forest[i + 1]);

	/* 
	 * j has the result of the last compare_link
	 * if an odd number of trees, get the last one. 
	 */
	if ((j = i - 2) == num - 3)
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

	curr = forest[0];
	ALGFREE(forest);
	
	return curr;
}

/* Traverses the sub-heap rooted at Node */
static void 
traverse(const struct index_pheap_node *node, struct single_list *keys,
		struct single_list *indexes)
{
	if (node != NULL) {
		slist_append(keys, node->key);
		slist_append(indexes, &(node->index));
		traverse(node->child, keys, indexes);
		traverse(node->sibling, keys, indexes);
	}
}

/* Releases the sub-heap rooted at Node */
static void 
release_node(struct index_pheap_node *node, unsigned int ksize)
{
	if (node != NULL) {
		release_node(node->child, ksize);
		release_node(node->sibling, ksize);
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
	}
}

static inline void
detach_node(struct index_pheap_node *ptr)
{
	if (ptr->sibling != NULL)
		ptr->sibling->prev = ptr->prev;
	if (ptr->prev != NULL) {
		if (ptr->prev->child == ptr)	/* the first node */
			ptr->prev->child = ptr->sibling;
		else
			ptr->prev->sibling = ptr->sibling;
	}
	ptr->sibling = NULL;
	ptr->prev = NULL;
}
