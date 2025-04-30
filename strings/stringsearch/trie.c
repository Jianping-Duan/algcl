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
#include "trie.h"
#include "linearlist.h"

#define TRIE_VALUE_NULL		(-1)

static struct trie_node * get(struct trie_node *, const char *, int);
static struct trie_node * put(struct trie *, struct trie_node *, const char *,
	long, int);
static struct trie_node * delete(struct trie *, struct trie_node *,
	const char *, int);
static void collect(const struct trie_node *, char *, struct single_list *);
static void collect_pat(const struct trie_node *, char *, const char *,
	struct single_list *);
static int longest_prefix(struct trie_node *, const char *, int, int);
static void release(struct trie_node *node);

/* Does the set contain the given key? */
bool 
trie_contains(const struct trie *t, const char *key)
{
	if (key == NULL)
		return false;

	if (get(t->root, key, 0) == NULL)
		return false;
	return true;
}

/* Returns the value associated with the given key. */
long 
trie_get(const struct trie *t, const char *key)
{
	struct trie_node *node;

	if (key == NULL)
		errmsg_exit("The argument key is null for trie_get().\n");

	if ((node = get(t->root, key, 0)) != NULL)
		return node->value;
	return TRIE_VALUE_NULL;
}

/* Adds the key to the set if it is not already present. */
void 
trie_put(struct trie *t, const char *key, long value)
{
	if (key == NULL)
		errmsg_exit("The argument key is null for trie_add().\n");

	t->root = put(t, t->root, key, value, 0);
}

/* Removes the key from the set if the key is present. */
void 
trie_delete(struct trie *t, const char *key)
{
	if (key == NULL)
		errmsg_exit("The argument key is null for trie_delete().\n");

	t->root = delete(t, t->root, key, 0);
}

/* Returns all of the keys in the set that start with prefix */
void 
trie_keys_prefix(const struct trie *t, const char *prefix, 
				struct single_list *keys)
{
	char *buf;
	int len;
	struct trie_node *node;

	len = strlen(prefix);
	buf = (char *)algcalloc(MAX_STRING_LEN + len, sizeof(char));

	slist_init(keys, MAX_STRING_LEN + len, NULL);
	strcpy(buf, prefix);

	node = get(t->root, prefix, 0);
	collect(node, buf, keys);
	
	ALGFREE(buf);
}

/* 
 * Returns all of the keys in the set that match pattern,
 * where the character '.' is interpreted as a wildcard character.
 */
void 
trie_keys_match(const struct trie *t, const char *pattern,
				struct single_list *keys)
{
	char *buf;
	int len;

	len = strlen(pattern);
	buf = (char *)algcalloc(MAX_STRING_LEN + len, sizeof(char));

	slist_init(keys, MAX_STRING_LEN + len, NULL);
	collect_pat(t->root, buf, pattern, keys);
	
	ALGFREE(buf);
}

/* 
 * Returns the string in the set that is the longest prefix of query,
 * or null, if no such string.
 */
void 
trie_longest_prefix(const struct trie *t, const char *query, char *tgstr)
{
	int len;

	if (query == NULL)
		errmsg_exit("The argument query is null for trie_longest_prefix().\n");

	if ((len = longest_prefix(t->root, query, 0, -1)) == -1)
		*tgstr = '\0';
	else
		strncpy(tgstr, query, len);
}

void 
trie_clear(struct trie *t)
{
	if (TRIE_ISEMPTY(t))
		return;
	release(t->root);
	t->size = 0;
}

/******************** static function boundary ********************/

static struct trie_node * 
get(struct trie_node *node, const char *key, int d)
{
	if (node == NULL)
		return NULL;
	
	if ((int)strlen(key) == d)
		return node;
	
	return get(node->next[string_char_at(key, d)], key, d + 1);
}

static struct trie_node * 
put(struct trie *t, struct trie_node *node, const char *key, long value, int d)
{
	int ch; 

	if (node == NULL) {
		node = (struct trie_node *)algmalloc(sizeof(struct trie_node));
		node->value = TRIE_VALUE_NULL;
		for (ch = 0; ch < STRING_RADIX; ch++)
			node->next[ch] = NULL;
	}

	if (d == (int)strlen(key)) {
		if (node->value == TRIE_VALUE_NULL)
			t->size++;
		node->value = value;
	} else {
		ch = string_char_at(key, d);
		node->next[ch] = put(t, node->next[ch], key, value, d + 1);
	}

	return node;
}

static struct trie_node * 
delete(struct trie *t, struct trie_node *node, const char *key, int d)
{
	int ch;

	if (node == NULL)
		return NULL;

	if (d == (int)strlen(key)) {
		if (node->value != TRIE_VALUE_NULL)
			t->size--;
		node->value = TRIE_VALUE_NULL;
	} else {
		ch = string_char_at(key, d);
		node->next[ch] = delete(t, node->next[ch], key, d + 1);
	}

	/* remove subtrie rooted at x if it is completely empty */
	if (node->value != TRIE_VALUE_NULL)
		return node;
	for (ch = 0; ch < STRING_RADIX; ch++)
		if (node->next[ch] != NULL)
			return node;

	ALGFREE(node); /* node->next[ch] == NULL && !node->isstring */
	return NULL;
}

static void 
collect(const struct trie_node *node, char *prefix, struct single_list *result)
{
	int c;
	char s[2];

	if (node == NULL)
		return;

	if (node->value != TRIE_VALUE_NULL)
		slist_append(result, prefix);

	for (c = 1; c < STRING_RADIX; c++) { /* exclusion c = 0 ('\0') */
		s[0] = (char)c,	s[1] = '\0';
		strcat(prefix, s);
		collect(node->next[c], prefix, result);
		delete_char_at(prefix, strlen(prefix) - 1);
	}
}

static void 
collect_pat(const struct trie_node *node, char *prefix, const char *pat, 
			struct single_list *result)
{
	int c, ch, len1, len2;
	char s[2];

	if (node == NULL)
		return;

	len1 = strlen(prefix);
	len2 = strlen(pat);
	if (len1 == len2 && node->value != TRIE_VALUE_NULL)
		slist_append(result, prefix);

	if (len1 == len2)
		return;

	c = string_char_at(pat, len1);
	if (c == (int)'.')
		for (ch = 1; ch < STRING_RADIX; ch++) {
			s[0] = (char)ch, s[1] = '\0';
			strcat(prefix, s);
			collect_pat(node->next[ch], prefix, pat, result);
			delete_char_at(prefix, strlen(prefix) - 1);
		}
	else {
		s[0] = (char)c, s[1] = '\0';
		strcat(prefix, s);
		collect_pat(node->next[c], prefix, pat, result);
		delete_char_at(prefix, strlen(prefix) - 1);
	}
}

/* 
 * Returns the length of the longest string key in the subtrie
 * rooted at node that is a prefix of the query string,
 * assuming the first d character match and we have already
 * found node prefix match of the specified length.
 */
static int 
longest_prefix(struct trie_node *node, const char *query, int d, int len)
{
	if (node == NULL)
		return len;

	if (node->value != TRIE_VALUE_NULL)
		len = d;

	if (d == (int)strlen(query))
		return len;

	return longest_prefix(node->next[string_char_at(query, d)], query,
		d + 1, len);
}

static void 
release(struct trie_node *node)
{
	int c;

	if (node != NULL) {
		for (c = 0; c < STRING_RADIX; c++)
			release(node->next[c]);
		ALGFREE(node);
	}
}
