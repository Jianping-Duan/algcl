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
#include "floydwarshallsp.h"
#include "singlelist.h"
#include <getopt.h>

static void usage_info(const char *);

int 
main(int argc, char *argv[])
{
	struct adjmatrix_ewdigraph g;
	struct floyd_warshall_sp sp;
	unsigned int vs, es, v, w;
	struct single_list paths;
	struct slist_node *nptr;
	struct diedge *e;
	char se[128];

	int op;
	const char *optstr = "v:e:";

	extern char *optarg;
	extern int optind;
	
	SET_RANDOM_SEED;

	if (argc != (int)strlen(optstr) + 1)
		usage_info(argv[0]);
	
	while ((op = getopt(argc, argv, optstr)) != -1) {
		switch (op) {
		case 'v':
			if (sscanf(optarg, "%u", &vs) != 1)
				errmsg_exit("Illegal number. -v %s\n", optarg);
			break;
		case 'e':
			if (sscanf(optarg, "%u", &es) != 1)
				errmsg_exit("Illegal number. -e %s\n", optarg);
			break;
		default:
			fprintf(stderr, "Parameters error.\n");
			usage_info(argv[0]);
		}
	}
	
	if (optind < argc)
		usage_info(argv[0]);
	
	adjmatewdg_init_randomly(&g, vs, es);
	adjmatewdg_print(&g);
	printf("\n");

	fwsp_init(&sp, &g);

	printf("Prints all-pairs shortest path distances.\n");
	printf("     ");
	for (v = 0; v < ADJMATEWDG_VERTICES(&g); v++)
		printf("%6d ", v);
	printf("\n");
	for (v = 0; v < ADJMATEWDG_VERTICES(&g); v++) {
		printf("%6d: ", v);
		for (w = 0; w < ADJMATEWDG_VERTICES(&g); w++)
			if (FWSP_HAS_PATH(&sp, v, w))
				printf("%6.2f ",
					(double)fwsp_distto(&sp, v, w));
			else
				printf("   Inf ");
		printf("\n");
	}

	printf("Prints all-pairs shortest paths.\n");
	if (FWSP_HAS_NEGATIVE_CYCLE(&sp))
		goto has_negcycle;

	for (v = 0; v < ADJMATEWDG_VERTICES(&g); v++)
		for (w = 0; w < ADJMATEWDG_VERTICES(&g); w++)
			if (FWSP_HAS_PATH(&sp, v, w)) {
				fwsp_path_get(&sp, v, w, &paths);
				SLIST_FOREACH(&paths, nptr, struct diedge, e) {
					DIEDGE_STRING(e, se);
					printf("%s  ", se);
				}
				printf("\n");
				slist_clear(&paths);
			} else {
				printf("%u to %u no path.\n", v, w);
			}

has_negcycle:
	
	adjmatewdg_clear(&g);
	fwsp_clear(&sp);
	
	return 0;
}

static void 
usage_info(const char *pname)
{
	fprintf(stderr, "Usage: %s -v -e\n", pname);
	fprintf(stderr, "-f: The data file for the edge-weighted digraph.\n");
	fprintf(stderr, "-s: The soruce vertex of the edge-weighted "
		"digraph.\n");
	exit(EXIT_FAILURE);
}
