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
#ifndef _REDBLACKBST_H_
#define _REDBLACKBST_H_

#include "algcomm.h"

enum bst_redblack {RED, BLACK};

struct rbtree_node {
	void *key;			/* key contained by the Node */
	struct rbtree_node *left;	/* link to left subtrees */
	struct rbtree_node *right;	/* link to right subtrees */
	enum bst_redblack color;	/* color for parent link */
	unsigned long size;		/* subtrees count */
	long height;			/* height of the subtree */
};

struct rbtree {
	struct rbtree_node *root;	/* root node */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* Returns the number of keys in this Red-Black BST. */
#define RBBST_SIZE(bst)	\
	((bst)->root == NULL ? 0 : (bst)->root->size)

/* 
 * Returns the height of the internal Red-Black tree.
 * It is assumed that the height of an empty tree is -1 
 * and the height of a tree with just one node is 0. 
 */
#define RBBST_HEIGHT(bst)	\
	((bst)->root == NULL ? (-1) : (bst)->root->height)

/* Is this Red Black BST empty ? */
#define RBBST_ISEMPTY(bst)	((bst)->root == NULL)

struct single_list;

/* Initializes an empty Red-Black binary search tree. */
void rbbst_init(struct rbtree *bst, unsigned int ksize, algcomp_ft *kcmp);

/* Returns Key associated with the given key */
void * rbbst_get(const struct rbtree *bst, const void *key);

/* Inserts the specified key into the Red-Black BST. */
int rbbst_put(struct rbtree *bst, const void *key);

/* Release the Red-Black tree */
void rbbst_clear(struct rbtree *bst);

/* Preorder traverse */
void rbbst_preorder(const struct rbtree *bst, struct single_list *keys);

/* Returns the smallest key in the Red-Black BST */
void * rbbst_min(const struct rbtree *bst);

/* Returns the largest key in the Red-Black BST */
void * rbbst_max(const struct rbtree *bst);

/* Remove the smallest key from this Red-Black BST. */
int rbbst_delete_min(struct rbtree *bst);

/* Remove the largest key from this Red-Black BST. */
int rbbst_delete_max(struct rbtree *bst);

/* Removes the specified key from this Red-Black BST. */
int rbbst_delete(struct rbtree *bst, const void *key);

/* Check integrity of red-black tree data structure. */
int rbbst_check(const struct rbtree *bst);

/* 
 * Returns the largest key in the Red-Black tree less than
 * or equal to Key.
 */
void * rbbst_floor(const struct rbtree *bst, const void *key);

/* 
 * Returns the smallest key in the Red-Black tree greater than
 * or equal to Key.
 */
void * rbbst_ceiling(const struct rbtree *bst, const void *key);

/* Return the number of keys in the Red-Black BST strictly less than Key. */
unsigned long rbbst_rank(const struct rbtree *bst, const void *key);

/* Return the key in the BST of a given rank. */
void * rbbst_select(const struct rbtree *bst, unsigned long rank);

/* 
 * Returns all keys in the Red-Black BST in the
 * given range in ascending order.
 */
void rbbst_keys(const struct rbtree *bst, const void *lokey, const void *hikey,
		struct single_list *keys);

#endif /* _REDBLACKBST_H_ */
