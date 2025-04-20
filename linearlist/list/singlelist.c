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
#include "singlelist.h"

static struct slist_node * make_node(const void *, unsigned int);
static struct slist_node * clone_node(struct slist_node *, struct slist_node **,
	unsigned int);

void
slist_init(struct single_list *slist, unsigned int ksize, algcomp_ft *equal)
{
	slist->first = NULL;
	slist->last = NULL;
	slist->size = 0;
	slist->keysize = ksize;
	slist->equal = equal;
}

/* Inserts a key at the first location. */
void 
slist_put(struct single_list *slist, const void *key)
{
	struct slist_node *newnode;
	
	newnode = make_node(key, slist->keysize);
	
	if (SLIST_ISEMPTY(slist)) {
		slist->first = newnode;
		slist->last = newnode;
	} else {
		newnode->next = slist->first;
		slist->first = newnode;
	}

	slist->size++;
}

/* Inserts a key at the last location. */
void
slist_append(struct single_list *slist, const void *key)
{
	struct slist_node *newnode;
	
	newnode = make_node(key, slist->keysize);

	if (SLIST_ISEMPTY(slist)) {
		slist->first = newnode;
		slist->last = newnode;
	} else {
		slist->last->next = newnode;
		slist->last = newnode;
	}

   	slist->size++;
}

/* 
 * Does the single linked-list contain the given key. 
 * if contained, return the index of loction, -1 is not contain.
 */
long
slist_contains(const struct single_list *slist, const void *key)
{
	struct slist_node *current;
	long loc = 0;

	if (slist->equal == NULL) {
		errmsg_exit("Don't execute this 'slist_contains' function, "
			"because equal initialize is null.\n");
	}
	
	current = slist->first;
	while (current != NULL) {
		if (slist->equal(current->key, key) == 0)
			return loc;
		current = current->next;
		loc++;
	}
	return -1;
}

/* 
 * Removes the specified key from this single linked-list. 
 */
void
slist_delete(struct single_list *slist, const void *key)
{
	struct slist_node *current, *pnext;

	if (slist->equal == NULL) {
		errmsg_exit("Don't execute this 'slist_delete' function, "
			"because equal initialize is null.\n");
	}

	if (SLIST_ISEMPTY(slist) || slist->size == 0)
		return;
	
	/* only one node */
	if (slist->first == slist->last &&
		slist->equal(slist->first->key, key) == 0) {
		ALGFREE(slist->first);
		slist->last = NULL;
		slist->size = 0;
	}
	/* key equals first node */
	else if (slist->equal(slist->first->key, key) == 0) {
		current = slist->first->next;
		ALGFREE(slist->first);
		slist->first = current;
		slist->size--;
	} else {
		current = slist->first;
		while (current != NULL) {
			/* from second node begin */
			pnext = current->next;
				
			/* pointer ended and current is last node */
			if (pnext == NULL)
				return;
				
			if (slist->equal(pnext->key, key) == 0) {
				/* key is last node */
				if (pnext == slist->last) {
					current->next = NULL;
					slist->last = current;
				} else {
					current->next = pnext->next;
				}
				ALGFREE(pnext);
				slist->size--;
			}
			current = current->next;
		}
	}
}

/* Changes the specified key SKEY become to TKEY. */
void 
slist_change(struct single_list *slist, const void *skey, const void *tkey)
{
	struct slist_node *current;

	if (slist->equal == NULL) {
		errmsg_exit("Don't execute this 'slist_change' function, "
			"because equal initialize is null.\n");
	}
	
	current = slist->first;
	while (current != NULL) {
		if (slist->equal(current->key, skey) == 0) {
			slist->keysize == 0 ? current->key = (void *)tkey :
				memcpy(current->key, tkey, slist->keysize);
		}
		current = current->next;
	}
}

/* Reverses this single linked-list. */
void
slist_reverse(struct single_list *slist)
{
	struct slist_node *prev, *current, *pnext;
	
	prev = NULL;
	current = slist->first;
	slist->last = slist->first;	
	pnext = current->next;
	while (pnext != NULL) {
		current->next = prev;
		prev = current;
		current = pnext;
		pnext = pnext->next;
	}

	/* current is orginal last node */
	current->next = prev;
	slist->first = current;
}

/* Clones this single linked-list and output new. */
void
slist_clone(const struct single_list *slist, struct single_list *tlist)
{
	if (slist->keysize == 0) {
		errmsg_exit("Not use clone for the single linked list of "
			"key-size is 0.\n");
	}

	tlist->first = clone_node(slist->first, &tlist->last, slist->keysize);
	tlist->size = slist->size;
	tlist->keysize = slist->keysize;
	tlist->equal = slist->equal;
}

/* Clears this single linked list. */
void
slist_clear(struct single_list *slist)
{
	struct slist_node *current, *pnext;
	
	current = slist->first;
	while (current != NULL) {
		pnext = current->next;
		if (slist->keysize != 0)
			ALGFREE(current->key);
		ALGFREE(current);
		current = pnext;
	}
	slist->size = 0;
}

/******************** static function boundary ********************/

/* Creates new node and return it. */
static struct slist_node * 
make_node(const void *key, unsigned int ksize)
{
	struct slist_node *current;
	
	current = (struct slist_node *)algmalloc(sizeof(struct slist_node));
	
	if (ksize == 0)	/* not to copy */
		current->key = (void *)key;
	else {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
	
	current->next = NULL;
	
	return current;
}

static struct slist_node * 
clone_node(struct slist_node *node, struct slist_node **last,
		unsigned int ksize)
{
	struct slist_node *current;
	
	/* next == NULL if the linked list was ended */
	if (node == NULL)	
		return NULL;

	if (node->next == NULL)
		*last = node;
	
	current = make_node(node->key, ksize);
	current->next = clone_node(node->next, &node->next, ksize);
	
	return current;
}
