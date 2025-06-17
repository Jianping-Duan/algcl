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
#include "splaytree.h"
#include "singlelist.h"
#include <getopt.h>

static void usage_info(const char *);
static void print_st(const struct splay_tree *);
static int less(const void *, const void *);

int 
main(int argc, char *argv[])
{
	FILE *fp;
	struct splay_tree bst;
	struct element item, *el;
	clock_t start_time, end_time;
	char *fname = NULL, *key = NULL;

	int op;
	const char *optstr = "f:k:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
		case 'f':
			fname = optarg;
			break;
		case 'k':
			key = optarg;
			break;
		default:
			fprintf(stderr, "Parameters error.\n");
			usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);
	
	splayt_init(&bst, sizeof(struct element), less);
	
	printf("Starting read data from \"%s\" file to the splay tree.\n",
		fname);
	start_time = clock();
	fp = open_file(fname, "rb");
	rewind(fp);
	while (!feof(fp)) {
		if (fread(&item, sizeof(struct element), 1, fp) > 0)
			splayt_put(&bst, &item);
	}
	close_file(fp);
	end_time = clock();
	printf("Read completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("The number of nodes in this splay tree is: %lu\n",
		SPLAYT_SIZE(&bst));
	printf("\n");
	
	printf("Prints this splay tree:\n");
	print_st(&bst);
	printf("\n");

	start_time = clock();
	el = (struct element *)splayt_min(&bst);
	printf("The minimum key of the splay tree: %s\n", el->key);
	el = (struct element *)splayt_max(&bst);
	printf("The maximum key of the splay tree: %s\n", el->key);
	end_time = clock();
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("Begin search key: %s\n", key);
	strncpy(item.key, key, MAX_KEY_LEN);
	item.value = -1;
	start_time = clock();
	if ((el = (struct element *)splayt_get(&bst, &item)) != NULL)
		printf("It's value: %ld\n", el->value);
	else
		printf("Not found.\n");
	end_time = clock();
	printf("Search completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("Begin delete key: %s\n", key);
	strncpy(item.key, key, MAX_KEY_LEN);
	item.value = -1;
	start_time = clock();
	if (splayt_delete(&bst, &item) == 0)
		printf("Delete completed.\n");
	else
		printf("Not found.\n");
	end_time = clock();
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("prints this splay tree:\n");
	print_st(&bst);
	printf("The number of keys in this splay tree is: %ld\n",
		SPLAYT_SIZE(&bst));
	
	splayt_clear(&bst);
	
	return 0;
}

static void
print_st(const struct splay_tree *st)
{
	struct single_list elset;
	struct slist_node *nptr;
	struct element *el;

	splayt_preorder(st, &elset);
	SLIST_FOREACH(&elset, nptr, struct element, el) {
		printf("%10s %10ld\n", el->key, el->value);
	}
	slist_clear(&elset);
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -k\n", pname);
	fprintf(stderr, "-f: The data file will be read in memory.\n");
	fprintf(stderr, "-k: The key will be searched.\n");
	exit(EXIT_FAILURE);
}

static int 
less(const void *key1, const void *key2)
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
