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
#include "tstrie.h"
#include "singlelist.h"

#define TSTRIE_HEIGHT_NODE(node)	\
	((node) == NULL ? -1 : (node)->height)

static const struct tstrie_node * get(const struct tstrie_node *, 
	const char *, int);
static struct tstrie_node * put(struct tstrie_node *, const char *, int, int);
static void collect(const struct tstrie_node *, char *, struct single_list *);
static void collect_pat(const struct tstrie_node *, char *, int, const char *,
	struct single_list *);
static struct tstrie_node * delete_node(struct tstrie_node *, const char *,
	int);
static void release(struct tstrie_node *);

/* Returns the value associated with the given key. */
int 
tstrie_get(const struct tstrie *tst, const char *key)
{
	const struct tstrie_node *node;

	if (key == NULL)
		errmsg_exit("Calls tstrie_get() with null argument.\n");
	if (strlen(key) == 0)
		return -1;

	if ((node = get(tst->root, key, 0)) == NULL)
		return -1;
	return node->value;
}

/* Inserts the key-value pair into the TST */
void 
tstrie_put(struct tstrie *tst, const char *key, int val)
{
	if (key == NULL)
		errmsg_exit("Calls tstrie_put() with null key.\n");

	if (TSTRICE_CONTAINS(tst, key))
		return;

	tst->root = put(tst->root, key, val, 0);
	tst->size++;
}

/* Returns all keys in the TST */
void 
tstrie_keys(const struct tstrie *tst, struct single_list *keys)
{
	char *buf;

	slist_init(keys, MAX_STRING_LEN, NULL);
	buf = (char *)algmalloc(MAX_STRING_LEN * sizeof(char));
	*buf = '\0';
	collect(tst->root, buf, keys);
	ALGFREE(buf);
}

/* Returns all of the keys in the set that start with prefix. */
void 
tstrie_keys_prefix(const struct tstrie *tst, const char *prefix,
				struct single_list *keys)
{
	const struct tstrie_node *node;
	char *buf;

	slist_init(keys, MAX_STRING_LEN, NULL);
	if ((node = get(tst->root, prefix, 0)) == NULL)
		return;

	buf = (char *)algmalloc(MAX_STRING_LEN * sizeof(char));
	if (node->value != -1) {
		strcpy(buf, prefix);
		slist_append(keys, buf);
		memset(buf, 0, MAX_STRING_LEN);
	}

	strcpy(buf, prefix);
	collect(node->mid, buf, keys);
	ALGFREE(buf);
}

/* 
 * Returns all of the keys in the symbol table that match pattern,
 * where the character '.' is interpreted as a wildcard character.
 */
void 
tstrie_keys_match(const struct tstrie *tst, const char *pattern, 
				struct single_list *keys)
{
	char *buf;

	slist_init(keys, MAX_STRING_LEN, NULL);
	buf = (char *)algmalloc(MAX_STRING_LEN * sizeof(char));
	*buf = '\0';
	collect_pat(tst->root, buf, 0, pattern, keys);
	ALGFREE(buf);
}

/* 
 * Returns the string in the symbol table that is the longest prefix of query,
 * or null, if no such string. 
 */
char * 
tstrie_longest_prefix(const struct tstrie *tst, const char *query)
{
	int len, i, c;
	struct tstrie_node *node;
	char *prefix;

	if (query == NULL)
		errmsg_exit("calls tstrie_longest_prefix() argument query is null.\n");

	if (strlen(query) == 0)
		return NULL;

	node = tst->root;
	len = 0, i = 0;
	while (node != NULL && i < (int)strlen(query)) {
		c = string_char_at(query, i);
		if (c < node->ch)
			node = node->left;
		else if (c > node->ch)
			node = node->right;
		else {
			i++;
			if (node->ch != -1)
				len = i;
			node = node->mid;
		}
	}

	prefix = (char *)algcalloc(len + 1, sizeof(char));
	*prefix = '\0';
	strncpy(prefix, query, len);

	return prefix;
}

/* Removes the key from the set if the key is present. */
void
tstrie_delete(struct tstrie *tst, const char *key)
{
	char *buf;
	struct single_list keys;

	if (key == NULL)
		errmsg_exit("The argument key is null for tstrie_delete().\n");

	buf = (char *)algcalloc(strlen(key) + 1, sizeof(char));
	strcpy(buf, key);

	if (TSTRICE_CONTAINS(tst, buf)) {
		tst->root = delete_node(tst->root, buf, 0); 
		while (strlen(buf) > 1) {
			delete_char_at(buf, strlen(buf) - 1);
			tstrie_keys_prefix(tst, buf, &keys);
			if (SLIST_LENGTH(&keys) > 0)
				break;
			tst->root = delete_node(tst->root, buf, 0); 
		}
		tst->size--;
		slist_clear(&keys);
	}

	ALGFREE(buf);
}

