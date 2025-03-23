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
#include "stringsort.h"

/* extended ASCII alphabet size */
#define STRING_RADIX	256

/* cutoff to insertion sort */
#ifdef INSERTION_SORT_CUTOFF
#undef INSERTION_SORT_CUTOFF
#endif
#define INSERTION_SORT_CUTOFF	8

static inline int less(const char *, const char *, int);
static inline void exch(char **, long, long);
static inline void insertion_sort(char **, long, long, int);
static inline void mstsort_aux(char **, long, long, int, char **);
static inline void ipmstsort(char **, long, long, int);
static inline void quick_sort(char **, long, long, int);


/* Least-significant digit sort */
void 
lsdsort(char **sa, int w, long n)
{
	int d, r, *count;
	long i;
	char **aux;

	aux = (char **)algcalloc(n, sizeof(char *));
	for(i = 0; i < n; i++)
		aux[i] = (char *)algcalloc(w + 1, sizeof(char));

	count = (int *)algcalloc(STRING_RADIX + 1, sizeof(int));

	/* sort by key-indexed counting on dth character */
	for(d = w - 1; d >= 0; d--) {

		/* initialize counter array */
		for(r = 0; r <= STRING_RADIX; r++)
			count[r] = 0;
		
		/* compute frequency counts */
		for(i = 0; i < n; i++)
			count[string_char_at(sa[i], d) + 1]++;
		
		/* compute cumulates */
		for(r = 0; r < STRING_RADIX; r++)
			count[r + 1] += count[r];

		/* move data */
		for(i = 0; i < n; i++)
			strncpy(aux[count[string_char_at(sa[i], d)]++], sa[i], w + 1);

		/* move data */
		for(i = 0; i < n; i++)
			strncpy(sa[i], aux[i], w + 1);
	}

	for(i = 0; i < n; i++)
		ALGFREE(aux[i]);
	ALGFREE(aux);
	ALGFREE(count);
}

/* Most-significant digit sort */
void 
mstsort(char **sa, int mw, long n)
{
	long i;
	char **aux;

	aux = (char **)algcalloc(n, sizeof(char *));
	for(i = 0; i < n; i++)
		aux[i] = (char *)algcalloc(mw + 1, sizeof(char));

	mstsort_aux(sa, 0, n - 1, 0, aux);

	for(i = 0; i < n; i++)
		ALGFREE(aux[i]);
	ALGFREE(aux);
}

/* 3-way string quicksort */
void 
quicksort(char **sa, long n)
{
	quick_sort(sa, 0, n - 1, 0);
}

/* In-place most-significant digit sort */
void 
inplace_mstsort(char **sa, long n)
{
	ipmstsort(sa, 0, n - 1, 0);
}

/******************** static function boundary ********************/

/* is str1 less than str2, starting at character d */
static inline int 
less(const char *str1, const char *str2, int d)
{
	long len1, len2, i;

	len1 = strlen(str1), len2 = strlen(str2);
	for(i = d; i < MAX(len1, len2); i++) {
		if(string_char_at(str1, i) < string_char_at(str2, i))
			return 1;
		if(string_char_at(str1, i) >= string_char_at(str2, i))
			return 0;
	}
	return len1 < len2;
}

/* exchange sa[i] and sa[j] */
static inline void 
exch(char **sa, long i, long j)
{
	long len1, len2;
	char *tmp;

	len1 = strlen(sa[i]), len2 = strlen(sa[j]);
	tmp = (char *)algcalloc(MAX(len1, len2) + 1, sizeof(char));

	strncpy(tmp,  sa[i], len1);
	memset(sa[i], 0, len1);
	strncpy(sa[i], sa[j], len2);
	memset(sa[j], 0, len2);
	strncpy(sa[j], tmp, len1);

	ALGFREE(tmp);
}

