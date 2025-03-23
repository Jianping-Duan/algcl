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
#ifndef _BINARYSEARCH_H_
#define _BINARYSEARCH_H_

#include "algcomm.h"

struct binary_search {
	struct element *items;	/* key-value pairs */
	unsigned long capacity;	/* array maximum capacity */
	unsigned long size;		/* array current size */
};

/* Returns the number of key-value pairs in this array. */
#define BINSEARCH_SIZE(bs)	((bs)->size)

/* Returns true if this array is empty. */
#define BINSEARCH_ISEMPTY(bs)	((bs)->size == 0)

/* Returns true if this array is full. */
#define BINSEARCH_ISFULL(bs)	\
	((bs)->size < (bs)->capacity ? 0 : 1)

/* Returns the smallest key in this array. */
#define BINSEARCH_MIN(bs)	((bs)->items[0].key)

/* Returns the largest key in this array. */
#define BINSEARCH_MAX(bs)	\
	((bs)->items[(bs)->size - 1].key)

/* Returns all key-value pairs for this array. */
#define BINSEARCH_ITEMS(bs)		((bs)->items)

#define BINSEARCH_CLEAR(bs)		do {	\
	ALGFREE((bs)->items);				\
	(bs)->capacity = 0;					\
	(bs)->size = 0;						\
} while(0)

/* 
 * Removes the smallest key and 
 * associated value from this array. 
 */
#define BINSEARCH_DELETE_MIN(bs)	do {			\
	if(!BINSEARCH_ISEMPTY(bs))						\
		binsearch_delete(bs, binsearch_min(bs));	\
} while(0)

/* 
 * Removes the largest key and 
 * associated value from this array.
 */
#define BINSEARCH_DELETE_MAX(bs)	do {			\
	if(!BINSEARCH_ISEMPTY(bs))						\
		binsearch_delete(bs, binsearch_max(bs));	\
} while(0)

/* 
 * Returns the number of keys in 
 * this array in the specified range.
 */
#define BINSEARCH_RANGE_SIZE(bs, lo, hi)	\
	(strcmp((hi), (lo)) < 0 ? 0 :			\
	binsearch_get((bs), (hi)) != NULL ?		\
	binsearch_rank((bs), (hi)) -			\
	binsearch_rank((bs), (lo)) + 1 :		\
	binsearch_rank((bs), (hi)) - binsearch_rank((bs), (lo)))
	
/* 
 * Initializes an empty array with 
 * the specified maximum capacity.
 */
#define BINSEARCH_INIT(bs, cap)		do {	\
	(bs)->items = (struct element *)algmalloc(	\
			sizeof(struct element) * (cap));	\
	(bs)->capacity = (cap);		\
	(bs)->size = 0;				\
} while(0)

struct queue;

/* 
 * Returns the number of keys in this array strictly 
 * less than the specified key.
 */
unsigned long binsearch_rank(const struct binary_search *bs, const char *key);

/* 
 * Returns the value associated with the given key in this array.
 */
struct element * binsearch_get(const struct binary_search *bs, const char *key);

/* 
 * Removes the specified key and associated with value from this array.
 */
void binsearch_delete(struct binary_search *bs, const char *key);

/* Inserts the specified key-value pair into the array.*/
void binsearch_put(struct binary_search *bs, const struct element *item);

/* Return kth item in this ordered array */
struct element * binsearch_select(const struct binary_search *bs,
								unsigned long k);

/* 
 * Returns the largest key in this array less than or equal to argument key.
 */
struct element * binsearch_floor(const struct binary_search *bs,
								const char *key);

/* 
 * Returns the smallest key in this symbol table greater than or equal
 * to argument key.
 */
struct element * binsearch_ceiling(const struct binary_search *bs, 
								const char *key);

/* 
 * Gets all keys from the array in the given range in ascending order.
 */
void binsearch_keys(const struct binary_search *bs, const char *lokey, 
					const char *hikey, struct queue *qp);

#endif /* _BINARYSEARCH_H_ */