void 
tstrie_clear(struct tstrie *tst)
{
	if (TSTRIE_SIZE(tst) > 0) {
		release(tst->root);
		tst->size = 0;
	}
}

/******************** static function boundary ********************/

/* return subtrie corresponding to given key */
static const struct tstrie_node * 
get(const struct tstrie_node *node, const char *key, int d)
{
	int c;

	if (node == NULL)
		return NULL;

	c = string_char_at(key, d);
	if (c < node->ch)
		return get(node->left, key, d);
	else if (c > node->ch)
		return get(node->right, key, d);
	else if (d < (int)strlen(key) - 1)
		return get(node->mid, key, d + 1);
	else
		return node;
}

static struct tstrie_node * 
put(struct tstrie_node *node, const char *key, int val, int d)
{
	int c = string_char_at(key, d);
	long mh = -1;

	if (node == NULL) {
		node = algmalloc(sizeof(struct tstrie_node));
		node->left = NULL, node->mid = NULL, node->right = NULL;
		node->ch = c;
		node->value = -1;
		node->height = 0;
	}

	if (c < node->ch)
		node->left = put(node->left, key, val, d);
	else if (c > node->ch)
		node->right = put(node->right, key, val, d);
	else if (d < (int)strlen(key) - 1)
		node->mid = put(node->mid, key, val, d + 1);
	else
		node->value = val;

	mh = MAX(TSTRIE_HEIGHT_NODE(node->left), TSTRIE_HEIGHT_NODE(node->right));
	node->height = 1 + MAX(mh, TSTRIE_HEIGHT_NODE(node->mid));

	return node;
}

/* all keys in subtrie rooted at node with given prefix */
static void 
collect(const struct tstrie_node *node, char *prefix,
		struct single_list *result)
{
	char s[2], *rstr;

	if (node == NULL)
		return;
	
	collect(node->left, prefix, result);
	s[0] = (char)node->ch, s[1] = '\0';
	if (node->value != -1) {
		rstr = (char *)algmalloc(MAX_STRING_LEN * sizeof(char));
		strcpy(rstr, prefix);
		strcat(rstr, s);
		slist_append(result, rstr); 
		ALGFREE(rstr);
	}
	collect(node->mid, strcat(prefix, s), result);
	delete_char_at(prefix, (int)strlen(prefix) - 1);
	collect(node->right, prefix, result);
}

static void 
collect_pat(const struct tstrie_node *node, char *prefix, int d, 
		const char *pat, struct single_list *result)
{
	int c;
	char s[2], *rstr;

	if (node == NULL)
		return;

	c = string_char_at(pat, d);

	if (c == (int)'.' || c < node->ch)
		collect_pat(node->left, prefix, d, pat, result);

	if (c == (int)'.' || c == node->ch) {
		s[0] = (char)node->ch, s[1] = '\0';
		if (d == (int)strlen(pat) - 1 && node->value != -1) {
			rstr = (char *)algmalloc(MAX_STRING_LEN * sizeof(char));
			strcpy(rstr, prefix);
			strcat(rstr, s);
			slist_append(result, rstr); 
			ALGFREE(rstr);
		}
		
		if (d < (int)strlen(pat) - 1) {
			collect_pat(node->mid, strcat(prefix, s), d + 1, pat, result);
			delete_char_at(prefix, (int)strlen(prefix) - 1);
		}
	}

	if (c == (int)'.' || c > node->ch)
		collect_pat(node->right, prefix, d, pat, result);
}

static struct tstrie_node * 
delete_node(struct tstrie_node *node, const char *key, int d)
{
	int c;
	long mh = -1;
	struct tstrie_node *current = NULL;

	if (node == NULL)
		return NULL;

	c = string_char_at(key, d);
	if (c < node->ch)
		node->left = delete_node(node->left, key, d);
	else if (c > node->ch)
		node->right = delete_node(node->right, key, d);
	else {
		if (d < (int)strlen(key) - 1)
			node->mid = delete_node(node->mid, key, d + 1);

		if (d == (int)strlen(key) - 1) {
			if (node->mid != NULL && node->value != -1) {
				node->value = -1;
				return node;
			}

			if (node->mid == NULL && node->left == NULL &&
			   node->right == NULL) {
				ALGFREE(node);
				return NULL;
			}
			
			if (node->left == NULL) {
				current = node->right;
				ALGFREE(node);
				return current;
			}

			if(node->right == NULL) {
				current = node->left;
				ALGFREE(node);
				return current;
			}
		}
	}

	mh = MAX(TSTRIE_HEIGHT_NODE(node->left), TSTRIE_HEIGHT_NODE(node->right));
	node->height = 1 + MAX(mh, TSTRIE_HEIGHT_NODE(node->mid));

	return node;
}

static void 
release(struct tstrie_node *node)
{
	if (node != NULL) {
		release(node->left);
		release(node->right);
		release(node->mid);
		ALGFREE(node);
	}
}
