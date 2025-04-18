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
#include "priorityqueue.h"

static void show_keys(const struct priority_queue *);
static int greater(const void *, const void *);

int
main(int argc, char *argv[])
{
	int *key, el, i;
	int sz = 0, n = 0, cnt = 0;
	clock_t start_time, end_time;
	struct priority_queue pq;

	if (argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);
	
	if (sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Not a integer number, %s.\n", argv[1]);

	if (sz <= 0)
		errmsg_exit("Given a integer number must be greater than 0.\n");
	
	SET_RANDOM_SEED;
	
	pqueue_init(&pq, sz + 1, sizeof(int), greater); 
	printf("Following output a series of numbers and inserts then to "
		"the priority queue:\n");
	start_time = clock();
	for (i = 0; i < sz; i++) {
		el = rand_range_integer(1, sz < 100 ? sz * 2 : sz);
		pqueue_insert(&pq, &el);
	}
	end_time = clock();
	printf("Inserted done, total elements are %lu.\n\n", PQUEUE_SIZE(&pq));
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	show_keys(&pq);
	printf("\n");
	
	n = rand_range_integer(1, sz);
	printf("Deletes %d keys from this priority queue and output it.\n", n);
	start_time = clock();
	for (i = 0; i < n; i++) {
		key = (int *)pqueue_delete(&pq);
		printf("%3d ", *key);
		if (++cnt % 10 == 0)
			printf("\n");
		ALGFREE(key);
	}
	end_time = clock();
	if (cnt % 10 != 0)
		printf("\n");
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	printf("Prints this priority queue.\n");
	show_keys(&pq);

	printf("Total elements are %lu.\n", PQUEUE_SIZE(&pq));
	
	pqueue_clear(&pq);
	
	return 0;
}

static void 
show_keys(const struct priority_queue *pq)
{
	int i, cnt = 0;
	int **keys;

	keys = (int **)PQUEUE_KEYS(pq);
	for (i = 0; i < (int)PQUEUE_SIZE(pq); i++) {
		printf("%3d ", *keys[i]);
		if (++cnt % 10 == 0)
			printf("\n");
	}
	if (cnt % 10 != 0)
		printf("\n");
}

static int 
greater(const void *k1, const void *k2)
{
	int *x = (int *)k1, *y = (int *)k2;
	return *x > *y ? 1 : 0;
}
