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
#ifndef _SEPARATECHAINHASH_H_
#define _SEPARATECHAINHASH_H_

#include "algcomm.h"

struct seqlist;
struct queue;

struct schain_hash {
	unsigned long pairs;	/* number of key-value pairs */
	unsigned long size;	/* hash table size */
	struct seqlist *lists;	/* array of linked-list */
};

/* Returns the separate-chain hash table capacity */
#define SCHASH_SIZE(sch)	((sch)->size)

/* 
 * Returns the number of key-value pairs in 
 * this separate-chain hash table.
 */
#define SCHASH_PAIRS(sch)	((sch)->pairs)

/* 
 * Returns true if this separate-chain hash 
 * table is full.
 */
#define SCHASH_ISFULL(sch)	\
	((sch)->pairs < (sch)->size ? 0 : 1)

/* Initializes an empty separate-chains hash table. */
void schash_init(struct schain_hash *sch, unsigned long htsize);

/* 
 * Returns the value associated with the specified key 
 * in the separate-chains hash table.
 */
struct element * schash_get(const struct schain_hash *sch, const char *key);

/* 
 * Inserts the specified key-value pair into 
 * the separate-chains hash table.
 */
void schash_put(struct schain_hash *sch, const struct element *item);

/* 
 * Removes the specified key and its associated value
 * from this separate-chains hash table.
 */
void schash_delete(struct schain_hash *sch, const char *key);

/* Returns all keys in this separate-chains hash table. */
void schash_keys(const struct schain_hash *sch, struct queue *keys);

/* Clears this separate-chains hash table. */
void schash_clear(struct schain_hash *sch);

#endif /* _SEPARATECHAINHASH_H_ */
