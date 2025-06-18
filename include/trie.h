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
#ifndef _TRIE_H_
#define _TRIE_H_

#include "algcomm.h"

/* extended ASCII alphabet size */
#define STRING_RADIX	256

/* Max string length */
#define MAX_STRING_LEN	128

/* R-way trie node */
struct trie_node {
	struct trie_node *next[STRING_RADIX];
	long value;
};

struct trie {
	struct trie_node *root;	/* R-way trie node */
	long size;		/* number of keys in trie */
};

/* Initializes an empty set of strings. */
#define TRIE_INIT(t)	do {	\
	(t)->root = NULL;	\
	(t)->size = 0;		\
} while (0)

/* Returns the number of strings in the set. */
#define TRIE_SIZE(t)		((t)->size)

/* Is the set empty? */
#define TRIE_ISEMPTY(t)		((t)->size == 0)

struct queue;
struct single_list;

/* Does the set contain the given key? */
bool trie_contains(const struct trie *t, const char *key);

/* Returns the value associated with the given key. */
long trie_get(const struct trie *t, const char *key);

/* Adds the key to the set if it is not already present. */
void trie_put(struct trie *t, const char *key, long value);

/* Removes the key from the set if the key is present. */
void trie_delete(struct trie *t, const char *key);

/* Returns all of the keys in the set that start with prefix */
void trie_keys_prefix(const struct trie *t, const char *prefix,
		struct single_list *keys);

/* 
 * Returns all of the keys in the set that match pattern,
 * where the character '.' is interpreted as a wildcard character.
 */
void trie_keys_match(const struct trie *t, const char *pattern,
		struct single_list *keys);

/* 
 * Returns the string in the set that is the longest prefix of query,
 * or null, if no such string.
 */
void trie_longest_prefix(const struct trie *t, const char *query, char *tgstr);

/* Clears this trie. */
void trie_clear(struct trie *t);

#endif	/* _TRIE_H_ */
