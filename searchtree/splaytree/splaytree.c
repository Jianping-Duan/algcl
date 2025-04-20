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
#include "splaytree.h"
#include "singlelist.h"

static struct splayt_node * make_node(const void *, unsigned int);
static inline void rotate_left(struct splay_tree *, struct splayt_node *);
static inline void rotate_right(struct splay_tree *, struct splayt_node *);
static struct splayt_node * splay(struct splay_tree *, struct splayt_node *);
static struct splayt_node * find(const struct splay_tree *, const void *);
static struct splayt_node * min_node(struct splayt_node *);
static struct splayt_node * max_node(struct splayt_node *);
static void replace(struct splay_tree *, struct splayt_node *,
	struct splayt_node *);
static void clear(struct splayt_node *, unsigned int);
static void preorder(const struct splayt_node *x, struct single_list *elset);

/* Initializes an empty splay tree. */
void
splayt_init(struct splay_tree *st, unsigned int ksize, algcomp_ft *cmp)
{
	st->root = NULL;
	st->size = 0;
	st->keysize = ksize;
	st->cmp = cmp;
}

/* 
 * To insert a value key into a splay tree:
 * Insert key as with a normal binary search tree.
 * When a key is inserted, a splay is performed.
 * As a result, the newly inserted node x becomes the root of the tree.
 */
int
splayt_put(struct splay_tree *st, const void *key)
{
	struct splayt_node *current, *pnode = NULL;

	current = st->root;
	while (current != NULL) {
		pnode = current;
		if (st->cmp(current->key, key) == 1)
			current = current->right;
		else if (st->cmp(current->key, key) == -1)
			current = current->left;
		else 
			/* st->cmp(current->key, key) == 0 */
			return 1;
	}

	current = make_node(key, st->keysize);
	current->parent = pnode;

	if (pnode == NULL)
		st->root = current;
	else if (st->cmp(pnode->key, current->key) == 1)
		pnode->right = current;
	else if (st->cmp(pnode->key, current->key) == -1)
		pnode->left = current;
	else {
		ALGFREE(current);
		return 1;
	}
	current->parent = pnode;

	st->root = splay(st, current);
	st->size++;

	return 0;
}

/* Returns the key in this splay tree by the given key. */
void * 
splayt_get(struct splay_tree *st, const void *key)
{
	struct splayt_node *current;
	void *el;

	if (st->cmp(st->root->key, key) == 0)
		return (st->root->key);

	if((current = find(st, key)) == NULL)
		return NULL;
	else {
		el = current->key;
		st->root = splay(st, current);
		return el;
	}
}

/* 
 * To delete a node x, use the same method as with a binary search tree:
 *
 * If x has two children:
 * Swap its value with that of either the rightmost node of its left sub tree
 * (its in-order predecessor) or theleftmost node of its right subtree 
 * (its in-order successor).
 * Remove that node instead.
 *
 * In this way, deletion is reduced to the problem of removing a node with 0 or
 * 1 children. Unlike a binary searchtree, in a splay tree after deletion,
 * we splay the parent of the removed node to the top of the tree.
 */
int
splayt_delete(struct splay_tree *st, const void *key)
{
	struct splayt_node *current, *min;

	if((current = find(st, key)) == NULL)
		return 1;
	
	splay(st, current);

	if (current->left == NULL)
		replace(st, current, current->right);
	else if (current->right == NULL)
		replace(st, current, current->left);
	else {
		min = min_node(current->right);
		if (min->parent != current) {
			replace(st, min, min->right);
			min->right = current->right;
			min->right->parent = current;
		}
		replace(st, current, min);
		min->left = current->left;
		min->left->parent = current;
	}

	if (st->keysize != 0)
		ALGFREE(current->key);
	ALGFREE(current);
	st->size--;

	return 0;
}

/* Returns the smallest key in the splay tree.*/
void * 
splayt_min(const struct splay_tree *st)
{
	if (SPLAYT_ISEMPTY(st))
		return NULL;
	return min_node(st->root)->key;
}

/* Returns the largest key in the splay tree. */
void * 
splayt_max(const struct splay_tree *st)
{
	if (SPLAYT_ISEMPTY(st))
		return NULL;
	return max_node(st->root)->key;
}

/* Gets all items from this splay tree. */
void
splayt_preorder(const struct splay_tree *st, struct single_list *keys)
{
	slist_init(keys, 0, st->cmp);

	if (SPLAYT_ISEMPTY(st))
		return;
	preorder(st->root, keys);
}

/* Clears this splay tree. */
void 
splayt_clear(struct splay_tree *st)
{
	if (!SPLAYT_ISEMPTY(st)) {
		clear(st->root, st->keysize);
		st->size = 0;
		st->keysize = 0;
		st->cmp = NULL;
	}
}

/******************** static function boundary ********************/

/* 
 * Create Splay BST node using the specified key. 
 */
static struct splayt_node * 
make_node(const void *key, unsigned int ksize)
{
	struct splayt_node *current;
	
	current = (struct splayt_node *)algmalloc(sizeof(struct splayt_node));
	
	current->left = NULL;
	current->right = NULL;
	current->parent = NULL;
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;
	
	return current;
}

/* Make a right-leaning link lean to the left */
static inline void 
rotate_left(struct splay_tree *st, struct splayt_node *hnode)
{
	struct splayt_node *lnode;

	lnode = hnode->right;
	if (lnode != NULL) {
		hnode->right = lnode->left;
		/* links lnode parent pointer to hnode */
		if (lnode->left != NULL)
			lnode->left->parent = hnode; 
		lnode->parent = hnode->parent;
	}

	/* special case */
	if (hnode->parent == NULL) 
		st->root = lnode;
	/* links hnode parent pointer to lnode */
	else if (hnode == hnode->parent->left)
		hnode->parent->left = lnode;
	else
		hnode->parent->right = lnode;

	if (lnode != NULL)
		lnode->left = hnode;
	hnode->parent = lnode;
}

