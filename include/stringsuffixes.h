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
#ifndef _STRINGSUFFIXES_H_
#define _STRINGSUFFIXES_H_

#include "algcomm.h"

struct string_suffixes {
	char *text;		/* string text */
	long *index;	/* index[i] = j means text substring(j) */
					/* is ith largest suffix */
	long tlen;		/* number of characters in text */
};

/* Returns the length of the input string. */
#define STRSUFFIX_LENGTH(ss)	((ss)->tlen)

/* Returns the index into the original string of 
   the i th smallest suffix. */
static inline long 
strsuffix_index(const struct string_suffixes *ss, long i)
{
	if(i < 0 || i >= ss->tlen)
		errmsg_exit("Index %ld is not between 0 and %ld.\n", i, ss->tlen - 1);

	return ss->index[i];
}

#define STRSUFFIX_CLEAR(ss)		do {	\
	ALGFREE((ss)->text);				\
	ALGFREE((ss)->index);				\
	(ss)->tlen = 0;						\
} while(0)

/* Initializes a suffix array for the given text string */
void strsuffix_init(struct string_suffixes *ss, const char *txt);

/* 
 * Returns the length of the longest common prefix of the ith
 * smallest suffix and the i-1st smallest suffix. 
 */
long strsuffix_lcp(const struct string_suffixes *ss, long i);

/* Returns the ith smallest suffix as a string. */
char * strsuffix_select(const struct string_suffixes *ss, long i);

/* 
 * Returns the number of suffixes strictly less than the query string.
 * We note that rank(select(i)) equals i for each i between 0 and tlen - 1.
 */
long strsuffix_rank(const struct string_suffixes *ss, const char *query);

#endif	/* _STRINGSUFFIXES_H_ */
