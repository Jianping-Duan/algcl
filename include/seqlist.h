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
#ifndef _SEQLIST_H_
#define _SEQLIST_H_

#include "algcomm.h"

struct seqlist_node {
	struct element item;	/* key-value pair */
	struct seqlist_node *next;
};

struct seqlist {
	struct seqlist_node *first; /* pointer of the first node. */
	unsigned long len;			/* number of key-value pairs */
};

/* Returns the number of items in this sequentail list. */
#define SEQLIST_LENGTH(sl)	((sl)->len)

/* Is the sequentail list empty? */
#define SEQLIST_ISEMPTY(sl)		((sl)->first == NULL)

/* Initializes an empty sequentail list. */	
#define SEQLIST_INIT(slist)		do {	\
	(slist)->first = NULL;				\
	(slist)->len = 0;					\
} while (0)

/* Avoid includs the "queue.h" header file. */
struct queue;

/* 
 * Inserts the specified key-value pair into the sequentail list. 
 */
void seqlist_put(struct seqlist *slist, const struct element *item);

/* 
 * Returns the value associated with the given key in 
 * this sequentail list.
 */
struct element * seqlist_get(const struct seqlist *slist, const char *key);

/* 
 * Removes the specified key and its associated value from 
 * the sequentail list.
 */
void seqlist_delete(struct seqlist *slist, const char *key);

/* 
 * Changes the specified key of value from 
 * this sequentail list.
 */
void seqlist_change(struct seqlist *slist, const char *key, 
					const struct element *item);

/* Returns all keys in the sequentail list. */
void seqlist_keys(const struct seqlist *slist, struct queue *keys);

/* Clears this sequentail list. */
void seqlist_clear(struct seqlist *slist);

#endif /* _SEQLIST_H_ */
