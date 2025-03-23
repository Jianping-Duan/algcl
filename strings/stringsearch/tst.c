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
#include "tstrie.h"
#include "singlelist.h"

int 
main(void)
{
	const char * const sa[8] = {"she", "sells", "sea", 
		"shells", "by", "the", "are", "shore"};

	struct tstrie t;
	struct single_list keys;
	struct slist_node *nptr;
	int i;
	char *key, *pre;
	
	SET_RANDOM_SEED;

	TSTRIE_INIT(&t);
	for(i = 0; i < 8; i++)
		tstrie_put(&t, sa[i], i);

	if(TSTRIE_SIZE(&t) < 100) {
		printf("All keys:\n");
		tstrie_keys(&t, &keys);
		printf("%-8s %-8s\n", "Key", "Value");
		SLIST_FOREACH(&keys, nptr, char, key) {
			printf("%-8s %-5d\n", key, tstrie_get(&t, key));
		}
		slist_clear(&keys);
	}
	printf("Total keys: %ld\n", TSTRIE_SIZE(&t));
	printf("\n");

	printf("tstrie_keys_prefix(\"sh\"):\n");
	tstrie_keys_prefix(&t, "sh", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%s\n", key);
	}
	printf("\n");
	slist_clear(&keys);

	printf("tstrie_keys_match(\".he.l.\"):\n");
	tstrie_keys_match(&t, ".he.l.", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%s\n", key);
	}
	printf("\n");
	slist_clear(&keys);
	
	printf("tstrie_longest_prefix(\"shellsort\"):\n");
	pre = tstrie_longest_prefix(&t, "shellsort");
	printf("%s\n", pre);
	ALGFREE(pre);
	printf("\n");

	printf("tstrie_longest_prefix(\"shell\"):\n");
	pre = tstrie_longest_prefix(&t, "shelly");
	printf("%s\n", pre);
	ALGFREE(pre);
	printf("\n");

	i = (int)rand_range_integer(0, 8);
	printf("delete the key: %s\n", sa[i]);
	tstrie_delete(&t, sa[i]);
	tstrie_keys(&t, &keys);
	printf("%-8s %-8s\n", "Key", "Value");
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%-8s %-5d\n", key, tstrie_get(&t, key));
	}
	slist_clear(&keys);
	printf("Total keys: %ld\n", TSTRIE_SIZE(&t));
	printf("\n");

	tstrie_clear(&t);

	return 0;
}
