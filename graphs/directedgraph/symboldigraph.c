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
#include "symboldigraph.h"
#include "skiplist.h"
#include "singlelist.h"

#define MAX_LINE_LENGTH	1024

static int 
sybcmp(const void *key1, const void *key2)
{
	struct element *k1, *k2;

	k1 = (struct element *)key1;
	k2 = (struct element *)key2;

	if (strcmp(k1->key, k2->key) < 0)
		return 1;
	else if (strcmp(k1->key, k2->key) == 0)
		return 0;
	else
		return -1;
}

/* 
 * Initializes a digraph from a file using 
 * the specified delimiter.
 * Each line in the file contains the name of a vertex, 
 * followed by a list of the names of the vertices adjacent
 * to that vertex, separated by the delimiter.
 */
void 
sybdigraph_init(struct symbol_digraph *sg, const char *filename,
		const char *delimiter)
{
	FILE *fin;
	char line[MAX_LINE_LENGTH], **tokens;
	unsigned int i, sz, v, w;
	struct element el, *el2, *el3;
	struct skipl_node *nptr;
	
	sz = MAX_LINE_LENGTH / 4;
	tokens = (char **)algcalloc(sz, sizeof(char *));
	for (i = 0; i < sz; i++)
		tokens[i] = (char *)algcalloc(MAX_KEY_LEN, sizeof(char));
	
	/* 
	 * First pass builds the index by reading strings to
	 * associate distinct strings with an index.
	 */
	sg->st = (struct skip_list *)algmalloc(sizeof(struct skip_list));
	skipl_init(sg->st, 16, sizeof(struct element), sybcmp);
	
	fin = open_file(filename, "r");
	while (fgets(line, MAX_LINE_LENGTH, fin) &&
		line[0] != '\n' && line[0] != '\r' && !feof(fin)) {
		string_erase_rn(line);
		string_tokens(line, delimiter, tokens, MAX_KEY_LEN, &sz);
		for (i = 0; i < sz; i++) {
			strncpy(el.key, tokens[i], MAX_KEY_LEN);
			el.value = SKIPL_SIZE(sg->st);
			if (skipl_get(sg->st, &el) == NULL)
				skipl_put(sg->st, &el);
			memset(&el, 0, sizeof(struct element));
		}
		sz = MAX_LINE_LENGTH / 4;
	}
	close_file(fin);
	
	/* inverted index to get string keys in an array */
	sg->keys = algcalloc(SKIPL_SIZE(sg->st), sizeof(char *));
	for (i = 0; i < SKIPL_SIZE(sg->st); i++)
		sg->keys[i] = (char *)algcalloc(MAX_KEY_LEN, sizeof(char));

	SKIPL_FOREACH(sg->st, nptr, struct element, el2) {
		if ((el3 = (struct element *)skipl_get(sg->st, el2)) != NULL)
			strcpy(sg->keys[el3->value], el3->key);
	}
	
	/* 
	 * Second pass builds the digraph by connecting first
	 * vertex on each line to all others.
	 */
	sg->dg = (struct digraph *)algmalloc(sizeof(struct digraph));
	digraph_init(sg->dg, SKIPL_SIZE(sg->st));
	
	sz = MAX_LINE_LENGTH / 4;
	fin = open_file(filename, "r");
	memset(line, 0, MAX_LINE_LENGTH);
	while (fgets(line, MAX_LINE_LENGTH, fin) && line[0] != '\n' &&
		line[0] != '\r' && !feof(fin)) {
		string_erase_rn(line);
		string_tokens(line, delimiter, tokens, MAX_KEY_LEN, &sz);
		strncpy(el.key, tokens[0], MAX_KEY_LEN);
		el.value = -1;
		if((el2 = (struct element *)skipl_get(sg->st, &el)) != NULL) {
			v = (unsigned int)el2->value;
			for(i = 1; i < sz; i++) {
				strncpy(el.key, tokens[i], MAX_KEY_LEN);
				el.value = -1;
				el3 = (struct element *)skipl_get(sg->st, &el);
				if(el3 != NULL) {
					w = (unsigned int)el3->value;
					digraph_add_edge(sg->dg, v, w);
				}
				memset(&el, 0, sizeof(struct element));
			}
		}
		memset(&el, 0, sizeof(struct element));
		sz = MAX_LINE_LENGTH / 4;
	}
	close_file(fin);
	
	for(i = 0; i < sz; i++)
		ALGFREE(tokens[i]);
	ALGFREE(tokens);
}

/* Does the symbol digraph contain the vertex named s */
int
sybdigraph_contains(const struct symbol_digraph *sg, const char *s)
{
	struct element el;

	strncpy(el.key, s, MAX_KEY_LEN);
	el.value = -1;
	return skipl_get(sg->st, &el) != NULL;
}

/* Returns the integer associated with the vertex named s */
long 
sybdigraph_indexof(const struct symbol_digraph *sg, const char *s)
{
	struct element el, *el2;
	
	strncpy(el.key, s, MAX_KEY_LEN);
	el.value = -1;
	el2 = (struct element *)skipl_get(sg->st, &el);
	return el2->value;
}

void 
sybdigraph_print(const struct symbol_digraph *sg)
{
	struct digraph *g;
	char **keys;
	struct single_list *adj;
	struct slist_node *nptr;
	unsigned int v, *w;
	
	g = SYBDIGRAPH_DIGRAPH(sg);
	keys = SYBDIGRAPH_KEYS(sg);
	for (v = 0; v < DIGRAPH_VERTICES(g); v++) {
		printf("%s -- %u\n", keys[v], v);
		adj = DIGRAPH_ADJLIST(g, v);
		SLIST_FOREACH(adj, nptr, unsigned int, w) {
			printf("   %s -- %u\n", SYBDIGRAPH_NAMEOF(sg, *w), *w);
		}
	}
}

void 
sybdigraph_clear(struct symbol_digraph *sg)
{
	unsigned int i;
	
	for (i = 0; i < DIGRAPH_VERTICES(sg->dg); i++)
		ALGFREE(sg->keys[i]);
	ALGFREE(sg->keys);
	
	skipl_clear(sg->st);
	digraph_clear(sg->dg);
}
