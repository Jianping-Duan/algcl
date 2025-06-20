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
#include "nfaregexp.h"
#include "digraphdfs.h"
#include "linearlist.h"

/* Initializes the NFA from the specified regular expression. */
void 
nfa_regexp_init(struct nfa_regexp *nr, const char *regexp)
{
	int i, *lp, *or, ornext, *orv;
	int lpcnt = 0, rpcnt = 0;
	struct stack ops, orpath;

	nr->rlen = strlen(regexp);
	nr->regexp = (char *)algcalloc(nr->rlen + 1, sizeof(char));
	strcpy(nr->regexp, regexp);

	nr->eptran = (struct digraph *)algmalloc(sizeof(struct digraph));
	digraph_init(nr->eptran, nr->rlen + 1);

	STACK_INIT(&ops, sizeof(int));
	STACK_INIT(&orpath, sizeof(int)); 
	lp = (int *)algmalloc(sizeof(int));
	or = (int *)algmalloc(sizeof(int));
	orv = (int *)algmalloc(sizeof(int));

	for (i = 0; i < (int)nr->rlen; i++) {
		*lp = i;

		switch (nr->regexp[i]) {
		case '(':
		case '|':
			if (nr->regexp[i] == '(')
				lpcnt++;
			stack_push(&ops, &i);
			break;
		case ')':
			rpcnt++;

			while (!STACK_ISEMPTY(&ops)) {
				stack_pop(&ops, (void **)&or);

				switch (nr->regexp[*or]) {
				case '|':
					digraph_add_edge(nr->eptran, *or, i);
					ornext = (*or) + 1;
					stack_push(&orpath, &ornext);
					break;
				case '(':
					*lp = *or;
					/* only matchs first left parenthesis */
					/* in this stack */
					break;
				default:	/* none of others */
					assert(0);
				}
			}

			/* other edges */
			while (!STACK_ISEMPTY(&orpath)) {
				stack_pop(&orpath, (void **)&orv);
				digraph_add_edge(nr->eptran, *lp, *orv);
			}

			break;
		default:
			break;
		}
			
		/* closure operator (uses 1-character lookahead) */
		if (i < (int)nr->rlen - 1) {
			switch (nr->regexp[i + 1]) {
			case '*':
				digraph_add_edge(nr->eptran, *lp, i + 1);
				digraph_add_edge(nr->eptran, i + 1, *lp);
				break;
			case '+':
				digraph_add_edge(nr->eptran, i + 1, *lp);
				break;
			case '?':	/* may has a bug. */
				digraph_add_edge(nr->eptran, *lp, i + 1);
				break;
			default:
				break;
			}
		}

		switch (nr->regexp[i]) {
		case '(':
		case '*':
		case '+':
		case '?':
		case ')':
			digraph_add_edge(nr->eptran, i, i + 1);
			break;
		default:
			break;
		}
	}

	ALGFREE(lp);
	ALGFREE(or);
	ALGFREE(orv);

	if (lpcnt != rpcnt) {
		stack_clear(&ops);
		stack_clear(&orpath);
		errmsg_exit("Invalid regular expression.\n");
	}
	stack_clear(&ops);
	stack_clear(&orpath);
}

/* Returns true if the text is matched by the regular expression. */
bool 
nfa_regexp_recog(const struct nfa_regexp *nr, const char *txt)
{
	struct digraph_dfs dfs;
	struct single_list pc, match;
	struct slist_node *nptr;
	unsigned int v, *w, x;
	unsigned int i, tlen;

	digraph_dfs_init(&dfs, nr->eptran, 0);
	slist_init(&pc, sizeof(int), NULL);
	for (v = 0; v < DIGRAPH_VERTICES(nr->eptran); v++)
		if (DIGRAPH_DFS_MARKED(&dfs, v))
			slist_append(&pc, &v);

	slist_init(&match, sizeof(int), NULL);
	tlen = strlen(txt);
	for (i = 0; i < tlen; i++) {
		switch (txt[i]) {
		case '*':
		case '+':
		case '|':
		case '(':
		case ')':
			errmsg_exit("text contains the metacharacter '%c'\n",
				txt[i]);
		default:
			break;
		}

		SLIST_FOREACH(&pc, nptr, unsigned int, w) {
			if (*w == nr->rlen)
				continue;
			if (nr->regexp[*w] == txt[i] || nr->regexp[*w] == '.') {
				x = (*w) + 1;	/* routine edge */
				slist_append(&match, &x);
			}
		}

		if (SLIST_LENGTH(&match) == 0)
			continue;

		DIGRAPH_DFS_CLEAR(&dfs);
		slist_clear(&pc);
		digraph_dfs_svset(&dfs, nr->eptran, &match);
		slist_init(&pc, sizeof(int), NULL);
		for (v = 0; v < DIGRAPH_VERTICES(nr->eptran); v++)
			if (DIGRAPH_DFS_MARKED(&dfs, v))
				slist_append(&pc, &v);

		/* optimization if no states reachable */
		if (SLIST_LENGTH(&pc) == 0) {
			DIGRAPH_DFS_CLEAR(&dfs);
			slist_clear(&match);
			slist_clear(&pc);
			return false;
		}

		slist_clear(&match);
		slist_init(&match, sizeof(int), NULL);
	}

	DIGRAPH_DFS_CLEAR(&dfs);
	slist_clear(&match);

	/* check for accept state */
	SLIST_FOREACH(&pc, nptr, unsigned int, w) {
		if (*w == nr->rlen) {
			slist_clear(&pc);
			return true;
		}
	}

	return false;
}
