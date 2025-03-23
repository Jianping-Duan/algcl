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

struct rbbst_node{
	struct element item;		/* key-value pair */
	struct rbbst_node *left;	/* link to left subtrees */
	struct rbbst_node *right;	/* link to right subtrees */
	enum bst_redblack color;	/* color for parent link */
	unsigned long size;			/* subtrees count */
	long height;				/* height of the subtree */
};

struct redblack_bst{
	struct rbbst_node *root;	/* root node */
};

/* Initializes an empty Red-Black binary search tree. */
#define RBBST_INIT(bst)	((bst)->root = NULL)

/* 
 * Returns the number of key-value pairs in 
 * this Red-Black BST.
 */
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

struct queue;

/* Returns item associated with the given key */
struct element * rbbst_get(const struct redblack_bst *bst, const char *key);

/* 
 * Inserts the specified key-value pair 
 * into the Red-Black BST.
 */
void rbbst_put(struct redblack_bst *bst, const struct element *item);

/* Release the Red-Black tree */
void rbbst_clear(struct redblack_bst *bst);

/* Preorder traverse */
void rbbst_preorder(const struct redblack_bst *bst, 
					void (*print)(struct element *item));

/* Returns the smallest key in the Red-Black BST */
char * rbbst_min(const struct redblack_bst *bst);

/* Returns the largest key in the Red-Black BST */
char * rbbst_max(const struct redblack_bst *bst);

/* 
 * Remove the smallest key associated with value from 
 * this Red-Black BST.
 */
void rbbst_delete_min(struct redblack_bst *bst);

/* 
 * Remove the largest key associated with value from 
 * this Red-Black BST.
 */
void rbbst_delete_max(struct redblack_bst *bst);

/* 
 * Removes the specified key and its associated with 
 * value from this Red-Black BST. 
 */
void rbbst_delete(struct redblack_bst *bst,	const char *key);

/* Check integrity of red-black tree data structure. */
int rbbst_check(const struct redblack_bst *bst);

/* 
 * Returns the largest key in the symbol table less than
 * or equal to Key.
 */
struct element * rbbst_floor(const struct redblack_bst *bst, 
							const char *key);

/* Returns the smallest key in the symbol table greater 
 * than or equal to Key.
 */
struct element * rbbst_ceiling(const struct redblack_bst *bst,
								const char *key);

/* 
 * Return the number of keys in the Red-Black BST 
 * strictly less than Key.
 */
unsigned long rbbst_rank(const struct redblack_bst *bst, 
						const char *key);

/* Return the key in the BST of a given rank. */
struct element * rbbst_select(const struct redblack_bst *bst, 
							unsigned long rank);

/* 
 * Returns all keys in the Red-Black BST in the 
 * given range in ascending order.
 */
void rbbst_keys(const struct redblack_bst *bst, const char *lokey,
				const char *hikey, struct queue *keys);

#endif /* _REDBLACKBST_H_ */
