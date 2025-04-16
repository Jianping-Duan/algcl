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
#ifndef _STACK_H_
#define _STACK_H_

#include "algcomm.h"

struct stack_node {
	void *key;
	struct stack_node *next;
};

struct stack {
	struct stack_node *first;	/* top of stack */
	unsigned long size;			/* size of the stack */
	unsigned short keysize;
};

/* Is this stack empty? */
#define STACK_ISEMPTY(s)	((s)->first == NULL)

/* Returns the number of keys in the stack. */
#define STACK_SIZE(s)	((s)->size)

/* Initializes an empty stack */
#define STACK_INIT(st, ksize)	do {	\
	(st)->first = NULL;					\
	(st)->size = 0;						\
	(st)->keysize = ksize;				\
} while (0)

/* Adds the key to this stack. */
void stack_push(struct stack *st, const void *key);

/* 
 * Removes and returns the key most recently 
 * added to this stack. 
 */
void stack_pop(struct stack *st, void **key);

/* 
 * Returns (but does not remove) the key most 
 * recently added to this stack. 
 */
void * stack_peek(const struct stack *st);

/* Clears this stack. */
void stack_clear(struct stack *st);

#endif /* _STACK_H_ */
