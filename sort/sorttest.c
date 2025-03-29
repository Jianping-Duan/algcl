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

#define MAX_SORTS	10
#define MAX_ITEMS	100000

static int less(const void *, const void *);
static void (*sort_fptr)(void *, long, long, unsigned int, algcomp_ft *);
static void sort(int *, int, int);

int
main(void)
{
	int array[MAX_SORTS][MAX_ITEMS], i;

	SET_RANDOM_SEED;
	
	for(i = 0; i < MAX_ITEMS; i++) {
		array[0][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[1][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[2][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[3][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[4][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[5][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[6][i] = rand_range_integer(0, MAX_ITEMS * 2);
		array[7][i] = rand_range_integer(0, MAX_ITEMS * 2);
	}

	printf("Begin tests Insertion-Sort.\n");
	sort(array[0], MAX_ITEMS, 0);
	printf("\n");

	printf("Begin tests Selection-Sort.\n");
	sort(array[1], MAX_ITEMS, 1);
	printf("\n");
	
	printf("Begin tests Shell-Sort.\n");
	sort(array[2], MAX_ITEMS, 2);
	printf("\n");
	
	printf("Begin tests Quick-Sort.\n");
	sort(array[3], MAX_ITEMS, 3);
	printf("\n");

	printf("Begin tests Quick-3way-Sort.\n");
	sort(array[4], MAX_ITEMS, 4);
	printf("\n");

	printf("Begin tests Merge-Sort for Top-Down.\n");
	sort(array[5], MAX_ITEMS, 5);
	printf("\n");

	printf("Begin tests Merge-Sort for Bottom-Up.\n");
	sort(array[6], MAX_ITEMS, 6);
	printf("\n");

	printf("Begin tests binary insertion sort.\n");
	sort(array[7], MAX_ITEMS, 7);
	printf("\n");

	return 0;
}

static int
less(const void *k1, const void *k2)
{
	int *x = (int *)k1, *y = (int *)k2;

	if(*x < *y)
		return 1;
	else if(*x > *y)
		return -1;
	else
		return 0;
}

static void
sort(int *array, int sz, int flag)
{
	clock_t start_time, end_time;
	
	switch(flag) {
		case 0:
			sort_fptr = selection_sort_range;
			break;
		case 1:
			sort_fptr = insertion_sort_range;
			break;
		case 2:
			sort_fptr = shell_sort_range;
			break;
		case 3:
			sort_fptr = quick_sort_range;
			break;
		case 4:
			sort_fptr = quick_3way_sort_range;
			break;
		case 5:
			sort_fptr = merge_sort_topdown;
			break;
		case 6:
			sort_fptr = merge_sort_bottomup;
			break;
		case 7:
			sort_fptr = binary_isort_range;
			break;
		default: {
			fprintf(stderr, "None of sort algorithm.\n");
			return;
		}
	}

	start_time = clock();
	sort_fptr(array, 0, sz - 1, sizeof(int), less);
	end_time = clock();
	printf("Estimated time(s): %.3f\n", 
		(double)(end_time - start_time) /
		(double)CLOCKS_PER_SEC);
	if(CHECK_ORDERED(array, sz, sizeof(int), less))
		printf("Sort successful.\n");
	else
		printf("Sort failure.\n");

}
