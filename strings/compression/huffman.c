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
#include "huffman.h"
#include "fibonaccipq.h"
#include "binaryin.h"
#include "binaryout.h"
#include <math.h>	/* log2(), ceil() */

/* Huffman trie node */
struct huffman_node {
	int ch;						/* need to to encoded character */
	int freq;					/* character frequency */
	unsigned int size;			/* number of nodes in subtree */
	struct huffman_node *left;	/* left subtrees*/
	struct huffman_node *right;	/* right subtrees */
};

/* extended ASCII alphabet size */
#define RADIX	256

#define HUFFMAN_CODE_SIZE(node)	\
	((node) == NULL ? 0 : (node)->size)

static inline int huffman_isleaf(const struct huffman_node *);
static int compare(const void *, const void *);
static struct huffman_node * build_trie(const int *);
static void write_trie(struct huffman_node *, struct binary_output *);
static void build_code(char **, struct huffman_node *, char *);
static struct huffman_node * read_trie(struct binary_input *);
static void huffman_clear(struct huffman_node *);

/* 
 * Reads a sequence of 8-bit bytes from file input; compresses them
 * using Huffman codes with an 8-bit alphabet; and writes the results
 * to file output.
 */
void 
huffman_compress(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	char *input, **st, *code;
	int *freq;
	long len, i;
	struct huffman_node *root;
	unsigned int codewidth, clen, j;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);
	
	/* read the file input */
	input = binput_read_string(&bi);
	len = strlen(input);

	/* tabulate frequency counts */
	freq = (int *)algmalloc(RADIX * sizeof(int));
	for (i = 0; i < len; i++)
		freq[string_char_at(input, i)]++;

	/* build Huffman trie */
	root = build_trie(freq);

	/* build code table */
	codewidth = (unsigned int)ceil(log2((double)root->size));
	st = (char **)algmalloc(RADIX * sizeof(char *));
	for (i = 0; i < RADIX; i++) {
		*(st + i) = (char *)algmalloc((codewidth + 1) * sizeof(char));
		*(*(st + i)) = '\0';
	}

	code = (char *)algmalloc((codewidth + 1) * sizeof(char));
	*code = '\0';
	build_code(st, root, code);

	/* print trie for decoder */
	write_trie(root, &bo);

	/* print number of bytes in original uncompressed message */
	boutput_write_long(&bo, len);

	/* use Huffman code to encode input */
	for (i = 0; i < len; i++) {
		strcpy(code, st[string_char_at(input, i)]);
		clen = strlen(code);
		for (j = 0; j < clen; j++)
			switch (code[j]) {
				case '0':
					boutput_write_bool(&bo, false);
					break;
				case '1':
					boutput_write_bool(&bo, true);
					break;
				default:
					errmsg_exit("Illegal state.\n");
			}
	}

	BINPUT_CLOSE(&bi);
	boutput_close(&bo);
	
	ALGFREE(input);
	huffman_clear(root);

	for (i = 0; i < RADIX; i++)
		ALGFREE(st[i]);
	ALGFREE(st);
	ALGFREE(code);

	end_time = clock();
	printf("Compression finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}

/* 
 * Reads a sequence of bits that represents a Huffman-compressed message 
 * from file input; expands them; and writes the results to file output.
 */
void 
huffman_expand(const char *infile, const char *outfile)
{
	struct binary_input bi;
	struct binary_output bo;
	struct huffman_node *root, *x;
	long len, i;
	bool bit;
	clock_t start_time, end_time;

	start_time = clock();

	binput_init(&bi, infile);
	boutput_init(&bo, outfile);

	/* read in Huffman trie from input stream */
	root = read_trie(&bi);

	/* number of bytes to write */
	len = binput_read_long(&bi);

	/* decode using the Huffman trie */
	for (i = 0; i < len; i++) {
		x = root;
		while (!huffman_isleaf(x)) {
			bit = binput_read_bool(&bi);
			if (bit)
				x = x->right;
			else
				x = x->left;
		}
		boutput_write_int_r(&bo, x->ch, 8);
	}

	boutput_write_char(&bo, '\n');	/* append enter character */
	boutput_write_char(&bo, EOF);	/* append EOF */

	BINPUT_CLOSE(&bi);
	boutput_close(&bo);

	huffman_clear(root);

	end_time = clock();
	printf("Expansion finished. elapsed time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
}

/******************** static function boundary ********************/

/* is the node a leaf node? */
static inline int 
huffman_isleaf(const struct huffman_node *node)
{
	return node->left == NULL && node->right == NULL;
}

/* compare, based on frequency (minimum root heap)*/
static int 
compare(const void *k1, const void *k2)
{
	struct huffman_node *node1, *node2;

	node1 = (struct huffman_node *)k1;
	node2 = (struct huffman_node *)k2;

	return node1->freq > node2->freq;

#if 0
	if (node1->freq < node2->freq)
		return 1;
	else if (node1->freq == node2->freq)
		return 0;
	else
		return -1;
#endif
}

/* build the Huffman trie given frequencies */
static struct huffman_node * 
build_trie(const int *freqs)
{
	struct fibonaccipq pq;
	struct huffman_node *node, *left, *right, *parent;
	int c;

    /* initialize priority queue with singleton trees */
	fibpq_init(&pq, 0, compare);
	for (c = 0; c < RADIX; c++)
		if (freqs[c] > 0) {
			node = algmalloc(sizeof(struct huffman_node));
			node->ch = c;
			node->freq = freqs[c];
			node->left = NULL;
			node->right = NULL;
			node->size = 0;

			fibpq_insert(&pq, node);
		}

	/* merge two smallest trees */
	while (FIBPQ_SIZE(&pq) > 1) {
		left = (struct huffman_node *)fibpq_delete(&pq);
		right = (struct huffman_node *)fibpq_delete(&pq);

		parent = (struct huffman_node *)algmalloc(sizeof(struct huffman_node));
		parent->ch = '\0';
		parent->freq = left->freq + right->freq;
		parent->left = left;
		parent->right = right;
		parent->size = 2 + HUFFMAN_CODE_SIZE(left) + HUFFMAN_CODE_SIZE(right);

		fibpq_insert(&pq, parent);
	}

	parent = (struct huffman_node *)fibpq_delete(&pq);
	fibpq_clear(&pq);

	return parent;
}

/* write bitstring-encoded trie to file output */
static void 
write_trie(struct huffman_node *node, struct binary_output *bo)
{
	if (huffman_isleaf(node)) {
		boutput_write_bool(bo, true);
		boutput_write_int_r(bo, node->ch, 8);
		return;
	}

	boutput_write_bool(bo, false);
	write_trie(node->left, bo);
	write_trie(node->right, bo);
}

/* make a lookup table from symbols and their encodings */
static void 
build_code(char **st, struct huffman_node *node, char *code)
{
	if (!huffman_isleaf(node)) {
		build_code(st, node->left, strcat(code, "0"));
		delete_char_at(code, strlen(code) - 1);
		
		build_code(st, node->right, strcat(code, "1"));
		delete_char_at(code, strlen(code) - 1);
	} else 
		strcpy(st[node->ch], code);
}

static struct huffman_node * 
read_trie(struct binary_input *bi)
{
	struct huffman_node *node;
	bool isleaf;

	isleaf = binput_read_bool(bi);

	node = (struct huffman_node *)algmalloc(sizeof(struct huffman_node));
	if (isleaf) {
		node->ch = binput_read_int_r(bi, 8);
		node->freq = -1;
		node->left = NULL;
		node->right = NULL;
	} else {
		node->ch = '\0';
		node->freq = -1;
		node->left = read_trie(bi);
		node->right = read_trie(bi);
	}

	return node;
}

static void 
huffman_clear(struct huffman_node *root)
{
	if (root != NULL) {
		huffman_clear(root->left);
		huffman_clear(root->right);
		ALGFREE(root);
	}
}
