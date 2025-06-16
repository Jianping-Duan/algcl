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
#ifndef _SINGLELIST_H_
#define _SINGLELIST_H_

#include "algcomm.h"

/* single linked-list node. */
struct slist_node {
	void *key;			/* key of the node */
	struct slist_node *next;	/* point to next node */
};

/* single linked-list. */
struct single_list {
	struct slist_node *first;	/* point to the first node. */
	struct slist_node *last;	/* point to the last node. */
	unsigned long size;		/* number of keys. */
	unsigned int keysize;		/* bytes of key. */
	algcomp_ft *equal;		/* equal compare for two keys */
};

/* 
 * Returns the number of keys in 
 * this single linked-list.
 */
#define SLIST_LENGTH(sl)	((sl)->size)

/* Is the single linked-list empty? */ 
#define SLIST_ISEMPTY(sl)	((sl)->first == NULL)

/* Returns the first key in this single linked-list. */
#define SLIST_FIRST_KEY(sl)	\
	(SLIST_LENGTH(sl) == 0 ? NULL : (sl)->first->key)

/* Returns the last key in this single linked-list. */
#define SLIST_LAST_KEY(sl)	\
	(SLIST_LENGTH(sl) == 0 ? NULL : (sl)->last->key) 

/* Internal macro, used for SLIST_FOREACH. */
#define _SLIST_NODE_KEY(nptr)	\
	((nptr) != NULL ? (nptr)->key : NULL)

/* 
 * Traverses this single linked-list. 
 * Before use it, you must declare 'nptr' pointer.
 */
#define SLIST_FOREACH(slist, nptr, dtyp, key)		\
	for ((nptr) = (slist)->first,			\
		(key) = (dtyp *)_SLIST_NODE_KEY(nptr);	\
		(nptr) != NULL;				\
		(nptr) = (nptr)->next,			\
		(key) = (dtyp *)_SLIST_NODE_KEY(nptr))

/* For traverse. rewinds to first pointer. */
static inline void 
slist_rewind(const struct single_list *slist, struct slist_node **loc)
{
	*loc = slist->first;
}

/* For traverse, whether has next node? */
static inline int 
slist_has_next(struct slist_node *loc)
{
	return loc != NULL;
}

/* For traverse, returns current location of key. */
static inline void * 
slist_next_key(struct slist_node **loc)
{
	void *key;

	if (loc == NULL || (*loc) == NULL)
		return NULL;

	key = (*loc)->key;
	*loc = (*loc)->next;

	return key;
}

/* Initializes en empty single linked-list. */
void slist_init(struct single_list *slist, unsigned int ksize,
		algcomp_ft *equal);

/* Inserts a key at the first location. */
void slist_put(struct single_list *slist, const void *key);

/* Inserts a key at the last location. */
void slist_append(struct single_list *slist, const void *key);

/* 
 * Does the single linked-list contain the given key.
 * if contained, return the index of loction, -1 is not contain. 
 */
long slist_contains(const struct single_list *slist, const void *key);

/* 
 * Removes the specified key from this single linked-list.
 */
void slist_delete(struct single_list *slist, const void *key);

/* Changes the specified key SKEY become to TKEY. */
void slist_change(struct single_list *slist, const void *skey,
		const void *tkey);

/* Reverses this single linked-list. */
void slist_reverse(struct single_list *slist);

/* Clones this single linked-list and output new. */
void slist_clone(const struct single_list *slist, struct single_list *tlist);

/* Clears this single linked list. */
void slist_clear(struct single_list *slist);

#endif /* _SINGLELIST_H_ */
