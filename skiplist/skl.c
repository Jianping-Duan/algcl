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
#include "skiplist.h"
#include <getopt.h>

static void usage_info(const char *);
static void print(const void *);
static int less(const void *, const void *);

int 
main(int argc, char *argv[])
{
	FILE *fp;
	struct skip_list sl;
	struct element item, *el;
	int len, sz = 0;
	bool dflag = false;
	clock_t start_time, end_time;
	char *fname = NULL, *key = NULL, *rand_key = NULL;

	int op;
	const char *optstr = "f:k:n:";

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
			case 'n':
				if (sscanf(optarg, "%d", &sz) != 1)
					errmsg_exit("Illegal number. -n %s\n",	optarg);
				if (sz <= 0)
					errmsg_exit("Given a size must be greater than 0.\n");
				break;
			default:
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);
	
	skipl_init(&sl, sz, sizeof(struct element), less);
	
	printf("Start read data from \"%s\" file to the skip list...\n", fname);
	start_time = clock();
	fp = open_file(fname, "rb");
	rewind(fp);
	while (!feof(fp)) {
		if (fread(&item, sizeof(struct element), 1, fp) > 0)
			skipl_put(&sl, &item);
	}
	len = (int)strlen(item.key);
	close_file(fp);
	end_time = clock();
	printf("Read completed, estimated time(s): %.3f\n\n",
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);

	printf("show all key-value pairs for this skip list:\n");
	skipl_traverse(&sl, print);
	printf("\n");

	el = (struct element *)skipl_min(&sl);
	printf("The minimum of key in this skip list: %s\n", el->key);
	el = (struct element *)skipl_max(&sl);
	printf("The maximum of key in this skip list: %s\n", el->key);
	printf("\n");

	rand_key = rand_string(len);
	printf("The largest key in this skip list less than or equal "
		"to '%s'\n", rand_key);
	strncpy(item.key, rand_key, MAX_KEY_LEN);
	item.value = -1;
	start_time = clock();
	if ((el = (struct element *)skipl_floor(&sl, &item)) != NULL)
		printf("It's key %s, value is %ld\n", el->key, el->value);
	else
		printf("The given key '%s' is too small.\n", rand_key);
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	ALGFREE(rand_key);
	
	rand_key = rand_string(len);
	printf("The smallest key in this skip list greater than or equal "
		"to '%s'\n", rand_key);
	strncpy(item.key, rand_key, MAX_KEY_LEN);
	item.value = -1;
	start_time = clock();
	if ((el = (struct element *)skipl_ceiling(&sl, &item)) != NULL) {
		printf("It's key %s, value is %ld\n",
		el->key, el->value);
	} else
		printf("The given key '%s' is too large.\n", rand_key);
	end_time = clock();
	printf("Estimated time(s): %.3f\n\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	ALGFREE(rand_key);
	
	printf("Begin search key: %s\n", key);
	strncpy(item.key, key, MAX_KEY_LEN);
	item.value = -1;
	start_time = clock();
	if ((el = (struct element *)skipl_get(&sl, &item)) != NULL) {
		printf("key: %s, value: %ld\n", el->key, el->value);
		dflag = true;
	} else
		printf("Not found.\n");
	end_time = clock();
	printf("Search completed, estimated time(s): %.3f\n\n",
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	
	if (!dflag) {
		ALGFREE(key);
		skipl_clear(&sl);
		return 0;
	}

	printf("Begin delete key: %s\n", key);
	start_time = clock();
	skipl_delete(&sl, &item);
	end_time = clock();
	printf("Deletion completed, estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("\n");

	printf("show all key-value pairs for this skip list:\n");
	skipl_traverse(&sl, print);
	printf("The number of key-value pairs: %ld\n", SKIPL_SIZE(&sl));

	skipl_clear(&sl);

	return 0;
}

static void 
print(const void *key)
{
	struct element *item = (struct element *)key;
	printf("%-5s   %-5ld\n", item->key, item->value);
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -f -k\n", pname);
	fprintf(stderr, "-f: The data file will be read in memory..\n");
	fprintf(stderr, "-k: The key will be searched.\n");
	fprintf(stderr, "-n: The max level for this skip list.\n");
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
