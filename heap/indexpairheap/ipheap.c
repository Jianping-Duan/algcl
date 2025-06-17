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
#include "indexpairheap.h"
#include "singlelist.h"

static int cmp(const void *, const void *);
static void show_keys(const struct index_pheap *iph);

int
main(int argc, char *argv[])
{
	int el, i, j;
	int *key;
	unsigned long index;
	struct index_pheap iph;
	int sz = 0, cnt = 0, n;

	if (argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);

	if (sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Not a integer number, %s.\n", argv[1]);

	if (sz <= 0)
		errmsg_exit("Given a integer number must be greater than 0.\n");
	
	SET_RANDOM_SEED;
	
	ipheap_init(&iph, sz, sizeof(int), cmp);
	
	printf("Following output a series of Index-Key pairs and inserts "
		"then to the indexed pairing heap:\n");
	for (i = 0; i < sz; i++)
		if (!IPHEAP_ISFULL(&iph)) {
			el = rand_range_integer(1, sz < 100 ? sz * 2 : sz);
			printf("%3d-%-3d   ", i, el);
			if (++cnt % 5 == 0)
				printf("\n");
			ipheap_insert(&iph, i, &el);
		}
	if (cnt % 5 != 0)
		printf("\n");
	printf("Inserted done, total elements are %lu.\n", IPHEAP_SIZE(&iph));
	printf("\n");

	printf("Following outputs all Index-Key pairs for the indexed pairing "
		"heap:\n");
	show_keys(&iph);
	printf("\n");

	printf("Gets the minimum key and its associated index "
		"from this indeed pairing heap.\n");
	key = (int *)IPHEAP_GETKEY(&iph);
	index = IPHEAP_GETINDEX(&iph);
	printf("The Index-Key pair is %lu-%d\n", index, *key);
	printf("\n");

	n = rand_range_integer(1, sz / 2);
	printf("Deletes %d keys from this indexed pairing heap and output its "
		"index.\n", n);
	cnt = 0;
	for (i = 0; i < n; i++) {
		index = ipheap_delete(&iph);
		printf("%-3lu ", index);
		if (++cnt % 10 == 0)
			printf("\n");
	}
	if (cnt % 10 != 0)
		printf("\n");
	printf("\n");

	printf("Start randomly deleting the keys and associated with "
		"the index.\n");
	for (i = 0; i < sz / 2; i++) {
		n = rand_range_integer(0, sz);
		if (ipheap_remove(&iph, n) == 0)
			printf("The index %d and its key have been deleted.\n",
				n);
	}
	printf("Total elements are %lu\n", IPHEAP_SIZE(&iph));
	printf("\n");

	printf("Start randomly changing this indexed pairing heap of key.\n");
	for (i = 0; i < sz; i++) {
		j = rand_range_integer(0, sz - 1);
		el = rand_range_integer(0, sz * 2);
		if (ipheap_change(&iph, j, &el) == 0) {
			printf("Changed successfully, "
				"new index-key pairs: %3d-%-3d\n", j, el);
		}
	}
	show_keys(&iph);
	printf("\n");

	printf("Deletes all keys from this indexed pairing heap and returns "
		"its index.\n");
	cnt = 0;
	while (!IPHEAP_ISEMPTY(&iph)) {
		index = ipheap_delete(&iph);
		printf("%-3lu  ", index);
		if (++cnt % 10 == 0)
			printf("\n");
	}
	if (cnt % 10 != 0)
		printf("\n");

	printf("Total elements are %lu\n", IPHEAP_SIZE(&iph));
	printf("\n");

	return 0;
}

static int 
cmp(const void *k1, const void *k2)
{
	const int *x = (int *)k1, *y = (int *)k2;
	if (*x < *y)
		return 1;
	else if (*x == *y)
		return 0;
	else
		return -1;
}

static void 
show_keys(const struct index_pheap *iph)
{
	struct single_list keys;
	struct single_list indexes;
	struct slist_node *kloc, *iloc;
	int *key;
	unsigned long *ind;
	int cnt = 0;

	ipheap_traverse(iph, &keys, &indexes);
	slist_rewind(&keys, &kloc);
	slist_rewind(&indexes, &iloc);
	while (slist_has_next(kloc) && slist_has_next(iloc)) {
		key = (int *)slist_next_key(&kloc);
		ind = (unsigned long *)slist_next_key(&iloc);
		printf("%3lu-%-3d ", *ind, *key);
		if (++cnt % 5 == 0)
			printf("\n");
	}

	if (cnt % 5 != 0)
		printf("\n");
}

