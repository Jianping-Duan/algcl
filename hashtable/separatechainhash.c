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
#include "separatechainhash.h"
#include "seqlist.h"
#include "queue.h"

/* Buffer hash key */
static struct {
	char key[MAX_KEY_LEN + 1];
	long hash;
} hash_buffer = {"", -1};

static long hash_code(const char *, unsigned long);

/* Initializes an empty separate-chains hash table */
void
schash_init(struct schain_hash *sch, unsigned long htsize)
{
	unsigned long i;
	
	sch->pairs = 0;
	sch->size = htsize;
	sch->lists = (struct seqlist *)algmalloc(htsize * sizeof(struct seqlist));
	
	/* initializes every linked-list */
	for (i = 0; i < htsize; i++)
		SEQLIST_INIT(&(sch->lists[i]));
}

/* 
 * Returns the value associated with the specified key 
 * in the separate-chains hash table.
 */
struct element * 
schash_get(const struct schain_hash *sch, const char *key)
{
	long hash;
	
	if (key == NULL)
		return NULL;
	
	hash = hash_code(key, sch->size);
	return seqlist_get(&(sch->lists[hash]), key);
}

/* 
 * Inserts the specified key-value pair into 
 * the separate-chains hash table.
 */
void
schash_put(struct schain_hash *sch, const struct element *item)
{
	long hash;
	
	assert(item != NULL);
	
	if (schash_get(sch, item->key) == NULL)
		sch->pairs++;
	else {
		hash = hash_code(item->key, sch->size);
		seqlist_change(&(sch->lists[hash]),	item->key, item);
		return;
	}
	
	hash = hash_code(item->key, sch->size);
	seqlist_put(&(sch->lists[hash]), item);
}

/* 
 * Removes the specified key and its associated value 
 * from this separate-chains hash table.
 */
void
schash_delete(struct schain_hash *sch, const char *key)
{
	long hash;
	
	if (key == NULL)
		return;
	
	if (schash_get(sch, key) != NULL)
		sch->pairs--;
	
	hash = hash_code(key, sch->size);
	seqlist_delete(&(sch->lists[hash]), key);
}

/* Returns all keys in this separate-chains hash table. */
void 
schash_keys(const struct schain_hash *sch, struct queue *keys)
{
	unsigned long i;
	void *key;
	struct queue lq;
	
	for (i = 0; i < sch->size; i++) {
		if (SEQLIST_ISEMPTY(&(sch->lists[i])))
			continue;
		
		QUEUE_INIT(&lq, 0);
		seqlist_keys(&(sch->lists[i]), &lq);
		while (!QUEUE_ISEMPTY(&lq)) {
			dequeue(&lq, &key);
			enqueue(keys, key);
		}
		queue_clear(&lq);
	}
}

/* Clears this separate-chains hash table */
void 
schash_clear(struct schain_hash *sch)
{
	unsigned long i;
	
	for (i = 0; i < sch->size; i++) {
		if (SEQLIST_ISEMPTY(&(sch->lists[i])))
			continue;
		seqlist_clear(&(sch->lists[i]));
		sch->pairs--;
	}
	
	ALGFREE(sch->lists);
	sch->size = 0;
}

/******************** static function boundary ********************/

/* 
 * Hash function for keys returns value between 0 and size-1.
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
