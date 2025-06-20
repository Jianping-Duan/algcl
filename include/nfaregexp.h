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
#ifndef _NFAREGEXP_H_
#define _NFAREGEXP_H_

/* This implementation builds the NFA using a digraph and a stack
 * and simulates the NFA using digraph search.
 * The constructor takes time proportional to rlen, where rlen
 * is the number of characters in the regular expression.
 * The nfa_regexp_recog function takes time proportional to nrlen tlen,
 * where tlen is the number of characters in the text.
 */

#include "digraph.h"

struct nfa_regexp {
	struct digraph *eptran;	/* digraph of epsilon transitions */
	char *regexp;		/* regular expression */
	unsigned int rlen;	/* number of characters in regular expression */
};

#define NFA_REGEXP_CLEAR(nr)	do {	\
	digraph_clear((nr)->eptran);	\
	ALGFREE((nr)->eptran);		\
	ALGFREE((nr)->regexp);		\
	(nr)->rlen = 0;			\
} while(0)

/* Initializes the NFA from the specified regular expression. */
void nfa_regexp_init(struct nfa_regexp *nr, const char *regexp);

/* Returns true if the text is matched by the regular expression. */
bool nfa_regexp_recog(const struct nfa_regexp *nr, const char *txt);

#endif	/* _NFAREGEXP_H_ */
