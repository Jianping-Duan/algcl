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
#include "unionfind/unionfind.h"
#include <limits.h>

static void validate(const struct union_find *, long);

/* 
 * Initializes an empty union-find data structure with
 * N elements 0 through N - 1.
 * Initially, each element is in its own set. 
 */
void 
uf_init(struct union_find *uf, unsigned long n)
{
	long i;

	assert(n < LONG_MAX);

	uf->len = n;
	uf->count = n;
	
	uf->parent = (long *)algmalloc(n * sizeof(long));
	uf->rank = (short *)algmalloc(n * sizeof(short));
	for(i = 0; i < (long)n; i++) {
		uf->parent[i] = i;
		uf->rank[i] = 0;
	}
}

/* 
 * Returns the canonical element of 
 * the set containing element p.
 */
long 
uf_find(const struct union_find *uf, long p)
{
	validate(uf, p);
	while(p != uf->parent[p])
		/* path compression by halving */
		p = uf->parent[uf->parent[p]];

	return p;
}

/* Returns true if the two elements are in the same set. */
int 
uf_connected(const struct union_find *uf, long p, long q)
{
	validate(uf, p);
	validate(uf, q);

	return uf_find(uf, p) == uf_find(uf, q);
}

/* 
 * Merges the set containing element P with 
 * the set containing element Q. 
 */
void 
uf_union(struct union_find *uf, long p, long q)
{
	long pid, qid;

	validate(uf, p);
	validate(uf, q);

	/* 
	 * Needed for correctness to reduce 
	 * the number of array accesses.
	 */
	pid = uf_find(uf, p);
	qid = uf_find(uf, q);

	/* p and q are already in the same component */
	if(pid == qid)
		return;

	/* 
	 * Make root of smaller rank point to 
	 * root of larger rank.
	 */
	if(uf->rank[pid] < uf->rank[qid])
		uf->parent[pid] = qid;
	else if(uf->rank[pid] > uf->rank[qid])
		uf->parent[qid] = pid;
	else {
		uf->parent[qid] = pid;
		uf->rank[pid]++;
	}
	uf->count--;
}

/******************** static function boundary ********************/

/* validate that p is a valid index */
static void 
validate(const struct union_find *uf, long p)
{
	if(p < 0 || p >= (long)uf->len)
		errmsg_exit("index %ld is not between 0 and %ld.\n", p, uf->len - 1);
}
