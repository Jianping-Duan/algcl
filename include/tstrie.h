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
#ifndef _TSTRIE_H_
#define _TSTRIE_H_

#include "algcomm.h"

/* extended ASCII alphabet size */
#define STRING_RADIX	256

/* Max string length */
#define MAX_STRING_LEN	128

/* ternary search trie node */
struct tstrie_node {
	int ch;						/* character */
	struct tstrie_node *left;	/* left subtries */
	struct tstrie_node *mid;	/* middle subtries */
	struct tstrie_node *right;	/* right substries */
	int value;					/* value associated with string */
	long height;				/* height of subtire */
};

/* ternary search trie */
struct tstrie {
	struct tstrie_node *root;	/* root of trie */
	long size;	/* size of trie */
};

/* Initialize an empty TST */
#define	TSTRIE_INIT(tst)	do {	\
	(tst)->root = NULL;				\
	(tst)->size = 0;				\
} while(0)

/* Returns the number of keys in this TST */
#define TSTRIE_SIZE(tst)	((tst)->size)

/* Does this TST contain the given key */
#define TSTRICE_CONTAINS(tst, key)	\
	(key != NULL ? tstrie_get(tst, key) != -1 : 0)

struct single_list;

/* Returns the value associated with the given key. */
int tstrie_get(const struct tstrie *tst, const char *key);

/* Inserts the key-value pair into the TST */
void tstrie_put(struct tstrie *tst, const char *key, int val);

/* Gets all keys in the TST */
void tstrie_keys(const struct tstrie *tst, struct single_list *keys);

/* Returns all of the keys in the set that start with prefix. */
void tstrie_keys_prefix(const struct tstrie *tst, const char *prefix, 
						struct single_list *keys);

/* 
 * Returns all of the keys in the symbol table that match pattern,
 * where the character '.' is interpreted as a wildcard character.
 */
void tstrie_keys_match(const struct tstrie *tst, const char *pattern, 
					struct single_list *keys);

/* 
 * Returns the string in the symbol table that is the 
 * longest prefix of query, or null, if no such string. 
 */
char * tstrie_longest_prefix(const struct tstrie *tst, const char *query);

/* Removes the key from the set if the key is present. */
void tstrie_delete(struct tstrie *tst, const char *key);

/* Clears this trie. */
void tstrie_clear(struct tstrie *tst);

#endif	/* _TSTRIE_H_ */
