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
#include "searchtree.h"
#include "linearlist.h"

#define MAX_LINE_LENGTH	1024

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
	char line[MAX_LINE_LENGTH], **tokens, *key;
	unsigned int i, sz, v, w;
	struct element el, *el2;
	struct queue qu;
	
	sz = MAX_LINE_LENGTH / 4;
	tokens = (char **)algcalloc(sz, sizeof(char *));
	for(i = 0; i < sz; i++)
		tokens[i] = (char *)algcalloc(MAX_KEY_LEN, sizeof(char));
	
	/* 
	 * First pass builds the index by reading strings to
	 * associate distinct strings with an index.
	 */
	sg->st = (struct redblack_bst *)algmalloc(sizeof(struct redblack_bst));
	RBBST_INIT(sg->st);
	
	fin = open_file(filename, "r");
	while(fgets(line, MAX_LINE_LENGTH, fin) &&
		line[0] != '\n' && line[0] != '\r' && !feof(fin)) {
		string_erase_rn(line);
		string_tokens(line, delimiter, tokens, MAX_KEY_LEN, &sz);
		for(i = 0; i < sz; i++) {
			if(rbbst_get(sg->st, tokens[i]) == NULL) {
				strcpy(el.key, tokens[i]);
				el.value = RBBST_SIZE(sg->st);
				rbbst_put(sg->st, &el);
			}
			memset(tokens[i], 0, MAX_KEY_LEN);
		}
		sz = MAX_LINE_LENGTH / 4;
	}
	close_file(fin);
	
	/* inverted index to get string keys in an array */
	sg->keys = algcalloc(RBBST_SIZE(sg->st), sizeof(char *));
	for(i = 0; i < RBBST_SIZE(sg->st); i++)
		sg->keys[i] = (char *)algcalloc(MAX_KEY_LEN, sizeof(char));
	
	QUEUE_INIT(&qu, 0);
	rbbst_keys(sg->st, rbbst_min(sg->st), rbbst_max(sg->st), &qu);
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&key);
		if((el2 = rbbst_get(sg->st, key)) != NULL)
			strcpy(sg->keys[el2->value], key);
	}
	queue_clear(&qu);
	
	/* 
	 * Second pass builds the digraph by connecting first
	 * vertex on each line to all others.
	 */
	sg->g = (struct digraph *)algmalloc(sizeof(struct digraph));
	digraph_init(sg->g, RBBST_SIZE(sg->st));
	
	sz = MAX_LINE_LENGTH / 4;
	fin = open_file(filename, "r");
	memset(line, 0, MAX_LINE_LENGTH);
	while(fgets(line, MAX_LINE_LENGTH, fin) && line[0] != '\n' && 
		line[0] != '\r' && !feof(fin)) {
		string_erase_rn(line);
		string_tokens(line, delimiter, tokens, MAX_KEY_LEN, &sz);
		if((el2 = rbbst_get(sg->st, tokens[0])) != NULL) {
			v = (unsigned int)el2->value;
			for(i = 1; i < sz; i++) {
				if((el2 = rbbst_get(sg->st, tokens[i])) != NULL) {
					w = (unsigned int)el2->value;
					digraph_add_edge(sg->g, v, w);
				}
				memset(tokens[i], 0, MAX_KEY_LEN);
			}
		}
		memset(tokens[0], 0, MAX_KEY_LEN);
		sz = MAX_LINE_LENGTH / 4;
	}
	close_file(fin);
	
	for(i = 0; i < sz; i++)
		ALGFREE(tokens[i]);
	ALGFREE(tokens);
}

/* Returns the integer associated with the vertex named s */
long 
sybdigraph_indexof(const struct symbol_digraph *sg, const char *s)
{
	struct element *el;
	
	el = rbbst_get(sg->st, s);
	return el->value;
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
	for(v = 0; v < DIGRAPH_VERTICES(g); v++) {
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
	
	for(i = 0; i < DIGRAPH_VERTICES(sg->g); i++)
		ALGFREE(sg->keys[i]);
	ALGFREE(sg->keys);
	
	rbbst_clear(sg->st);
	digraph_clear(sg->g);
}
