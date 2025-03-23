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
#include "gabowscc.h"
#include "singlelist.h"

static void display(const struct single_list *);
static int vequal(const void *, const void *);

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct digraph g;
	struct gabow_scc scc;
	struct single_list *comps;
	unsigned int i, m, v;

	if(argc != 2)
		errmsg_exit("Usage: %s <datafile> \n", argv[0]);
	
	fin = open_file(argv[1], "r");
	digraph_init_fistream(&g, fin);
	close_file(fin);
	digraph_print(&g);
	printf("\n");

	gbscc_init(&scc, &g);

	/* number of connected components */
	m = GBSCC_COUNT(&scc);
	printf("%u strong components.\n", m);

	/* compute list of vertices in each strong component */
	comps = (struct single_list *)algcalloc(m, sizeof(struct single_list));
	for(i = 0; i < m; i++)
		slist_init(&comps[i], sizeof(int), vequal);

	for(v = 0; v < DIGRAPH_VERTICES(&g); v++)
		slist_append(&comps[GBSCC_ID(&scc, v)], &v);
	
	for(i = 0; i < m; i++) {
		display(&comps[i]);
		slist_clear(&comps[i]);
	}

	ALGFREE(comps);
	GBSCC_CLEAR(&scc);

	return 0;
}

static void 
display(const struct single_list *slist)
{
	unsigned int *key;
	struct slist_node *nptr;

	SLIST_FOREACH(slist, nptr, unsigned int, key) {
		printf("%d ", *key);
	}
	printf("\n");
}

static int 
vequal(const void *k1, const void *k2)
{
	int *v = (int *)k1, *w = (int *)k2;
	return *v == *w;
}
