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
#include "longestsubstring.h"

/* compare suffix s[p..] and suffix t[q..] */
static int compare(const char *, long, const char *, long);
static char * lcp(const char *, long, const char *, long);

/* 
 * Returns the longest common string of 
 * the two specified strings. 
 */
char * 
string_lcs(const char *s1, const char *s2)
{
	long len1, len2, i = 0, j = 0;
	long p, q;
	char *lcs, *x;
	struct string_suffixes ss1, ss2;

	strsuffix_init(&ss1, s1);
	strsuffix_init(&ss2, s2);

	len1 = strlen(s1), len2 = strlen(s2);
	lcs = (char *)algcalloc(MIN(len1, len2), sizeof(char));
	*lcs = '\0';

	/* find longest common substring by "merging" sorted suffixes */
	while (i < len1 && j < len2) {
		p = strsuffix_index(&ss1, i);
		q = strsuffix_index(&ss2, j);
		x = lcp(s1, p, s2, q);
		if (strlen(x) > strlen(lcs)) {
			memset(lcs, 0, MIN(len1, len2));
			strcpy(lcs, x);
		}
		ALGFREE(x);

		if (compare(s1, p, s2, q) < 0)
			i++;
		else
			j++;
	}

	STRSUFFIX_CLEAR(&ss1);
	STRSUFFIX_CLEAR(&ss2);

	return lcs;
}

/* Returns the longest repeated substring of the specified string. */
char * 
string_lrs(const char *text)
{
	long tlen, slen, i;
	char *lrs = "";
	struct string_suffixes ss;

	tlen = strlen(text);
	strsuffix_init(&ss, text);

	for (i = 1; i < tlen; i++) 
		if ((slen = strsuffix_lcp(&ss, i)) > (long)strlen(lrs)) 
			lrs = substring(text, strsuffix_index(&ss, i), 
					strsuffix_index(&ss, i) + slen);

	STRSUFFIX_CLEAR(&ss);

	return lrs;
}

/******************** static function boundary ********************/

/* compare suffix s[p..] and suffix t[q..] */
static int 
compare(const char *s1, long p, const char *s2, long q)
{
	long len1, len2, n, i;

	len1 = strlen(s1) - p;
	len2 = strlen(s2) - q;
	n = MIN(len1, len2);

	for (i = 0; i < n; i++)
		if (string_char_at(s1, p + i) != string_char_at(s2, q + i))
			return string_char_at(s1, p + i) - string_char_at(s2, q + i);

	/* 
	 * iff left characters all equally, 
	 * then need to compare with length.
	 */
	if (len1 < len2)
		return -1;
	else if (len1 > len2)
		return 1;
	else
		return 0;
}

/* 
 * Return the longest common prefix of suffix
 * s[p..] and suffix t[q..] 
 */
static char * 
lcp(const char *s1, long p, const char *s2, long q)
{
	long len1, len2, n, i;
	char *subs;

	len1 = strlen(s1) - p;
	len2 = strlen(s2) - q;
	n = MIN(len1, len2);

	for (i = 0; i < n; i++)
		if (string_char_at(s1, p + i) != string_char_at(s2, q + i)) {
			subs = substring(s1, p, p + i);
			return subs;
		}

	subs = substring(s1, p, p + n);
	return subs;
}
