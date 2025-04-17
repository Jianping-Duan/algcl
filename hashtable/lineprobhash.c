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
#include "lineprobhash.h"
#include "queue.h"

static struct {
	char key[MAX_KEY_LEN + 1];
	long hash;
} hash_buffer = {"", -1};

static long hash_code(const char *, unsigned long);
static int isnull(struct element *);

/* 
 * Returns the value associated with the specified key
 * in the linear-probing hash table.
 */
struct element * 
lphash_get(const struct line_prob_hash *lph, const char *key)
{
	unsigned long i;
	
	for (i = hash_code(key, lph->size); !isnull(&(lph->items[i]));
		i = (i + 1) % lph->size) {
		if (strcmp(lph->items[i].key, key) == 0)
			return &(lph->items[i]);
	}

	return NULL;
}

/* 
 * Inserts the specified key-value pair into 
 * the linear-probing hash table. 
 * overwriting the old value with the new value 
 * if the linear-probing hash table 
 * already contains the specified key.
 */
void
lphash_put(struct line_prob_hash *lph, const struct element *item)
{
	unsigned long i;
	
	assert(item != NULL);
	
	/* find empty location*/
	for (i = hash_code(item->key, lph->size); !isnull(&(lph->items[i]));
	    i = (i + 1) % lph->size) {
		if (strcmp(lph->items[i].key, item->key) == 0) {
			/* 
			 * Overwriting the old value with 
			 * the new value if already contains.
			 */
			lph->items[i].value = item->value;
			return;
		}
	}
	
	/* insert item to the empty location of Items.*/
	lph->items[i] = *item;
	lph->pairs++;
}

/* 
 * Removes the specified key and its associated value 
 * from linear-probing hash table.
 */
void 
lphash_delete(struct line_prob_hash *lph, const char *key)
{
	unsigned long i;
	struct element item;
	
	if (lphash_get(lph, key) == NULL)
		return;
	
	/* find position i of key */
	i = hash_code(key, lph->size);
	while (strcmp(key, lph->items[i].key) != 0)
		i = (i + 1) % lph->size;
	
	/* clear items[i] */
	memset(&(lph->items[i]), 0, sizeof(struct element));
	
	/* 
	 * rehash all keys in same cluster 
	 * avoid empty location.
	 */
	i = (i + 1) % lph->size;
	while (!isnull(&(lph->items[i]))) {
		item = lph->items[i];
		memset(&(lph->items[i]), 0, sizeof(struct element));
		lph->pairs--;
		lphash_put(lph, &item);
		i = (i + 1) % lph->size;
	}
	
	lph->pairs--;
}

/* Returns all keys in this linear-probing hash table */
void
lphash_keys(const struct line_prob_hash *lph, struct queue *keys)
{
	unsigned long i;
	
	for (i = 0; i < lph->size; i++)
		if (!isnull(&(lph->items[i])))
			enqueue(keys, lph->items[i].key);
}

/******************** static function boundary ********************/

/* 
 * Hash function for keys returns value 
 * between 0 and size-1.
 */
static long 
hash_code(const char *key, unsigned long htsize)
{
	long hash, len, i;
	const int R = 256;	/* Radix for character */
	
	assert(key != NULL && (len = strlen(key)) > 0);
	
	if (strcmp(hash_buffer.key, key) == 0)
		return hash_buffer.hash;
	else {
		hash = 0;
		for (i = 0; i < len; i++)
			hash = (R * hash + string_char_at(key, i)) % htsize;
		
		strncpy(hash_buffer.key, key, MAX_KEY_LEN);
		hash_buffer.hash = hash;
		
		return hash;
	}
}

static int
isnull(struct element *el)
{
	if (el == NULL)
		return 1;
	if (strlen(el->key) == 0 && el->value == 0)
		return 1;
	else
		return 0;
}