/* insertion sort sa[lo..hi], starting at dth character */
static inline void 
insertion_sort(char **sa, long lo, long hi, int d)
{
	long i, j;

	for(i = lo; i <= hi; i++)
		for(j = i; j > lo && less(sa[j], sa[j - 1], d); j--)
			exch(sa, j, j - 1);
}

static inline void 
mstsort_aux(char **sa, long lo, long hi, int d, char **aux)
{
	long i;
	int *count, r;

	/* cutoff to insertion sort for small subarrays */
	if(lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort(sa, lo, hi, d);
		return;
	}

	/* initialize counter array */
	count = (int *)algcalloc(STRING_RADIX + 2, sizeof(int));
	for(r = 0; r < STRING_RADIX + 2; r++)
		count[r] = 0;

	/* compute frequency counts */
	for(i = lo; i <= hi; i++)
		count[string_char_at(sa[i], d) + 2]++;

	/* transform counts to indices */
	for(r = 0; r < STRING_RADIX + 1; r++)
		count[r + 1] += count[r];

	/* distribute */
	for(i = lo; i <= hi; i++)
		strcpy(aux[count[string_char_at(sa[i], d) + 1]++], sa[i]);

	/* copy back */
	for(i = lo; i <= hi; i++)
		strcpy(sa[i], aux[i - lo]);

	/* recursively sort for each character (excludes sentinel -1) */
	for(r = 0; r < STRING_RADIX; r++)
		mstsort_aux(sa, lo + count[r], lo + count[r + 1] - 1, d + 1, aux);

	ALGFREE(count);
}

/* sort from sa[lo] to sa[hi], starting at the dth character */
static inline void 
ipmstsort(char **sa, long lo, long hi, int d)
{
	long i;
	int *heads, *tails, r, c;

	if(lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort(sa, lo, hi, d);
		return;
	}

	heads = (int *)algcalloc(STRING_RADIX + 2, sizeof(int));
	for(r = 0; r < STRING_RADIX + 2; r++)
		heads[r] = 0;

	tails = (int *)algcalloc(STRING_RADIX + 1, sizeof(int));
	for(r = 0; r < STRING_RADIX + 1; r++)
		tails[r] = 0;

	/* compute frequency counts */
	for(i = lo; i <= hi; i++)
		heads[string_char_at(sa[i], d) + 2]++;

	/* transform counts to indices */
	heads[0] = lo;
	for(r = 0; r < STRING_RADIX + 1; r++) {
		heads[r + 1] += heads[r];
		tails[r] = heads[r + 1];
	}

	/* sort by d-th character in-place */
	for(r = 0; r < STRING_RADIX + 1; r++)
		while(heads[r] < tails[r]) {
			c = string_char_at(sa[heads[r]], d);
			while(c + 1 != r) {
				exch(sa, heads[r], heads[c + 1]++);
				c = string_char_at(sa[heads[r]], d);
			}
			heads[r]++;
		}

	ALGFREE(heads);

	/* recursively sort for each character (excludes sentinel -1) */
	for(r = 0; r < STRING_RADIX; r++)
		ipmstsort(sa, tails[r], tails[r + 1] - 1, d + 1);

	ALGFREE(tails);
}

/* 3-way string quicksort a[lo..hi] starting at dth character */
static inline void 
quick_sort(char **sa, long lo, long hi, int d)
{
	long lt, gt, i;
	int v, t;

	if(lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort(sa, lo, hi, d);
		return;
	}

	lt = lo, gt = hi;
	i = lo + 1;
	v = string_char_at(sa[lo], d);

	while(i <= gt) {
		t = string_char_at(sa[i], d);
		if(t < v)
			exch(sa, lt++, i++);
		else if(t > v)
			exch(sa, i, gt--);
		else
			i++;
	}

	quick_sort(sa, lo, lt - 1, d);
	if(v >= 0)
		quick_sort(sa, lt, gt, d + 1);
	quick_sort(sa, gt + 1, hi, d);
}
