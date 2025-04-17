/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, 2025 Jianping Duan <static.integer@hotmail.com>
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
 * 3. Neither the name of the University nor the names of its contributors
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
#include "sortalg.h"

static void exch(void *, void *, unsigned int);
static void valcpy(void *, const void * restrict, unsigned int);
static long partition(void *, long, long, unsigned int, algcomp_ft *);
static void merge_sort_aux(void *, void *, long, long, unsigned int,
						algcomp_ft *);

/* Is the array base[lo..hi) sorted? */
int
check_ordered_range(const void *base, long lo, long hi,
		unsigned int size, algcomp_ft *cmp)
{
	long i;

	for (i = lo + 1; i <= hi; i++)
		if (cmp(base + i * size, base + (i - 1) * size) == 1)
			return 0;
	return 1;
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order, 
 * using the insertion sort.
 */
void
insertion_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long i, j;

	for (i = lo; i <= hi; i++)
		for (j = i; j > lo && cmp(base + j * size, base + (j - 1) * size) == 1;
			j--) {
			exch(base + j * size, base + (j - 1) * size, size);
		}
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the selection sort. 
 */
void 
selection_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long i, j, min;

	for (i = lo; i <= hi; i++) {
		min = i;
		for (j = i + 1; j <= hi; j++)
			if (cmp(base + j * size, base + min * size) == 1)
				min = j;
		if (cmp(base + i * size, base + min * size) != 0)
			exch(base + i * size, base + min * size, size);
	}
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order, 
 * using the shell sort. 
 */
void
shell_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long h, i, j, n;

	/* 
	 * 3x+1 increment sequence:  
	 * 1, 4, 13, 40, 121, 364, 1093, ... 
	 */
	h = 1;
	n = hi - lo + 1;
	while (h < n/3)
		h = h * 3 + 1;

	while (h >= 1) {
		/* h-sort array */
		for (i = lo + h; i <= hi; i++)
			for (j = i; j >= lo + h && cmp(base + j * size,
				base + (j - h) * size) == 1; j -= h) {
				exch(base + j * size, base + (j - h) * size, size);
			}
		h /= 3;
	}
}

#ifndef INSERTION_SORT_CUTOFF
#define INSERTION_SORT_CUTOFF	8
#endif

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the quick sort. 
 */
void 
quick_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long j;

	if (lo >= hi)
		return;

	/* cutoff to insertion-sort for small array. */
	if (lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort_range(base, lo, hi, size, cmp);
		return;
	}
	
	j = partition(base, lo, hi, size, cmp);
	quick_sort_range(base, lo, j - 1, size, cmp);
	quick_sort_range(base, j + 1, hi, size, cmp);
}

/* 
 * quicksort the subarray base[lo .. hi] 
 * using 3-way partitioning.
 */
void
quick_3way_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long i, lt, gt;
	void *v;
	int cmprlt;

	if (lo >= hi)
		return;

	if (lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort_range(base, lo, hi, size, cmp);
		return;
	}
	
	v = algmalloc(size);
	lt = lo, gt = hi;
	i = lo + 1;
	/* value copy. It can't be "v = base + lo * size" */
	valcpy(v, base + lo * size, size); 	

	while (i <= gt) {
		cmprlt = cmp(base + i * size, v); 
		if (cmprlt == 1) {
			exch(base + i * size, base + lt * size, size);
			i++;
			lt++;
		} else if (cmprlt == -1) {
			exch(base + i * size, base + gt * size, size);
			gt--;
		} else
			i++;
	}

	ALGFREE(v);
	
	/* 
	 * base[lo..lt-1] < v = base[lt..gt] < base[gt+1..hi]. 
	 */
	quick_3way_sort_range(base, lo, lt - 1, size, cmp);
	quick_3way_sort_range(base, gt + 1, hi, size, cmp);
}

/* 
 * stably merge ordered base[lo .. mid] with 
 * base[mid+1 ..hi] using aux[lo .. hi].
 */
