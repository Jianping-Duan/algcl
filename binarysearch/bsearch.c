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
#include "binarysearch.h"
#include "queue.h"
#include <getopt.h>

static void usage_info(const char *);

int
main(int argc, char *argv[])
{
	FILE *fp;
	struct binary_search bs;
	struct element item, *el;
	clock_t start_time, end_time;
	struct queue qu;
	char *fname = NULL, *key = NULL;
	int len;

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
	
	fp = open_file(fname, "rb");

#define ARRAY_SIZE	8192
	
	BINSEARCH_INIT(&bs, ARRAY_SIZE);
	
	printf("Start read data from \"%s\" file to the "
		"binary search array...\n", fname);
	start_time = clock();
	rewind(fp);
	while (!feof(fp)) {
		if (fread(&item, sizeof(struct element), 1, fp) > 0 &&
			!BINSEARCH_ISFULL(&bs)) {
			binsearch_put(&bs, &item);
		}
	}
	fclose(fp);
	end_time = clock();
	printf("Read completed, estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	
	printf("The ordered array of minimum key: %s\n", BINSEARCH_MIN(&bs));
	printf("The ordered array of maximum key: %s\n\n", BINSEARCH_MAX(&bs));
	
	len = BINSEARCH_RANGE_SIZE(&bs, BINSEARCH_MIN(&bs), BINSEARCH_MAX(&bs));
	printf("The ordered array size: %d\n\n", len);
	
	printf("The largest key in this ordered array is "
		"less than or equal to %s\n", key);
	if ((el = binsearch_floor(&bs, key)) != NULL)
		printf("key: %s, value: %ld\n\n", el->key, el->value);
	else
		printf("That largest key not existed.\n\n");
	
	printf("The smallest key in this ordered array is "
		"greater than or equal to %s\n", key);
	if ((el = binsearch_ceiling(&bs, key)) != NULL)
		printf("key: %s, value: %ld\n\n", el->key, el->value);
	else
		printf("That smallest key not existed.\n\n");
	
	printf("Begin search key: %s\n", key);
	start_time = clock();
	if ((el = binsearch_get(&bs, key)) != NULL)
		printf("It's value: %ld\n", el->value);
	else
		printf("Not found.\n");
	end_time = clock();
	printf("Search completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("Begin delete key: %s\n", key);
	start_time = clock();
	binsearch_delete(&bs, key);
	end_time = clock();
	printf("Deletion completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");
	
	printf("Following output this ordered array:\n");
	QUEUE_INIT(&qu, 0);
	binsearch_keys(&bs, BINSEARCH_MIN(&bs), BINSEARCH_MAX(&bs), &qu);
	while (!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&el);
		printf("key: %s, value: %ld\n", el->key, el->value);
	}
	queue_clear(&qu);
	
	printf("Total elements: %ld\n", BINSEARCH_SIZE(&bs));
	
	BINSEARCH_CLEAR(&bs);
	
	return 0;
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -k\n", pname);
	fprintf(stderr, "-f: The data file will be read in memory..\n");
	fprintf(stderr, "-k: The key will be searched.\n");
	exit(EXIT_FAILURE);
}
