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
#ifndef _LINEPROBHASH_H_
#define _LINEPROBHASH_H_

#include "algcomm.h"

struct line_prob_hash {
	unsigned long pairs;	/* number of key-value pairs */
	unsigned long size;		/* hash table size */
	struct element *items;	/* key-value pair */
};

/* Returns the linear-probing hash table capacity */
#define LPHASH_SIZE(lph)	((lph)->size)

/* 
 * Returns the number of key-value pairs 
 * in this linear-probing hash table.
 */
#define LPHASH_PAIRS(lph)	((lph)->pairs)

/* 
 * Returns true if this linear-probing
 * hash table is full.
 */
#define LPHASH_ISFULL(lph)	\
	((lph)->pairs < (lph)->size ? 0 : 1)

/* Initializes an empty linear-probing hash table */
#define LPHASH_INIT(lph, htsize)	do {		\
	(lph)->pairs = 0;							\
	(lph)->size = htsize;						\
	(lph)->items = (struct element *)algmalloc(	\
			htsize * sizeof(struct element));	\
} while(0)
	
/* Clears this linear-probing hash table */
#define LPHASH_CLEAR(lph)		do {	\
	ALGFREE((lph)->items);				\
	(lph)->pairs = 0;					\
	(lph)->size = 0;					\
} while(0)

struct queue;

/* 
 * Returns the value associated with the specified key 
 * in the linear-probing hash table.
 */
struct element * lphash_get(const struct line_prob_hash *lph, const char *key);

/* 
 * Inserts the specified key-value pair into the linear-probing hash table. 
 */
void lphash_put(struct line_prob_hash *lph, const struct element *item);

/* 
 * Removes the specified key and its associated value 
 * from linear-probing hash table.
 */
void lphash_delete(struct line_prob_hash *lph, const char *key);

/* Returns all keys in this linear-probing hash table. */
void lphash_keys(const struct line_prob_hash *lph, struct queue *keys);

#endif /* _LINEPROBHASH_H_ */
