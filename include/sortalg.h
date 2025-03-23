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
#ifndef _SORTALG_H_
#define _SORTALG_H_

#include "algcomm.h"

/* Is the base ordered? */
#define CHECK_ORDERED(base, nmemb, size, cmp)	\
	check_ordered_range(base, 0, nmemb - 1, size, cmp)

/* Insertion sort */
#define INSERTION_SORT(base, nmemb, size, cmp)	\
	insertion_sort_range(base, 0, nmemb - 1, size, cmp)

/* Selection sort */
#define SELECTION_SORT(base, nmemb, size, cmp)	\
	selection_sort_range(base, 0, nmemb - 1, size, cmp)

/* Shell sort */
#define SHELL_SORT(base, nmemb, size, cmp)	\
	shell_sort_range(base, 0, nmemb - 1, size, cmp)

/* Quick sort */
#define QUICK_SORT(base, nmemb, size, cmp)	\
	quick_sort_range(base, 0, nmemb - 1, size, cmp)

/* Quick 3-way sort */
#define QUICK_3WAY_SORT(base, nmemb, size, cmp)	\
	quick_3way_sort_range(base, 0, nmemb - 1, size, cmp)

/* Top-Down merge sort */
#define merge_sort_td(base, nmemb, size, cmp)	\
	merge_sort_topdown(base, 0, nmemb - 1, size, cmp)

/* Bottom-Up Merge sort */
#define MERGE_SORT_BU(base, nmemb, size, cmp)	\
	merge_sort_bottomup(base, 0, nmemb - 1, size, cmp)

/* Binary insertion sort */
#define BINARY_INSERTION_SORT(base, nmemb, size, cmp)	\
	binary_isort_range(base, 0, nmemb - 1, size, cmp)

/* Is the array base[lo..hi) sorted? */
int check_ordered_range(const void *base, long lo, long hi,
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the insertion sort. 
 */
void insertion_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the selection sort. 
 */
void selection_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the shell sort. 
 */
void shell_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the quick sort.
 */
void quick_sort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the quick 3 way sort.
 */
void quick_3way_sort_range(void *base, long lo, long hi,
		unsigned int size, algcomp_ft *cmp);

/* 
 * stably merge ordered base[lo .. mid] with 
 * base[mid+1 ..hi] using aux[lo .. hi].
 */
void ordered_merge(void *base, void *aux, long lo, 
	long mid, long hi, unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the merge sort.
 */
void merge_sort_topdown(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the merge sort.
 */
void merge_sort_bottomup(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

/* 
 * Rearranges the subarray base[lo..hi) in ascending order,
 * using the binary insertion sort. 
 */
void binary_isort_range(void *base, long lo, long hi, 
		unsigned int size, algcomp_ft *cmp);

#endif	/* _SORTALG_H_ */
