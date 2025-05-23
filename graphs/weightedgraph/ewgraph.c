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
#include "edgeweightedgraph.h"

int
main(int argc, char *argv[])
{
	FILE *fin;
	struct ewgraph g[3];
	int i;
	
	if (argc != 2)
		errmsg_exit("Usage: %s <datafile>\n", argv[0]);

	SET_RANDOM_SEED;
	
	printf("Prints a graph from input stream.\n");
	fin = open_file(argv[1], "r");
	ewgraph_init_fistream(&g[0], fin);
	close_file(fin);
	ewgraph_print(&g[0]);
	printf("\n");

	printf("Copies current edge-weighted graph and print it.\n");
	ewgraph_clone(&g[0], &g[1]);
	ewgraph_print(&g[1]);
	printf("\n");

	printf("Generates a random edge-weighted graph.\n");
	ewgraph_init_randomly(&g[2], 10, 15);
	ewgraph_print(&g[2]);
	ewgraph_clear(&g[2]);

	for (i = 0; i < 3; i++)
		ewgraph_clear(&g[i]);

	return 0;
}
