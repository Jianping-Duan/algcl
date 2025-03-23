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
#ifndef _BSEARCHTREE_H_
#define _BSEARCHTREE_H_

#include "algcomm.h"

struct bst_node {
	struct element item;		/* key-value pairs */
	struct bst_node *left;		/* left subtrees */
	struct bst_node *right;		/* right subtrees */
	unsigned long size;			/* number of nodes in subtree */
	long height;				/* height of the subtree */
};

struct bsearch_tree {
	struct bst_node *root;
};

/* Returns the number of key-value pairs in this BST */
#define BST_SIZE(b)	\
	((b)->root == NULL ? 0 : (b)->root->size)

/* Returns the 1 if this BST empty */
#define BST_ISEMPTY(b)	(BST_SIZE(b) == 0 ? 1 : 0)

/* 
 * Returns the height of the internal BST.
 * It is assumed that the height of an empty tree is -1 
 * and the height of a tree with just one node is 0.
 */
#define BST_HEIGHT(bst)	\
	((bst)->root == NULL ? (-1) : (bst)->root->height)

/* Initializes an empty binary search tree */
#define BST_INIT(b)	((b)->root = NULL)

struct queue;

/* Inserts the specified key-value pair into the BST */
void bst_put(struct bsearch_tree *bst,	const struct element *item);

/* Returns the value associated with the given key. */
struct element * bst_get(const struct bsearch_tree *bst, const char *key);

/* Prints all elements using previous order traverse. */
void bst_preorder(const struct bsearch_tree *bst, 
				void (*print)(const struct element *el));

/* Releases this binary search tree */
void bst_clear(struct bsearch_tree *bst);

/* Returns the smallest key is the BST */
char * bst_min(const struct bsearch_tree *bst);

/* Returns the largest key is the BST */
char * bst_max(const struct bsearch_tree *bst);

/* 
 * Removes the smallest key and associated 
 * with value from the BST.
 */
void bst_delete_min(struct bsearch_tree *bst);

/* 
 * Removes the largest key and associated 
 * with value from the BST.
 */
void bst_delete_max(struct bsearch_tree *bst);

/* 
 * Removes the specified key and its 
 * associated value from the BST.
 */
void bst_delete(struct bsearch_tree *bst, const char *key);

/* 
 * Return the number of keys in the BST strictly 
 * less than the specified key.
 */
unsigned long bst_rank(const struct bsearch_tree *bst, const char *key);

/* Return the key in the BST of a given rank. */
struct element * bst_select(const struct bsearch_tree *bst, 
							unsigned long rank);

/* 
 * Returns the largest key in the BST less than 
 * or equal to the specified key. 
 */
struct element * bst_floor(const struct bsearch_tree *bst, const char *key);

/* 
 * Returns the smallest key in the BST greater than 
 * or equal to the specified key. 
 */
struct element * bst_ceiling(const struct bsearch_tree *bst, const char *key);

/* Returns the number of leaf nodes */
long bst_leaf_nodes(const struct bsearch_tree *bst);

/* Breadth-first search traverse. */
void bst_breadth_first(const struct bsearch_tree *bst, struct queue *keys);

/* 
 * Gets all keys in the BST in the given range in ascending order.
 */
void bst_keys(const struct bsearch_tree *bst, const char *lokey,
			const char *hikey, struct queue *keys);

/* Check integrity of BST data structure. */
int bst_check(const struct bsearch_tree *bst);

#endif /* _BSEARCHTREE_H_ */
