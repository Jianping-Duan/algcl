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
#include "indexpqueue.h"

static int compare(const void *, const void *);

int 
main(void)
{
	int i, j;
	long *pq;
	const char * const strs[10] = {"it", "was", "the", "best", "of", 
		  "times", "it", "was", "the", "worst" };
	char **keys;
	struct index_pqueue ipq;
	
	printf("The contents of the strings and it's indexes:\n");
	for (i = 0; i < 10; i++)
		printf("%-2d %-6s\n", i, strs[i]);
	printf("\n");
	
	printf("Begin inserts this strings to the index priority queue...\n");
	ipqueue_init(&ipq, 20, 10, compare);
	for (i = 0; i < 10; i++)
		if (!IPQUEUE_ISFULL(&ipq))
			ipqueue_insert(&ipq, i, strs[i]);
	printf("Insertion completed.\n\n");
	
	printf("Deletes all keys and returns them associated index:\n");
	while (!IPQUEUE_ISEMPTY(&ipq)) {
		i = ipqueue_delete(&ipq);
		printf("%-2d %-6s\n", i, strs[i]);
	}
	printf("\n");
	
	printf("Begin inserts this strings to the index priority queue again...\n");
	for (i = 0; i < 10; i++)
		if (!IPQUEUE_ISFULL(&ipq))
			ipqueue_insert(&ipq, i, strs[i]);
	printf("Insertion completed.\n\n");
	
	printf("Prints all indexes for this index priority queue and its "
		"associated key:\n");
	pq = IPQUEUE_INDEXES(&ipq);
	for (i = 0; i < (int)IPQUEUE_SIZE(&ipq); i++) {
		j = pq[i];
		printf("%-2d %-6s\n", j, strs[j]);
	}
	printf("\n");
	
	printf("Prints all original keys for this index priority queue:\n");
	keys = (char **)IPQUEUE_KEYS(&ipq);
	for (i = 0; i < (int)IPQUEUE_SIZE(&ipq); i++)
		printf("%-2d %-6s\n", i, keys[i]);
	
	ipqueue_clear(&ipq);
	
	return 0;
}

static int
compare(const void *el1, const void *el2)
{
	const char *strs1 = (char *)el1, *strs2 = (char *)el2;
	return strcmp(strs1, strs2) > 0;
}
