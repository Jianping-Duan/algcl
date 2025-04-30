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
#include "trie.h"
#include "singlelist.h"

int
main(void)
{
	const char * const sa[8] = {"she", "sells", "sea", 
		"shells", "by", "the", "are", "shore"};

	struct trie t;
	struct single_list keys;
	struct slist_node *nptr;
	int i;
	char *key, pre[MAX_STRING_LEN];
	
	SET_RANDOM_SEED;

	TRIE_INIT(&t);
	for (i = 0; i < 8; i++)
		trie_put(&t, sa[i], i);

	if (TRIE_SIZE(&t) < 100) {
		printf("trie_keys_prefix(\"\"):\n");
		trie_keys_prefix(&t, "", &keys);
		printf("%-8s %-8s\n", "Key", "Value");
		SLIST_FOREACH(&keys, nptr, char, key) {
			printf("%-8s %-5ld\n", key, trie_get(&t, key));
		}
		printf("\n");
		slist_clear(&keys);
	}

	printf("trie_keys_prefix(\"sh\"):\n");
	trie_keys_prefix(&t, "sh", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%s\n", key);
	}
	printf("\n");
	slist_clear(&keys);

	printf("trie_keys_prefix(\"shortening\"):\n");
	trie_keys_prefix(&t, "shortening", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%s\n", key);
	}
	printf("\n");
	slist_clear(&keys);

	printf("trie_keys_match(\".he.l.\"):\n");
	trie_keys_match(&t, ".he.l.", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%s\n", key);
	}
	printf("\n");
	slist_clear(&keys);

	printf("trie_longest_prefix(\"shellsort\"):\n");
	trie_longest_prefix(&t, "shellsort", pre);
	printf("%s\n", pre);

	printf("trie_longest_prefix(\"xshellsort\"):\n");
	trie_longest_prefix(&t, "xshellsort", pre);
	printf("%s\n", pre);

	i = (int)rand_range_integer(0, 8);
	printf("delete the key: %s\n", sa[i]);
	trie_delete(&t, sa[i]);
	trie_keys_prefix(&t, "", &keys);
	SLIST_FOREACH(&keys, nptr, char, key) {
		printf("%-8s %-5ld\n", key, trie_get(&t, key));
	}
	printf("Total keys: %ld\n", TRIE_SIZE(&t));

	trie_clear(&t);

	return 0;
}
