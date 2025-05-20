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

#define MAX_SORTS	8
#define MIN_ITEMS	100

static int less(const void *, const void *);
static void (*sort_fptr)(void *, long, long, unsigned int, algcomp_ft *);
static void sort(int *, int, int);

int
main(int argc, char *argv[])
{
	int i, j, sz;
	int (*array[MAX_SORTS]);

	const char sortmsg[MAX_SORTS][80] = {
		"Begin tests Insertion-Sort",
		"Begin tests Selection-Sort",
		"Begin tests Shell-Sort",
		"Begin tests Quick-Sort",
		"Begin tests Quick-3way-Sort",
		"Begin tests Merge-Sort for Top-Down",
		"Begin tests Merge-Sort for Bottom-Up",
		"Begin tests Binary Insertion Sort"
	};

	if (argc != 2)
		errmsg_exit("Usage: %s <size>\n", argv[0]);

	if (sscanf(argv[1], "%d", &sz) != 1)
		errmsg_exit("Illegal integer number, %s\n", argv[1]);
	if (sz < MIN_ITEMS) {
		errmsg_exit("Given a integer number must be equal or greater than %d",
			MIN_ITEMS);
	}

	SET_RANDOM_SEED;

	for (i = 0; i < MAX_SORTS; i++) {
		array[i] = (int *)algmalloc(sz * sizeof(int));
		for (j = 0; j < sz; j++)
			array[i][j] = rand_range_integer(0, sz * 2);
	}

	for (i = 0; i < MAX_SORTS; i++) {
		printf("%s\n", sortmsg[i]);
		sort(array[i], sz, i);
		printf("\n");
	}

	for (i = 0; i < MAX_SORTS; i++)
		ALGFREE(array[i]);

	return 0;
}

static int
less(const void *k1, const void *k2)
{
	int *x = (int *)k1, *y = (int *)k2;

	if (*x < *y)
		return 1;
	else if (*x > *y)
		return -1;
	else
		return 0;
}

static void
sort(int *array, int sz, int flag)
{
	clock_t start_time, end_time;
	
	switch (flag) {
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
		(double)(end_time - start_time) / (double)CLOCKS_PER_SEC);
	if (CHECK_ORDERED(array, sz, sizeof(int), less))
		printf("Sort successful.\n");
	else
		printf("Sort failure.\n");

}
