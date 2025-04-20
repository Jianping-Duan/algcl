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
	void *key;					/* key contained by the Node */
	struct splayt_node *left;	/* link to left subtrees */
	struct splayt_node *right;	/* link to right subtrees */
	struct splayt_node *parent;	/* link to parent node */
};

/* structure of splay tree */
struct splay_tree {
	struct splayt_node *root;	/* root node */
	unsigned long size;			/* size of splay tree */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;			/* comparator over the keys */
};

/* Returns the number of keys in this splay tree. */
#define SPLAYT_SIZE(st)		((st)->size)

/* Is this Splay Tree empty? */
#define SPLAYT_ISEMPTY(st)	((st)->root == NULL)

struct single_list;

/*
 * Initializes an empty splay tree.
 */
void splayt_init(struct splay_tree *st, unsigned int ksize, algcomp_ft *cmp);

/* 
 * Inserts the key into this splay tree. 
 */
int splayt_put(struct splay_tree *st, const void *key);

/* 
 * Returns the key in this splay tree by the given key.
 */
void * splayt_get(struct splay_tree *st, const void *key);

/* 
 * Removes the specified key from this splay tree. 
 */
int splayt_delete(struct splay_tree *st, const void *key);

/* 
 * Returns the smallest key in the splay tree. 
 */
void * splayt_min(const struct splay_tree *st);

/* 
 * Returns the largest key in the splay tree. 
 */
void * splayt_max(const struct splay_tree *st);

/* 
 * Gets all items from this splay tree. 
 */
void splayt_preorder(const struct splay_tree *st, struct single_list *keys);

/* 
 * Clears this splay tree. 
 */
void splayt_clear(struct splay_tree *st);

#endif	/* _SPLAYTREE_H_ */
