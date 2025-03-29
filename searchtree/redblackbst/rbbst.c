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
#include "redblackbst.h"
#include <getopt.h>

static void usage_info(const char *);
static void check(struct redblack_bst *);

int
main(int argc, char *argv[])
{
	FILE *fp;
	struct redblack_bst bst;
	struct element item, *el;
	clock_t start_time, end_time;
	char *fname = NULL, *key = NULL, *rand_key = NULL;
	int rank, len;

	int op;
	const char *optstr = "f:k:";

	extern char *optarg;
	extern int optind;

	if(argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while((op = getopt(argc, argv, optstr)) != -1) {
		switch(op) {
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
	
	if(optind < argc)
		usage_info(argv[0]);
	
	fp = open_file(fname, "rb");
	
	SET_RANDOM_SEED;
	
	RBBST_INIT(&bst);
	
	printf("Start read data from \"%s\" file to the Red-Black BST...\n", fname);
	start_time = clock();
	rewind(fp);
	while(!feof(fp)) {
		if(fread(&item, sizeof(struct element), 1, fp) > 0)
			rbbst_put(&bst, &item);
	}
	close_file(fp);
	end_time = clock();
	printf("Read completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("The height of the Red-Black BST is: %ld\n", RBBST_HEIGHT(&bst));
	printf("The number of nodes in this Red-Black BST is: %ld\n\n",
		RBBST_SIZE(&bst));
	
	check(&bst);
	
	start_time = clock();
	printf("The red-black BST of minimum key is: %s\n",	rbbst_min(&bst));
	printf("The red-black BST of maximum key is: %s\n",	rbbst_max(&bst));
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	
	printf("Begin delete the minimum key and the "
		"maximum key from the Red-Black BST...\n");
	start_time = clock();
	rbbst_delete_min(&bst);
	rbbst_delete_max(&bst);
	end_time = clock();
	printf("Deletion completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	check(&bst);
	
	printf("Begin search key: %s\n", key);
	start_time = clock();
	if((el = rbbst_get(&bst, key)) != NULL)
		printf("It's value: %ld\n", el->value);
	else
		printf("Not found.\n");
	end_time = clock();
	printf("Search completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("The rank of key '%s' is %ld\n\n", key, rbbst_rank(&bst, key));
	
	rank = rand_range_integer(0, RBBST_SIZE(&bst));
	printf("The element of rank %d is:\n", rank);
	start_time = clock();
	el = rbbst_select(&bst, rank);
	end_time = clock();
	printf("Key: %s, value: %ld\n", el->key, el->value);
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	
	len = (int)strlen(key);
	rand_key = rand_string(len);
	printf("The largest key in this Red-Black BST less "
		"than or equal to '%s'\n", rand_key);
	start_time = clock();
	if((el = rbbst_floor(&bst, rand_key)) != NULL)
		printf("It's key %s, value is %ld\n", el->key, el->value);
	else
		printf("The given key '%s' is too small.\n", rand_key);
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	ALGFREE(rand_key);
	
	rand_key = rand_string(len);
	printf("The smallest key in this Red-Black BST "
		"greater than or equal to '%s'\n", rand_key);
	start_time = clock();
	if((el = rbbst_ceiling(&bst, rand_key)) != NULL)
		printf("It's key %s, value is %ld\n", el->key, el->value);
	else
		printf("The given key '%s' is too large.\n", rand_key);
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	ALGFREE(rand_key);
	
	printf("Begin delete key: %s\n", key);
	start_time = clock();
	rbbst_delete(&bst, key);
	end_time = clock();
	printf("Deletion completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	check(&bst);
	
	printf("The number of nodes in this Red-Black BST is: %ld\n\n",
		RBBST_SIZE(&bst));

	rbbst_clear(&bst);
	
	return 0;
}

static void
check(struct redblack_bst *bst)
{
	clock_t start_time, end_time;
	
	printf("Begin checks the integrity of Red-Black BST "
		"data structure......\n");
	start_time = clock();
	rbbst_check(bst);
	end_time = clock();
	printf("Check completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -k\n", pname);
	fprintf(stderr, "-f: The data file will be read in memory..\n");
	fprintf(stderr, "-k: The key will be searched.\n");
	exit(EXIT_FAILURE);
}
