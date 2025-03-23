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
#include "singlelist.h"
#include <getopt.h>

static void usage_info(const char *);
static int equal(const void *, const void *);
static void display(const struct single_list *);
static void print_key(const struct single_list *);

int
main(int argc, char *argv[])
{
	int *keys, el, i, el2, *el3, *el4;
	struct single_list slist, tlist;
	int si, ei, n;

	int op;
	const char *optstr= "s:e:n:";

	extern char *optarg;
	extern int optind;
	
	SET_RANDOM_SEED;

		if(argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while((op = getopt(argc, argv, optstr)) != -1) {
		switch(op) {
			case 's':
				if(sscanf(optarg, "%d", &si) != 1)
					errmsg_exit("Illegal number. -s %s\n", optarg);
				break;
			case 'e':
				if(sscanf(optarg, "%d", &ei) != 1)
					errmsg_exit("Illegal number. -e %s\n", optarg);
				break;
			case 'n':
				if(sscanf(optarg, "%d", &n) != 1)
					errmsg_exit("Illegal number. -n %s\n", optarg);
				break;
			default: 
				fprintf(stderr, "Parameters error.\n");
				usage_info(argv[0]);
		}
	}

	if(optind < argc)
		usage_info(argv[0]);
	
	slist_init(&slist, sizeof(int), equal);
	
	printf("Following output a series of numbers and puts "
		"then to the first of the single linked list.\n");
	keys = (int *)algcalloc(n * 2, sizeof(int));
	for(i = 0; i < n; i++) {
		el = rand_range_integer(si, ei);
		printf("%d ", el);
		slist_put(&slist, &el);
		keys[i] = el;
	}
	printf("\n");
	printf("Puted done, total elements are %lu.\n\n", SLIST_LENGTH(&slist));
	
	printf("Display all keys for the single linked list.\n");
	display(&slist);
	print_key(&slist);
	el3 = SLIST_FIRST_KEY(&slist);
	el4 = SLIST_LAST_KEY(&slist);
	printf("The first key: %d, the last key: %d\n", *el3, *el4);
	printf("\n");

	printf("Following output a series of numbers and appends then to "
		"the last of the single linked list.\n");
	for(i = n; i < n * 2; i++) {
		el = rand_range_integer(si * 2, ei * 2);
		printf("%d ", el);
		slist_append(&slist, &el);
		keys[i] = el;
	}
	printf("\n");
	printf("Appended done, total elements are %lu.\n\n", SLIST_LENGTH(&slist));

	printf("Display all keys for the single linked list.\n");
	display(&slist);
	print_key(&slist);
	el3 = SLIST_FIRST_KEY(&slist);
	el4 = SLIST_LAST_KEY(&slist);
	printf("The first key: %d, the last key: %d\n", *el3, *el4);
	printf("\n");
	
	el = rand_range_integer(1, 100);
	printf("Begin searches the element is %d on "
		"the singled linked list.\n", el);
	if((i = (int)slist_contains(&slist, &el)) < 0)
		printf("The element is %d not found.\n", el);
	else
		printf("Its location is %d\n", i);
	printf("\n");
	
	printf("Start deletes a key randomly from "
		"the single linked list.\n");
	i = rand_range_integer(0, n);
	el = keys[i];
	slist_delete(&slist, &el);
	keys[i] = -1;
	display(&slist);
	print_key(&slist);
	printf("%d has deleted.\n", el);
	el3 = SLIST_FIRST_KEY(&slist);
	el4 = SLIST_LAST_KEY(&slist);
	printf("The first key: %d, the last key: %d\n", *el3, *el4);
	printf("Total elements: %lu\n", SLIST_LENGTH(&slist));
	printf("\n");
	
	printf("Reverses the single linked list.\n");
	slist_reverse(&slist);
	display(&slist);
	print_key(&slist);
	el3 = SLIST_FIRST_KEY(&slist);
	el4 = SLIST_LAST_KEY(&slist);
	printf("The first key: %d, the last key: %d\n", *el3, *el4);
	printf("\n");
	
	printf("Clones other single linked list and changed among a key.\n");
	i = rand_range_integer(0, n * 2);
	while((el = keys[i]) == -1)
		i = rand_range_integer(0, n * 2);
	el2 = rand_range_integer(si * 3, ei * 3);
	
	slist_clone(&slist, &tlist);
	slist_change(&tlist, &el, &el2);
	display(&slist);
	print_key(&tlist);
	el3 = SLIST_FIRST_KEY(&slist);
	el4 = SLIST_LAST_KEY(&slist);
	printf("The first key: %d, the last key: %d\n", *el3, *el4);
	printf("The key is %d has became %d\n", el, el2);
	printf("\n");
	
	ALGFREE(keys);
	slist_clear(&slist);
	slist_clear(&tlist);
	
	return 0;
}

static void
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -s -e -n\n", pname);
	fprintf(stderr, "-s: The start integer number.\n");
	fprintf(stderr, "-e: The end integer number.\n");
	fprintf(stderr, "-n: The number of integers.\n");
	exit(EXIT_FAILURE);
}

static int
equal(const void *key1, const void *key2)
{
	int *x = (int *)key1, *y = (int *)key2;
	
	return *x == *y;
}

static void
display(const struct single_list *slist)
{
	int *key;
	struct slist_node *nptr;

	SLIST_FOREACH(slist, nptr, int, key) {
		printf("%d ", *key);
	}
	printf("\n");
}

static void
print_key(const struct single_list *slist)
{
	int *key;
	struct slist_node *loc;

	slist_rewind(slist, &loc);
	while(slist_has_next(loc)) {
		key = (int *)slist_next_key(&loc);
		printf("%d ", *key);
	}
	printf("\n");
}
