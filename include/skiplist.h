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
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include "algcomm.h"

struct skipl_node {
	void *key;			/* key contained by the Node */
	struct skipl_node **forward;	/* array to hold references to different
					   levels */
};

struct skip_list {
	struct skipl_node *head;	/* head node of skip-list */
	int maxlevel;			/* maximum number of levels */
	int level;			/* current level of skip-list */
	unsigned long size;		/* number of elements */
	unsigned int keysize;		/* the bytes of the key */
	algcomp_ft *cmp;		/* comparator over the keys */
};

/* 
 * Returns the number of key-value pairs 
 * in this skip list.
 */
#define SKIPL_SIZE(sl)		((sl)->size)

/* Is this skip list empty? */
#define SKIPL_ISEMPTY(sl)	((sl)->head->forward[0] == NULL)

/* Internal macro, used for SLIST_FOREACH. */
#define _SKIPL_NODE_KEY(nptr)	\
	((nptr) != NULL ? (nptr)->key : NULL)

/* 
 * Traverses this Skip List.
 * Before use it, you must declare 'nptr' pointer.
 */
#define SKIPL_FOREACH(sl, nptr, dtyp, key)				\
	for ((nptr) = (sl)->head->forward[0],				\
		(key) = (dtyp *)_SKIPL_NODE_KEY(nptr); (nptr) != NULL;	\
		(nptr) = (nptr)->forward[0],				\
		(key) = (dtyp *)_SKIPL_NODE_KEY(nptr))

struct single_list;

/* Initializes an empty skip list */
void skipl_init(struct skip_list *sl, int maxlvl, unsigned int ksize,
		algcomp_ft *cmp);

/* Returns the value associated with the given key. */ 
void * skipl_get(const struct skip_list *sl, const void *key);

/* 
 * Inserts the specified key-value pair 
 * into the ship list.
 */
void skipl_put(struct skip_list *sl, const void *key);

/* 
 * Removes the specified key and its associated with 
 * value from this skip list. 
 */
void skipl_delete(struct skip_list *sl, const void *key);

/* Gets all keys from this skip list */
void skipl_keys(const struct skip_list *sl, struct single_list *keys);

/* Clears this skip list. */
void skipl_clear(struct skip_list *sl);

/* Returns the smallest key in the skip list. */
void * skipl_min(const struct skip_list *sl);

/* Returns ths largest key in ths skip list. */
void * skipl_max(const struct skip_list *sl);

/* 
 * Returns the largest key in the skip list less than 
 * or equal to the specified key. 
 */
void * skipl_floor(const struct skip_list *sl, const void *key);

/* 
 * Returns the smallest key in the skip list greater than 
 * or equal to the specified key. 
 */
void * skipl_ceiling(const struct skip_list *sl, const void *key);

#endif	/* _SKIPLIST_H_ */
