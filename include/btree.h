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
#ifndef _BTREE_H_
#define _BTREE_H_

#include "algcomm.h"

/* 
 * Max children per B-tree node = MAX_CHILDREN-1
 * (must be even and greater than 2).
 */
#define MAX_CHILDREN	8

/* declare internal/external node */
struct btree_entry;

/* B-tree node data type */
struct btree_node {
	unsigned int sz;	/* number of children */
	struct btree_entry *children[MAX_CHILDREN];	/* the ordered of children */
	struct btree_node *prev;	/* points to previous node */
	struct btree_node *sibling;	/* points to next node */
	struct btree_node *parent;	/* points to parent node */
};

/* 
 * internal nodes: only use key and next, 
 * external nodes: only use key and value.
 */
struct btree_entry {
	void *key;
	void *value;
	struct btree_node *next;	/* points to lower level nodes */
};

/* B-Tree */
struct btree {
	struct btree_node *root;	/* root of the B-tree */
	int height;					/* height of the B-tree */
	unsigned long size;			/* number of key-value pairs in the B-tree */
	unsigned int keysize;		/* max size of the key */
	unsigned int valsize;		/* max size of the value */
	algcomp_ft *kcmp;			/* compare function for keys */
};

/* Returns true if this symbol table is empty. */
#define BTREE_ISEMPTY(bt)	((bt)->size == 0)

/* 
 * Returns the number of key-value pairs in this B-Tree. 
 */
#define BTREE_SIZE(bt)	((bt)->size)

/* Returns the height of this B-tree */
#define BTREE_HEIGHT(bt)	((bt)->height)

struct single_list;

/* Initializes an empty B-tree. */
void btree_init(struct btree *bt, unsigned int ksz, unsigned int vsz, 
				algcomp_ft *cmp);

/* Inserts the key-value pair into the B-Tree. */
void btree_put(struct btree *bt, const void *key, const void *val);

/* Returns the value associated with the given key. */
void btree_get(const struct btree *bt, const void *key, void **val);

/* Clears this B-Tree */
void btree_clear(struct btree *bt);

/* Output query result that set between lokey and hikey. */
void btree_range_query(const struct btree *bt, const void *lokey,
					const void *hikey, struct single_list *keys);

/* Deletes a key-value pair from this B-Tree */
int btree_delete(struct btree *bt, const void *key);

/* Returns the first key in this B-Tree. */
void * btree_first_key(const struct btree *bt);

/* Returns the last key in this B-Tree. */
void * btree_last_key(const struct btree *bt);

#endif	/* _BTREE_H_ */
