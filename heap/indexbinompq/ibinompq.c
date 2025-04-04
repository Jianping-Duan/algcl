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
#include "indexbinompq.h"
#include "queue.h"

static int greater(const void *, const void *);
static void show_keys(const struct index_binompq *);

int 
main(int argc, char *argv[])
{
	int el, i, *key;
	unsigned long index;
	struct index_binompq pq;
	int sz = 0, n = 0;

	if(argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);

	if(sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Not a integer number, %s.\n", argv[1]);

	if(sz <= 0)
		errmsg_exit("Given a integer number must be greater than 0.\n");
	
	SET_RANDOM_SEED;
	
	ibinompq_init(&pq, sz, sizeof(int), greater);
	/* ibinompq_init(&pq, sz, 0, greater); */
	
	printf("Following output a series of Index-Key pairs "
		"and inserts then to the indexed binomial priority queue:\n");
	for(i = 0; i < sz; i++) {
		el = rand_range_integer(1, sz * 10);
		printf("%d-%d  ", i, el);
		ibinompq_insert(&pq, i, &el);
	}
	printf("Inserted done, total elements are %lu.\n", ibinompq_size(&pq));
	printf("\n");
	
	printf("Following outputs all Index-Key pairs for "
		"the indexed binomial priority queue:\n");
	show_keys(&pq);
	printf("\n");
	
	printf("Gets the minimum key from this binomial priority queue.\n");
	key = (int *)ibinompq_get_key(&pq);
	printf("The key is %d\n\n", *key);

	n = rand_range_integer(1, sz);
	printf("Deletes %d keys from this indexed binomial "
		"priority queue and output its index.\n", n);
	for(i = 0; i < n; i++) {
		index = ibinompq_delete(&pq);
		printf("The %d index: %lu.\n", i, index);
	}
	printf("\n");
	
	i = rand_range_integer(0, sz);
	printf("Start removes randomly the index is the key of %d:\n", i);
	ibinompq_remove(&pq, i);
	printf("Removed completely.\n");
	
	printf("Total elements are %lu\n", ibinompq_size(&pq));
	printf("\n");
	
	i = rand_range_integer(0, sz);
	el = rand_range_integer(sz * 20, sz * 30);
	printf("Start changes randomly the index is the key of %d, "
		"target key is %d.\n", i, el);
	ibinompq_change(&pq, i, &el);
	printf("Changed completely.\n\n");
	
	printf("Following outputs all Index-Key pairs for "
		"the indexed binomial priority queue:\n");
	show_keys(&pq);
	printf("\n");
	
	ibinompq_clear(&pq);
	
	return 0;
}

static int 
greater(const void *k1, const void *k2)
{
	const int *x = (int *)k1, *y = (int *)k2;
	return *x > *y;
}

static void 
show_keys(const struct index_binompq *pq)
{
	struct queue qkeys, qinds;
	int *key, *k;

	QUEUE_INIT(&qkeys, 0);
	QUEUE_INIT(&qinds, 0);
	ibinompq_traverse(pq, &qkeys, &qinds);
	while(!QUEUE_ISEMPTY(&qkeys) && !QUEUE_ISEMPTY(&qinds)) {
		dequeue(&qkeys, (void **)&key);
		dequeue(&qinds, (void **)&k);
		printf("%d-%d  ", *k, *key);
	}
	printf("\n");
	queue_clear(&qkeys);
	queue_clear(&qinds);
}
