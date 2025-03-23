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
#ifndef _SPLAYTREE_H_
#define _SPLAYTREE_H_

#include "algcomm.h"

/* splay tree node */
struct splayt_node {
	struct element item;		/* key-value pair */
	struct splayt_node *left;	/* link to left subtrees */
	struct splayt_node *right;	/* link to right subtrees */
	struct splayt_node *parent;	/* link to parent node */
};

/* structure of splay tree */
struct splay_tree {
	struct splayt_node *root;	/* root node */
	unsigned long size;			/* size of splay tree */
};

/* Initializes an empty splay binary search tree */
#define SPLAYT_INIT(st)		do {	\
	(st)->root = NULL;				\
	(st)->size = 0;					\
} while(0)

/* 
 * Returns the number of key-value pairs 
 * in this splay tree. 
 */
#define SPLAYT_SIZE(st)		((st)->size)

/* Is this Splay Tree empty? */
#define SPLAYT_ISEMPTY(st)	((st)->root == NULL)

struct single_list;

/* 
 * Inserts the specified key-value pair into the splay tree. 
 */
void splayt_put(struct splay_tree *st, const struct element *item);

/* Returns item associated with the given key */
struct element * splayt_get(struct splay_tree *st, const char *key);

/* 
 * Removes the specified key and its associated 
 * with value from this splay tree. 
 */
void splayt_delete(struct splay_tree *st, const char *key);

/* Returns the smallest key in the splay tree. */
char * splayt_min(const struct splay_tree *st);

/* Returns the largest key in the splay tree. */
char * splayt_max(const struct splay_tree *st);

/* Gets all items from this splay tree. */
void splayt_preorder(const struct splay_tree *st, struct single_list *elset);

/* Clears this splay tree. */
void splayt_clear(struct splay_tree *st);

#endif	/* _SPLAYTREE_H_ */
