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
#ifndef _ALGCOMM_H_
#define _ALGCOMM_H_

/* Commonly used header file. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

/* The max or min function that may be used. */
#define MAX(X, Y)		((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)		((X) < (Y) ? (X) : (Y))

#define MAX_KEY_LEN		64
#define MIN_KEY_LEN		2
#define BUFFER_SIZE		8192

/* Sets random seed */
#define SET_RANDOM_SEED		srand((unsigned)time(NULL))

/* key-value pair structure */
struct element {
	char key[MAX_KEY_LEN];
	long value;
};

/* The comparison function type */
typedef int algcomp_ft(const void *key1, const void *key2);

/* The encapsulation of free() */
#define	ALGFREE(ptr)	do {	\
	if (ptr != NULL) {	\
		free(ptr);	\
		ptr = NULL;	\
	}			\
} while (0)

/* Prints error message and exit. */
void errmsg_exit(const char *fmt, ...);

/* The encapsulation of malloc() */
void * algmalloc(size_t size);

/* The encapsulation of calloc() */
void * algcalloc(size_t nmemb, size_t size);

/* The encapsulation of realloc() */
void * algrealloc(void *ptr, size_t size);

/* 
 * Returns a random integer-number range from SI to EI - 1 
 */
unsigned int rand_range_integer(unsigned int si, unsigned int ei);

/* 
 * Returns a random floating-number range from SI to EI - 1 
 */
double rand_range_float(double si, double ei);

/* 
 * Returns a random string that it length is N,
 * its value need to be free.
 */
char * rand_string(short n);

/* 
 * Returns a random boolean from a Bernoulli distribution 
 * with success probability p 
 */
bool bernoulli_distribution(double p);

/* 
 * Rearranges the elements of the specified array 
 * in random integer-number order. 
 */
void shuffle_uint_array(unsigned int *arr, unsigned int n);

/* The encapsulation of fopen() */
FILE * open_file(const char *filename, const char *mode);

/* The encapsulation of fclose() */
void close_file(FILE *fp);

/* The encapsulation of strtok() */
void string_tokens(const char *str, const char *seps, char **tokens,
		unsigned short len, unsigned int *sz);

/* Erases the end of string of 'enter' */
char * string_erase_rn(char *str);

/* 
 * Returns the character of string 
 * at the specified index. 
 */
int string_char_at(const char *str, int d);

/* 
 * Removes the char at the specified 
 * position in this string. 
 */
char delete_char_at(char *str, unsigned int i);

/* 
 * Reads one line text from the input stream. 
 * its retrun value need to be free. 
 */
char * string_read_line(FILE *istream);

/* 
 * Reads all text from the input stream. 
 * its return value need to be free. 
 */
char * string_read_all(FILE *istream);

/* Returns a substring from a string. */
char * substring(const char *str, long lo, long hi);

#endif /* _ALGCOMM_H_ */