void 
ordered_merge(void *base, void *aux, long lo, long mid,
		long hi, unsigned int size, algcomp_ft *cmp)
{
	long i, j, k;

	/* copy to aux[] */
	for (k = lo; k <= hi; k++)
		valcpy(aux + k * size, base + k * size, size); 
	
	/* merge back to base[] */
	i = lo, j = mid + 1;
	for (k = lo; k <= hi; k++) {
		if (i > mid) {
			valcpy(base + k * size, aux + j * size, size);
			j++;
		} else if (j > hi) {
			valcpy(base + k * size, aux + i * size, size);
			i++;
		} else if (cmp(aux + j * size, aux + i * size) == 1) {
			valcpy(base + k * size, aux + j * size, size);
			j++;
		} else {
			valcpy(base + k * size, aux + i * size, size);
			i++;
		}
	}
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order, 
 * using the merge sort top-down.
 */
void
merge_sort_topdown(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	void *aux;

	aux = algcalloc(hi - lo + 1, size);
	merge_sort_aux(base, aux, lo, hi, size, cmp);
	ALGFREE(aux);
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order, 
 * using the merge sort bottom-up.
 */
void 
merge_sort_bottomup(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long len, i, j, mid, n;
	void *aux;

	n = hi - lo + 1;
	aux = algcalloc(n, size);

	for (len = 1; len < n; len *= 2)
		for (i = lo; i < hi + 1 - len; i += len + len) {
			mid = i + len - 1;
			j = MIN(i + len + len -1, hi);
			ordered_merge(base, aux, i, mid, j, size, cmp);
		}

	ALGFREE(aux);
}

/* 
 * Rearranges the subarray base[lo..hi) in ascending order, 
 * using the binary insertion sort. 
 */
void 
binary_isort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp)
{
	long i, j, llo, mid, lhi;
	void *v;
	
	for (i = lo + 1; i <= hi; i++) {
		/*  
		 * binary search to determine index j 
		 * at which to insert arr[i].
		 */
		v = algmalloc(size);
		valcpy(v, base + i * size, size);
		llo = lo, lhi = i;
		while (llo < lhi) {
			mid = llo + (lhi - llo) / 2;
			if (cmp(v, base + mid * size) == 1)
				lhi = mid;
			else
				llo = mid + 1;
		}
		
		/* 
		 * insertion sort with "half exchanges"
		 * (insert a[i] at index j and shift a[j], ..., a[i-1] to right)
		 */
		for (j = i; j > llo; j--)
			valcpy(base + j * size, base + (j - 1) * size, size);
		valcpy(base + llo * size, v, size);

		ALGFREE(v);
	}
}

/******************** static function boundary ********************/

static void
exch(void *e1, void *e2, unsigned int size)
{
	char *k1 = (char *)e1, *k2 = (char *)e2, swap;

	while (size--) {
		swap = *k1;
		*k1++ = *k2;
		*k2++ = swap;
	}
}

/* like memcpy */
static void
valcpy(void *tg, const void * restrict sr, unsigned int size)
{
	char *x = (char *)tg, *y = (char *)sr;

	while (size--)
		*x++ = *y++;
}

/* 
 * partition the subarray arr[lo..hi] so that 
 * arr[lo..j-1] <= arr[j] <= arr[j+1..hi] and 
 * return the index j. 
 */
static long 
partition(void *base, long lo, long hi, unsigned int size, algcomp_ft *cmp)
{
	long i, j;
	void *v;

	i = lo,	j = hi + 1;
	v = base + lo * size;
	
	while (1) {
		/* find item on lo to swap */
		do {
			i++;
		} while (cmp(base + i * size, v) == 1 && i != hi);

		/* find item on hi to swap */
		do {
			j--;
		} while (cmp(v, base + j * size) == 1 && j != lo);

		/* check if pointers cross */
		if (i >= j)
			break;
		
		if (cmp(base + i * size, base + j * size) != 0)
			exch(base + i * size, base + j * size, size);
	}
	
	/* put partitioning item v at base[j] */
	exch(base + j * size, base + lo * size, size);
	
	/* now, base[lo..j-1] <= base[j] <= base[j+1..hi] */
	return j;
}

/* 
 * mergesort base[lo..hi] using auxiliary array aux[lo..hi].
 */
static void
merge_sort_aux(void *base, void *aux, long lo,
		long hi, unsigned int size, algcomp_ft *cmp)
{
	long mid;

	if (lo >= hi)
		return;

	if (lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort_range(base, lo, hi, size, cmp);
		return;
	}

	mid = lo + (hi - lo) / 2;
	merge_sort_aux(base, aux, lo, mid, size, cmp);
	merge_sort_aux(base, aux, mid + 1, hi, size, cmp);
	ordered_merge(base, aux, lo, mid, hi, size, cmp);
}
