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
#include "searchtree.h"
#include "skiplist.h"

static int cmp(const void *, const void *);

int
main(int argc, char *argv[])
{
	int i, j, sz;
	unsigned int *dat;
	struct single_list slist;
	struct rbtree rbt;
	struct splay_tree spt;
	struct skip_list skl;
	clock_t start_time, end_time;

#define LOWER_SIZE	1000
#define QUERIES		(sz * 2)
#define START_TIME	(start_time = clock())
#define END_TIME	(end_time = clock())
#define SHOW_ESTIMATED	printf("Estimated time(s): %.3f\n",\
	(double)(end_time - start_time) / (double)CLOCKS_PER_SEC)

	if (argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);

	if (sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Illegal integer number, %s\n", argv[1]);
	if (sz < LOWER_SIZE) {
		errmsg_exit("Given a integer number must be equal or greater than %d",
			LOWER_SIZE);
	}

	printf("Start generating test data...\n");
	dat = (unsigned int *)algmalloc(sz * sizeof(int));
	START_TIME;
	for (i = 0; i < sz; i++)
		*(dat + i) = i;
	shuffle_uint_array(dat, sz);
	END_TIME;
	printf("Generated done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Inserts this test data into the Single Linked List.\n");
	slist_init(&slist, sizeof(int), cmp);
	START_TIME;
	for (i = 0; i < sz; i++)
		slist_append(&slist, &dat[i]);
	END_TIME;
	printf("Inserted done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Inserts this test data into the Skip List.\n");
	skipl_init(&skl, 16, sizeof(int), cmp);
	START_TIME;
	for (i = 0; i < sz; i++)
		skipl_put(&skl, &dat[i]);
	END_TIME;
	printf("Inserted done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Inserts this test data into the Red-Black Tree.\n");
	rbbst_init(&rbt, sizeof(int), cmp);
	START_TIME;
	for (i = 0; i < sz; i++)
		rbbst_put(&rbt, &dat[i]);
	END_TIME;
	printf("Inserted done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Inserts this test data into the Splay Tree.\n");
	splayt_init(&spt, sizeof(int), cmp);
	START_TIME;
	for (i = 0; i < sz; i++)
		splayt_put(&spt, &dat[i]);
	END_TIME;
	printf("Inserted done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Query the Red-Black Tree %d times.\n", QUERIES);
	START_TIME;
	for (i = 0; i < QUERIES; i++) {
		j = (int)rand_range_integer(0, sz);
		rbbst_get(&rbt, &dat[j]);
	}
	END_TIME;
	printf("Queried done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Query the Skip List %d times.\n", QUERIES);
	START_TIME;
	for (i = 0; i < QUERIES; i++) {
		j = (int)rand_range_integer(0, sz);
		skipl_get(&skl, &dat[j]);
	}
	END_TIME;
	printf("Queried done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Query the Splay Tree %d times.\n", QUERIES);
	START_TIME;
	for (i = 0; i < QUERIES; i++) {
		j = (int)rand_range_integer(0, sz);
		splayt_get(&spt, &dat[j]);
	}
	END_TIME;
	printf("Queried done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	printf("Query the Singly Linked List %d times.\n", QUERIES);
	START_TIME;
	for (i = 0; i < QUERIES; i++) {
		j = (int)rand_range_integer(0, sz);
		slist_contains(&slist, &dat[j]);
	}
	END_TIME;
	printf("Queried done.\n");
	SHOW_ESTIMATED;
	printf("\n");

	return 0;
}

static int
cmp(const void *key1, const void *key2)
{
	int *k1 = (int *)key1, *k2 = (int *)key2;

	if (*k1 < *k2)
		return 1;
	else if (*k1 == *k2)
		return 0;
	else
		return -1;
}
