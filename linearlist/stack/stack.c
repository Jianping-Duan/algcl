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
#include "stack.h"

static struct stack_node * make_node(const void *, unsigned short);

/* Adds the key to this stack. */
void
stack_push(struct stack *st, const void *key)
{
	struct stack_node *oldfirst;
	
	oldfirst = st->first;
	st->first = make_node(key, st->keysize);
	st->first->next = oldfirst;
	st->size++;
}

/* 
 * Returns (but does not remove) the item most recently 
 * added to this stack. 
 */
void *
stack_peek(const struct stack *st)
{
	if (STACK_ISEMPTY(st))
		return NULL;
	return st->first->key;
}

/* 
 * Removes and returns the key most recently 
 * added to this stack. 
 */
void
stack_pop(struct stack *st, void **key)
{
	struct stack_node *oldfirst;
	
	if (STACK_ISEMPTY(st))
		return;
	
	if (st->keysize == 0)
		*key = st->first->key;
	else
		memmove(*key, st->first->key, st->keysize);
	oldfirst = st->first;
	st->first = st->first->next;
	if (st->keysize != 0)
		ALGFREE(oldfirst->key);
	ALGFREE(oldfirst);
	st->size--;
}

/* Clears this stack. */
void
stack_clear(struct stack *st)
{
	void *key = NULL;

	if (st->keysize != 0)
		key = algmalloc(st->keysize);

	while (!STACK_ISEMPTY(st))
		stack_pop(st, &key);

	if (st->keysize != 0)
		ALGFREE(key);

}

/******************** static function boundary ********************/

static struct stack_node *
make_node(const void *key, unsigned short ksize)
{
	struct stack_node *current;
	
	current = (struct stack_node *)algmalloc(sizeof(struct stack_node));
	
	if (ksize == 0)
		current->key = (void *)key;
	else {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
		
	current->next = NULL;
	
	return current;
}
