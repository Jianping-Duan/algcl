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
#include "indexfibpq.h"
#include "queue.h"

static int greater(const void *, const void *);
static void show_keys(const struct index_fibpq *);

int 
main(int argc, char *argv[])
{
	int *key, el, i, j;
	unsigned long index;
	struct index_fibpq fpq;
	int sz = 0, n = 0, cnt = 0;

	if (argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);

	if (sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Not a integer number, %s.\n", argv[1]);

	if (sz <= 0)
		errmsg_exit("Given a integer number must be greater than 0.\n");
	
	SET_RANDOM_SEED;
	
	ifibpq_init(&fpq, sz, sizeof(int), greater);
	/* ifibpq_init(&fpq, sz, 0, greater); */
	
	printf("Following output a series of Index-Key pairs "
		"and inserts then to the indexed fibonacci priority queue:\n");
	for (i = 0; i < sz; i++)
		if (!IFIBPQ_ISFULL(&fpq)) {
			el = rand_range_integer(1, sz < 100 ? sz * 2 : sz);
			printf("%3d-%-3d  ", i, el);
			if (++cnt % 5 == 0)
				printf("\n");
			ifibpq_insert(&fpq, i, &el);
		}
	if (cnt % 5 != 0)
		printf("\n");
	printf("Inserted done, total elements are %lu.\n", IFIBPQ_SIZE(&fpq));
	printf("\n");
	
	printf("Following outputs all Index-Key pairs for "
		"the indexed fibonacci priority queue:\n");
	show_keys(&fpq);
	printf("\n");

	printf("Gets the minimum key and its associated index "
		"from this fibonacci priority queue.\n");
	key = (int *)IFIBPQ_GETKEY(&fpq);
	index = IFIBPQ_GETINDEX(&fpq);
	printf("The Index-Key pair is %lu-%d\n", index, *key);
	printf("\n");
	
	printf("Start randomly deleting the keys and associated with the index.\n");
	for (i = 0; i < sz / 2; i++) {
		n = rand_range_integer(1, sz);
		if (ifibpq_remove(&fpq, n) == 0)
			printf("The index %d and its key have been deleted.\n", n);
	}
	printf("Total elements are %lu\n", IFIBPQ_SIZE(&fpq));
	printf("\n");

	printf("Start randomly changing this indexed fibonacci priority queue "
		"key.\n");
	for (i = 0; i < n; i++) {
		j = rand_range_integer(0, sz - 1);
		el = rand_range_integer(0, sz * 2);
		if (ifibpq_change(&fpq, j, &el) == 0) {
			printf("Changed successfully, "
				"new index-key pairs: %3d-%-3d\n", j, el);
		}
	}
	
	printf("Following outputs all Index-Key pairs for "
		"the indexed fibonacci priority queue:\n");
	show_keys(&fpq);
	printf("\n");

	printf("Deletes all keys from this indexed fibonacci "
		"priority queue and returns its index.\n");
	cnt = 0;
	while (!IFIBPQ_ISEMPTY(&fpq)) {
		index = ifibpq_delete(&fpq);
		printf("%-3lu  ", index);
		if (++cnt % 10 == 0)
			printf("\n");
	}
	if (cnt % 10 != 0)
		printf("\n");

	printf("Total elements are %lu\n", IFIBPQ_SIZE(&fpq));
	printf("\n");

	ifibpq_clear(&fpq);
	
	return 0;
}

static int 
greater(const void *k1, const void *k2)
{
	const int *x = (int *)k1, *y = (int *)k2;
	return *x > *y;
}

static void 
show_keys(const struct index_fibpq *fpq)
{
	int *key;
	unsigned long *ind;
	struct queue qkeys, qinds;
	int cnt = 0;
	
	QUEUE_INIT(&qkeys, sizeof(int));
	QUEUE_INIT(&qinds, sizeof(long));
	ifibpq_traverse(fpq, &qkeys, &qinds);
	
	key = (int *)algmalloc(sizeof(int));
	ind = (unsigned long *)algmalloc(sizeof(long));
	
	while(!QUEUE_ISEMPTY(&qkeys) && 
		!QUEUE_ISEMPTY(&qinds)) {
		dequeue(&qkeys, (void **)&key);
		dequeue(&qinds, (void **)&ind);
		printf("%3lu-%-3d  ", *ind, *key);
		if (++cnt % 5 == 0)
			printf("\n");
	}
	if (cnt % 5 != 0)
		printf("\n");
	queue_clear(&qkeys);
	queue_clear(&qinds);
	
	ALGFREE(key);
	ALGFREE(ind);
}
