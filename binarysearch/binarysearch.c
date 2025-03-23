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
#include "binarysearch.h"
#include "queue.h"

/* 
 * Returns the number of keys in this array strictly
 * less than the specified key.
 */
unsigned long 
binsearch_rank(const struct binary_search *bs, const char *key)
{
	long long lo, mid, hi;
	int cmp;
	
	lo = 0, hi = bs->size - 1;
	
	while(lo <= hi) {
		mid = lo + (hi - lo) / 2;
		cmp = strcmp(key, bs->items[mid].key);
		if(cmp < 0)
			hi = mid - 1;
		else if(cmp > 0)
			lo = mid + 1;
		else
			return mid;
	}
	return lo;
}

/* 
 * Returns the value associated with 
 * the given key in this array.
 */
struct element * 
binsearch_get(const struct binary_search *bs, const char *key)
{
	unsigned long i;
	
	if(BINSEARCH_ISEMPTY(bs))
		return 0;
	
	i = binsearch_rank(bs, key);
	
	if(i < bs->size && strcmp(bs->items[i].key, key) == 0)
		return &(bs->items[i]);
	return NULL;
}

/* 
 * Removes the specified key and associated
 * with value from this array.
 */
void
binsearch_delete(struct binary_search *bs,	const char *key)
{
	unsigned long i, j;
	
	if(BINSEARCH_ISEMPTY(bs))
		return;
	
	/* compute rank */
	i = binsearch_rank(bs, key);
	
	/* key not in array */
	if(i == bs->size || strcmp(bs->items[i].key, key) != 0)
		return;
	
	/* move items[j + 1] to items[j] deleting*/
	for(j = i; j < bs->size - 1; j++)
		bs->items[j] = bs->items[j + 1];
	
	bs->size--;
	/* to avoid loiter */
	memset(&(bs->items[bs->size]), 0, 
		sizeof(struct element));
}

/* 
 * Inserts the specified key-value pair into the array, 
 * overwriting the old value with the new value if the 
 * array already contains the specified key.
 * Deletes the specified key and its associated with 
 * value from this array,if the specified value is null(0).
 */
void
binsearch_put(struct binary_search *bs, const struct element *item)
{
	unsigned long i, j;
	
	assert(item != NULL);
	
	if(item->value == 0) {
		binsearch_delete(bs, item->key);
		return;
	}
	
	i = binsearch_rank(bs, item->key);
	
	/* key already contains array */
	if(i < bs->size && strcmp(bs->items[i].key, item->key) == 0) {
		bs->items[i].value = item->value;
		return;
	}
	
	/* insert new key-value pair */
	for(j = bs->size; j > i; j--)
		/* empty location */
		bs->items[j] = bs->items[j - 1];
	bs->items[j] = *item;
	
	bs->size++;
}

/* Return kth item in this ordered array */
struct element * 
binsearch_select(const struct binary_search *bs, unsigned long k)
{
	if(k < bs->size)
		return &(bs->items[k]);
	return NULL;
}

/* Returns the largest key in this array less than or 
   equal to argument key */
struct element *
binsearch_floor(const struct binary_search *bs, const char *key)
{
	unsigned long i;
	
	i = binsearch_rank(bs, key);
	
	if(i < bs->size && strcmp(bs->items[i].key, key) == 0)
		return &(bs->items[i]);
	
	/* key is too small */
	if(i == 0)
		return NULL;
	return &(bs->items[i - 1]);
}

/* 
 * Returns the smallest key in this symbol table 
 * greater than or equal to argument key.
 */
struct element * 
binsearch_ceiling(const struct binary_search *bs, const char *key)
{
	unsigned long i;
	
	i = binsearch_rank(bs, key);
	
	/* key is to large */
	if(i == bs->size)
		return NULL;
	return &(bs->items[i]);
}

/* 
 * Gets all keys from the array in 
 * the given range in ascending order.
 */
void
binsearch_keys(const struct binary_search *bs, const char *lokey,
			const char *hikey, struct queue *qp)
{
	unsigned long i;
	
	if(strcmp(lokey, hikey) > 0)
		return;
	
	for(i = binsearch_rank(bs, lokey); 
		i < binsearch_rank(bs, hikey); i++)
		enqueue(qp, &(bs->items[i]));
	
	if(binsearch_get(bs, hikey) != NULL)
		enqueue(qp,
			&(bs->items[binsearch_rank(bs, hikey)]));
}
