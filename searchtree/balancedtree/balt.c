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
#include "btree.h"
#include "singlelist.h"
#include <getopt.h>

static void usage_info(const char *);
static int kcmp(const void *, const void *);

int 
main(int argc, char *argv[])
{
	int *keys, *key;
	char *val;
	int i, j, w, x, k, cnt = 0;
	struct btree bt;
	struct single_list records;
	struct slist_node *nptr;
	struct btree_entry *entry;
	clock_t start_time, end_time;
	int num = 0, width = 0;

	int op;
	const char *optstr = "n:w:";

	extern char *optarg;
	extern int optind;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
			case 'n':
				if (sscanf(optarg, "%d", &num) != 1)
					errmsg_exit("Illegal number. -n %s\n", optarg);
				break;
			case 'w':
				if (sscanf(optarg, "%d", &width) != 1)
					errmsg_exit("Illegal number. -w %s\n", optarg);
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);

	SET_RANDOM_SEED;

	btree_init(&bt, sizeof(int), width + 1, kcmp);
	keys = (int *)algmalloc((num / 2) * sizeof(int));

	printf("Begin inserts into B-Tree %d key-value pairs.\n", num);
	start_time = clock();
	for (i = 0, j = 0; i < num; i++) {
		w = rand_range_integer(5, width);
		val = rand_string(w);

		w = rand_range_integer(1, num);
		btree_put(&bt, &w, val);

		x = rand_range_integer(1, num);
		if (x % 10 == 0 && j < num / 2)
			keys[j++] = w;

		ALGFREE(val);
	}
	end_time = clock();
	printf("Inserted done, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");

	printf("Begin random query %d keys and print associated value.\n", j);
	start_time = clock();
	val = (char *)algmalloc(width * sizeof(char));
	for (i = 0; i < j; i++) {
		btree_get(&bt, &keys[i], (void **)&val);
		printf("%-8d %-8s\n", keys[i], val);
	}
	ALGFREE(val);
	ALGFREE(keys);
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");

	printf("The size: %lu\n", BTREE_SIZE(&bt));
	printf("The height: %d\n", BTREE_HEIGHT(&bt));
	printf("\n");

	i = *((int *)btree_first_key(&bt));
	j = *((int *)btree_last_key(&bt));
	printf("The minimum of key in this B-Tree: %d\n", i);
	printf("The maximum of key in this B-Tree: %d\n", j);
	printf("\n");

	for (k = 0; k < num * 2; k++) {
		x = rand_range_integer(1, num);
		if (btree_delete(&bt, &x) == 0) {
			printf("The key %d was not found.\n", x);
			continue;
		}

		printf("\n");
		printf("Deletes the key: %d\n", x);
		printf("---------------------------------------\n");
		btree_range_query(&bt, &i, &j, &records);
		printf("Record set size: %lu\n", SLIST_LENGTH(&records));
		w = 0;
		SLIST_FOREACH(&records, nptr, struct btree_entry, entry) {
			key = (int *)entry->key;
			printf("%3d ", *key);
			if (++w % 10 == 0)
				printf("\n");
		}
		printf("\n");
		cnt++;
		slist_clear(&records);
		printf("---------------------------------------\n");
	}
	printf("\n");

	printf("Deleted count: %d\n", cnt);
	printf("The size: %lu\n", BTREE_SIZE(&bt));
	printf("The height: %d\n", BTREE_HEIGHT(&bt));

	btree_clear(&bt);

	return 0;
}

static int
kcmp(const void *key1, const void *key2)
{
	const int *k1, *k2;

	k1 = (int *)key1;
	k2 = (int *)key2;

	if (*k1 < *k2)
		return 1;
	else if (*k1 == *k2)
		return 0;
	else
		return -1;
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -n -w\n", pname);
	fprintf(stderr, "-n: The number of items.\n");
	fprintf(stderr, "-w: The length of key.\n");
	exit(EXIT_FAILURE);
}
