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
#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#include "algcomm.h"

struct avl_node {
	void *key;				/* key contained by the Node */
	struct avl_node *left;	/* left subtree */
	struct avl_node *right;	/* right subtree */
	long height;			/* height of the subtree */
	unsigned long size;		/* number of nodes in subtree */
};

struct avl_tree {
	struct avl_node *root;	/* AVL tree root node */
	unsigned int keysize;	/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* Returns the number of key-value pairs in this AVL tree. */
#define AVLBST_SIZE(avl)	\
	((avl)->root == NULL ? 0 : (avl)->root->size)

/* 
 * Returns the height of the internal AVL tree.
 * It is assumed that the height of an empty tree is -1 and the height of a
 * tree with just one node is 0.
 */
#define AVLBST_HEIGHT(avl)	\
	((avl)->root == NULL ? (-1) : (avl)->root->height)

/* Checks if the AVL tree is empty. */
#define AVLBST_ISEMPTY(avl)	((avl)->root == NULL ? 1 : 0)

struct single_list;

/* Initializes an empty AVL tree */
void avlbst_init(struct avl_tree *avl, unsigned int ksize, algcomp_ft *cmp);

/* Returns the key in this avl tree by the given key. */
void * avlbst_get(const struct avl_tree *avl, const void *key);

/* Inserts the key into the AVL tree. */
void avlbst_put(struct avl_tree *avl, const void *key);

/* Clears this avl tree. */
void avlbst_clear(struct avl_tree *avl);

/* Traverses for preorder */
void avlbst_preorder(const struct avl_tree *avl, struct single_list *keys);

/* Returns the smallest key in the AVL BST. */
void * avlbst_min(const struct avl_tree *avl);

/* Returns the largest key in the AVL BST. */
void * avlbst_max(const struct avl_tree *avl);

/* Removes the smallest key from this AVL tree. */
void avlbst_delete_min(struct avl_tree *avl);

/* Removes the largest key from this AVL tree. */
void avlbst_delete_max(struct avl_tree *avl);

/* Removes the specified key from the AVL tree. */
int avlbst_delete(struct avl_tree *avl, const void *key);

/* Returns the largest key in the AVL tree less than or equal to Key. */
void * avlbst_floor(const struct avl_tree *avl, const void *key);

/* Returns the smallest key in the AVL tree greater than or equal to Key. */
void * avlbst_ceiling(const struct avl_tree *avl, const void *key);

/* Returns the number of keys in the AVL BST strictly less than Key. */
unsigned long avlbst_rank(const struct avl_tree *avl, const void *key);

/* Return the key in the AVL BST of a given rank. */
void * avlbst_select(const struct avl_tree *avl, unsigned long rank);

/* Returns all keys in the AVL tree in the given range. */
void avlbst_keys(const struct avl_tree *avl, const void *lokey,
				const void *hikey, struct single_list *keys);

/* Breadth first search traverse for this avl tree. */
void avlbst_breadth_first(const struct avl_tree *avl, struct single_list *keys);

/* Check integrity of AVL tree data structure. */
int avlbst_check(const struct avl_tree *avl);

#endif /*_AVLTREE_H_ */
