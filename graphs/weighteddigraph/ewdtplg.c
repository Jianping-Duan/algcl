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
#include "topological/ewditopological.h"
#include "singlelist.h"

static void display(struct single_list *);

int 
main(int argc, char *argv[])
{
	FILE *fin;
	struct ewdigraph g;
	struct single_list ord1, ord2;
	
	if (argc != 2)
		errmsg_exit("Usage: %s <filename>\n", argv[0]);

	fin = open_file(argv[1], "r");
	ewdigraph_init_fistream(&g, fin);
	close_file(fin);
	if (EWDIGRAPH_VERTICES(&g) < 100)
		ewdigraph_print(&g);
	printf("\n");

	printf("Topological use depth first order:\n");
	ewdtplg_use_dfso(&g, &ord1);
	display(&ord1);

	printf("Topological use queue:\n");
	ewdtplg_use_queue(&g, &ord2);
	display(&ord2);

	slist_clear(&ord1);
	slist_clear(&ord2);
	ewdigraph_clear(&g);
	
	return 0;
}

static void 
display(struct single_list *slist)
{
	unsigned int *key;
	struct slist_node *nptr;

	SLIST_FOREACH(slist, nptr, unsigned int, key) {
		printf("%d ", *key);
	}
	printf("\n");
}