/* Make a left-leaning link lean to the right */
static inline void 
rotate_right(struct splay_tree *st, struct splayt_node *hnode)
{
	struct splayt_node *lnode;

	lnode = hnode->left;
	if (lnode != NULL) {
		hnode->left = lnode->right;
		if (lnode->right != NULL)
			lnode->right->parent = hnode;
		lnode->parent = hnode->parent;
	}

	if (hnode->parent == NULL)
		st->root = lnode;
	else if (hnode == hnode->parent->left)
		hnode->parent->left = lnode;
	else
		hnode->parent->right = lnode;

	if (lnode != NULL)
		lnode->right = hnode;
	hnode->parent = lnode;
}

/* 
 * When a node x is accessed, a splay operation is performe on x to move it to
 * the root. 
 * To perform a splayoperation we carry out a sequence of splay steps, each of
 * which moves x closer to the root. 
 * By performing asplay operation on the node of interest after every access,
 * the recently accessed nodes are kept near the root andthe tree remains
 * roughly balanced, so that we achieve the desired amortized time bounds.
 *
 * Each particular step depends on three factors:
 * Whether x is the left or right child of its parent node, p whether p is the
 * root or not, and if not whether p is the left or right child of its parent,
 * g (the grandparent of x).
 *
 * It is important to remember to set gg (the great-grandparent of x) to now
 * point to x after any splay operation.If gg is null, then x obviously is now
 * the root and must be updated as such.
 *
 * There are three types of splay steps, each of which has two symmetric
 * variants: left- and right-handed. 
 * For thesake of brevity, only one of these two is shown for each type. 
 * (In the following diagrams, circles indicate nodesof interest and triangles
 * indicate sub-trees of arbitrary size.)
 *
 * The three types of splay steps are:
 *
 * Zig step: this step is done when p is the root.
 * The tree is rotated on the edge between x and p.
 * Zig steps exist todeal with the parity issue, will be done only as the last
 * step in a splay operation, and only when x has odd depthat the beginning of
 * the operation.
 *
 * Zig-zig step: this step is done when p is not the root and x and p are
 * either both right children or are both leftchildren. 
 * The picture below shows the case where x and p are both left children. 
 * The tree is rotated on the edgejoining p with its parent g,
 * then rotated on the edge joining x with p.
 * Note that zig-zig steps are the only thingthat differentiate splay trees
 * from the rotate to root method introduced by Allen and Munro [5] prior 
 * to the introduction of splay trees.
 *
 * Zig-zag step: this step is done when p is not the root and x is a right
 * child and p is a left child or vice versa (x isleft, p is right).
 * The tree is rotated on the edge between p and x, 
 * and then rotated on the resulting edge between x and g.
 */
static struct splayt_node *
splay(struct splay_tree *st, struct splayt_node *x)
{
	while (x->parent != NULL) {
		if (x->parent->parent == NULL) {
			if (x->parent->left == x)
				rotate_right(st, x->parent);
			else
				rotate_left(st, x->parent);
		} else {
			if (x->parent->left == x && x->parent->parent->left == x->parent) {
				rotate_right(st, x->parent->parent);
				rotate_right(st, x->parent);
			} else if (x->parent->right == x && 
				x->parent->parent->right == x->parent) {
				rotate_left(st, x->parent->parent);
				rotate_left(st, x->parent);
			} else if(x->parent->left == x && 
				x->parent->parent->right == x->parent) {
				rotate_right(st, x->parent);
				rotate_left(st, x->parent);
			} else {
				rotate_left(st, x->parent);
				rotate_right(st, x->parent);
			}
		}
	}

	return x;
}

/* the search for BST */
static struct splayt_node * 
find(const struct splay_tree *st, const void *key)
{
	struct splayt_node *current;

	current = st->root;
	while (current != NULL)
		if (st->cmp(current->key, key) == 1)
			current = current->right;
		else if (st->cmp(current->key, key) == -1)
			current = current->left;
		else
			return current;
	return NULL;
}

/* 
 * Returns the smallest node in the subtree rooted at X; 
 * null if no search node. 
 */
static struct splayt_node * 
min_node(struct splayt_node *x)
{
	struct splayt_node *current = x;

	while (current->left != NULL)
		current = current->left;
	return current;
}

/* 
 * Returns the largest node in the subtree rooted at X;
 * null if no search node. 
 */
static struct splayt_node * 
max_node(struct splayt_node *x)
{
	struct splayt_node *current = x;

	while (current->right != NULL)
		current = current->right;
	return current;
}

static void 
replace(struct splay_tree *st, struct splayt_node *x, struct splayt_node *y)
{
	if (x->parent == NULL)
		st->root = y;
	else if (x->parent->left == x)
		x->parent->left = y;
	else
		x->parent->right = y;

	if (y != NULL)
		y->parent = x->parent;
}

static void 
clear(struct splayt_node *x, unsigned int ksize)
{
	if (x != NULL) {
		clear(x->left, ksize);
		clear(x->right, ksize);
		if (ksize != 0)
			ALGFREE(x->key);
		ALGFREE(x);
	}
}

static void
preorder(const struct splayt_node *x, struct single_list *keys)
{
	if (x != NULL) {
		slist_append(keys, x->key);
		preorder(x->left, keys);
		preorder(x->right, keys);
	}
}
