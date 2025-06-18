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
#include "unionfind/quickfinduf.h"
#include <limits.h>

static void validate(const struct quick_find_uf *, long);

/* 
 * Initializes an empty union-find data structure with
 * N elements 0 through N - 1.
 * Initially, each element is in its own set. 
 */
void 
qfuf_init(struct quick_find_uf *uf, unsigned long n)
{
	long i;

	assert(n < LONG_MAX);

	uf->len = n;
	uf->count = n;
	
	uf->id = (long *)algmalloc(n * sizeof(long));
	for (i = 0; i < (long)n; i++)
		uf->id[i] = i;
}

/* 
 * Returns the canonical element of 
 * the set containing element p.
 */
long 
qfuf_find(const struct quick_find_uf *uf, long p)
{
	validate(uf, p);
	return uf->id[p];
}

/* Returns true if the two elements are in the same set. */
int 
qfuf_connected(const struct quick_find_uf *uf, long p, long q)
{
	validate(uf, p);
	validate(uf, q);

	return uf->id[p] == uf->id[q];
}

/* 
 * Merges the set containing element P with 
 * the set containing element Q. 
 */
void 
qfuf_union(struct quick_find_uf *uf, long p, long q)
{
	long i, pid, qid;

	validate(uf, p);
	validate(uf, q);

	/* Needed for correctness to reduce the number of array accesses. */
	pid = uf->id[p];
	qid = uf->id[q];

	/* p and q are already in the same component */
	if (pid == qid)
		return;

	for (i = 0; i < (long)uf->len; i++)
		if (pid == uf->id[i])
			uf->id[i] = qid;
	uf->count--;
}

/******************** static function boundary ********************/

/* validate that p is a valid index */
static void 
validate(const struct quick_find_uf *uf, long p)
{
	if (p < 0 || p >= (long)uf->len)
		errmsg_exit("index %ld is not between 0 and %ld.\n", p,
			uf->len - 1);
}
