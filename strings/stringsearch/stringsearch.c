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
#include "stringsearch.h"

#define STRING_RADIX	256

static inline int get_prime_number(void);
static long hash(const char *, int, int);
static int check(const char *, int, const char *, int);

/* 
 * String violence search. returns the index of the first occurrence 
 * of the pattern string in the text string.
 */
int 
string_violence_search(const char *pat, const char *txt)
{
	int i, j, tlen, plen;

	tlen = (int)strlen(txt), plen = (int)strlen(pat);
	for (i = 0; i <= tlen - plen; i++) {
		for (j = 0; j < plen; j++)
			if (string_char_at(txt, i + j) != string_char_at(pat, j))
				break;

		if (j == plen)	/* have found */
			return i;
	}
	
	/* not found */
	return tlen;
}

/* String violence search using index back. */
int 
violence_search_back(const char *pat, const char *txt)
{
	int i, j, tlen, plen;

	tlen = (int)strlen(txt), plen = (int)strlen(pat);
	for (i = 0, j = 0; i < tlen && j < plen; i++) {
		if (string_char_at(txt, i) == string_char_at(pat, j))
			j++;	/* match next */
		else {
			i -= j; /* back to not matched location */
			j = 0;	/* reset pattern index */
		}
	}

	if (j == plen)	/* have found */
		return i - plen;
	return tlen;
}

/* Knuth Morris Pratt search. */
int 
string_kmp_search(const char *pat, const char *txt)
{
	int i, j, x, c, tlen, plen;
	int **dfa;

	plen = (int)strlen(pat), tlen = (int)strlen(txt);
	dfa = (int **)algcalloc(STRING_RADIX, sizeof(int *));
	for (c = 0; c < STRING_RADIX; c++)
		dfa[c] = (int *)algcalloc(plen, sizeof(int));

	/* build DFA from pattern */
	for (c = 0; c < STRING_RADIX; c++)
		for (j = 0; j < plen; j++)
			dfa[c][j] = 0;
	dfa[string_char_at(pat, 0)][0] = 1;

	for (x = 0, j = 1; j < plen; j++) {
		for (c = 0; c < STRING_RADIX; c++)
			dfa[c][j] = dfa[c][x];	/* copy mismatch cases */
		dfa[string_char_at(pat, j)][j] = j + 1;	/* set match case. */
		x = dfa[string_char_at(pat, j)][x];	/* update restart state. */
	}

	for (i = 0, j = 0; i < tlen && j < plen; i++)
		j = dfa[string_char_at(txt, i)][j];

	for (c = 0; c < STRING_RADIX; c++)
		ALGFREE(dfa[c]);
	ALGFREE(dfa);
	
	if (j == plen)
		return i - plen;	/* found */
	return tlen;	/* not found */
}

/* Boyer Moore search */
int
boyer_moore_search(const char *pat, const char *txt)
{
	int i, j, c, plen, tlen, skip, *right;

	plen = (int)strlen(pat);
	tlen = (int)strlen(txt);

	/* position of rightmost occurrence of c in the pattern */
	right = (int *)algcalloc(STRING_RADIX, sizeof(int));
	for (c = 0; c < STRING_RADIX; c++)
		right[c] = -1;
	for (c = 0; c < plen; c++)
		right[string_char_at(pat, c)] = c;

	for (i = 0, skip = -1; i <= tlen - plen; i += skip) {
		skip = 0;
		for (j = plen - 1; j >= 0; j--)
			if (string_char_at(pat, j) != string_char_at(txt, i + j)) {
				skip = MAX(1, j - right[string_char_at(txt, i + j)]);
				break;
			}

		if (skip == 0)
			return i;
	}

	ALGFREE(right);

	return tlen;
}

/* Rabin karp search */
int 
rabin_karp_search(const char *pat, const char *txt)
{
	int plen, tlen, prim, i, offset;
	long rm, thash, phash;

	plen = (int)strlen(pat);
	tlen = (int)strlen(txt);

	if (tlen < plen)
		return tlen;

	prim = get_prime_number();
	thash = hash(txt, plen, prim);
	phash = hash(pat, plen, prim);

	/* check for match at offset 0 */
	if (thash == phash && check(pat, 0, txt, 0))
		return 0;

	/* precompute R^(m-1) % q for use in removing leading digit */
	rm = 1;
	for (i = 1; i <= plen - 1; i++)
		rm = (rm * STRING_RADIX) % prim;	/* R^(M - 1)%Q */

	/* check for hash match; if hash match, check for exact match */
	for (i = plen; i < tlen; i++) {
		/* Remove leading digit, add trailing digit, check for match. */
		thash = (thash + prim - rm * 
			string_char_at(txt, i - plen) % prim) % prim;
		thash = (thash * STRING_RADIX + string_char_at(txt, i)) % prim;

		/* match */
		offset = i - plen + 1;
		if (phash == thash && check(pat, 0, txt, offset))
			return offset;
	}

	/* no match */
	return tlen;
}

/******************** static function boundary ********************/

static inline int 
get_prime_number(void)
{
	const int primes[] = {
			  2147483647, 2147483629, 2147483587, 2147483579, 2147483563,
			  2147483549, 2147483543, 2147483497, 2147483489, 2147483477,
			  2147483423, 2147483399, 2147483353, 2147483323, 2147483269,
			  2147483249, 2147483237, 2147483179, 2147483171, 2147483137,
			  2147483123, 2147483077, 2147483069, 2147483059, 2147483053,
			  2147483033, 2147483029, 2147482951, 2147482949, 2147482943,
			  2147482937, 2147482921
			 };
	int i = rand_range_integer(0, 32);

	return primes[i];
}

static long 
hash(const char *key, int len, int prim)
{
	long h = 0; 
	int i;

	for (i = 0; i < len; i++) 
		h = (STRING_RADIX * h + string_char_at(key, i)) % prim;
	return h;
}

/* Las Vegas version: does pat[] match txt[i..i-m+1] ? */
static int 
check(const char *pat, int plen, const char *txt, int i)
{
	int j;
	
	for (j = 0; j < plen; j++)
		if (string_char_at(pat, j) != string_char_at(txt, i + j))
			return 0;
	return 1;
}
