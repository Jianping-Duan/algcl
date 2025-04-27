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
#include "unionfind/weightedquickunionuf.h"
#include <limits.h>

static void validate(const struct wquick_union_uf *, long);

/* 
 * Initializes an empty union-find data structure with N elements 0 through
 * N - 1. Initially, each element is in its own set. 
 */
void 
wquuf_init(struct wquick_union_uf *uf, unsigned long n)
{
	long i;

	assert(n < LONG_MAX);

	uf->len = n;
	uf->count = n;
	
	uf->parent = (long *)algmalloc(n * sizeof(long));
	uf->size = (long *)algmalloc(n * sizeof(long));
	for (i = 0; i < (long)n; i++) {
		uf->parent[i] = i;
		uf->size[i] = 1;
	}
}

/* 
 * Returns the canonical element of 
 * the set containing element p.
 */
long 
wquuf_find(const struct wquick_union_uf *uf, long p)
{
	validate(uf, p);
	while (p != uf->parent[p])
		p = uf->parent[p];

	return p;
}

/* Returns true if the two elements are in the same set. */
int 
wquuf_connected(const struct wquick_union_uf *uf, long p, long q)
{
	validate(uf, p);
	validate(uf, q);

	return wquuf_find(uf, p) == wquuf_find(uf, q);
}

/* 
 * Merges the set containing element P with 
 * the set containing element Q.
 */
void 
wquuf_union(struct wquick_union_uf *uf, long p, long q)
{
	long pid, qid;

	validate(uf, p);
	validate(uf, q);

	/* Needed for correctness to reduce the number of array accesses. */
	pid = wquuf_find(uf, p);
	qid = wquuf_find(uf, q);

	/* p and q are already in the same component */
	if (pid == qid)
		return;

	/* make smaller root point to larger one */
	if (uf->size[pid] < uf->size[qid]) {
		uf->parent[pid] = qid;
		uf->size[qid] += uf->size[pid];
	} else {
		uf->parent[qid] = pid;
		uf->size[pid] += uf->size[qid];
	}
	uf->count--;
}

/******************** static function boundary ********************/

/* validate that p is a valid index */
static void 
validate(const struct wquick_union_uf *uf, long p)
{
	if (p < 0 || p >= (long)uf->len)
		errmsg_exit("index %ld is not between 0 and %ld.\n", p, uf->len - 1);
}
