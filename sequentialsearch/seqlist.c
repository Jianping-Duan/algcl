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
#include "seqlist.h"
#include "queue.h"

static struct seqlist_node * make_node(const struct element *);

/* 
 * Inserts the specified key-value pair into 
 * the sequentail list.
 */
void
seqlist_put(struct seqlist *slist, const struct element *item)
{
	struct seqlist_node *newnode;
	
	newnode = make_node(item);
	if(slist->first != NULL)
		newnode->next = slist->first;
	slist->first = newnode;
	slist->len++;
}

/* 
 * Returns the value associated with the given key 
 * in this sequentail list.
 */
struct element * 
seqlist_get(const struct seqlist *slist, const char *key)
{
	struct seqlist_node *pnode;
	struct element *el;
	
	pnode = slist->first;
	while(pnode != NULL) {
		el = &(pnode->item);
		if(strcmp(el->key, key) == 0)
			return el;
		pnode = pnode->next;
	}
	
	return NULL;
}

/* 
 * Removes the specified key and its associated value 
 * from the sequentail list. 
 */
void 
seqlist_delete(struct seqlist *slist, const char *key)
{
	struct seqlist_node *pnode, *pnext;
	
	if(slist != NULL && slist->len > 0) {
		/* only one node */
		if(slist->first->next == NULL && 
			strcmp(slist->first->item.key, key) == 0) {
			ALGFREE(slist->first);
			slist->len = 0;
		}
		/* key equals the first node. */
		else if(slist->first->next != NULL && 
			strcmp(slist->first->item.key, key) == 0) {
			pnode = slist->first;
			slist->first = slist->first->next;
			ALGFREE(pnode);
			slist->len--;
		}
		else {
			pnode = slist->first;
			while(pnode != NULL) {
				/* from second node begin. */
				pnext = pnode->next; 
				
				/* pointer ended and pnode is last node. */
				if(pnext == NULL)
					return;
				
				if(strcmp(pnext->item.key, key) == 0) {
					/* key is the last node. */
					if(pnext->next == NULL) 
						pnode->next = NULL;
					else
						pnode->next = pnext->next;
					ALGFREE(pnext);
					slist->len--;
				}
				pnode = pnode->next;
			}
		}
	}		
}

/* 
 * Changes the specified key of value 
 * from this sequentail list. 
 */
void 
seqlist_change(struct seqlist *slist, const char *key,
			const struct element *item)
{
	struct seqlist_node *pnode;
	
	pnode = slist->first;
	while(pnode != NULL) {
		if(strcmp(pnode->item.key, key) == 0)
			pnode->item.value = item->value;
		pnode = pnode->next;
	}
}

/* Returns all keys in the sequentail list. */
void 
seqlist_keys(const struct seqlist *slist, struct queue *keys)
{
	struct seqlist_node *pnode;
	
	pnode = slist->first;
	while(pnode != NULL) {
		enqueue(keys, pnode->item.key);
		pnode = pnode->next;
	}
}

/* Clear this single linked list */
void
seqlist_clear(struct seqlist *slist)
{
	struct seqlist_node *pnode, *current;
	
	pnode = slist->first;
	while(pnode != NULL) {
		current = pnode;
		pnode = pnode->next;
		ALGFREE(current);
	}
	slist->len = 0;
}

/******************** static function boundary ********************/

/* 
 * Creates sequentail list node using 
 * the specified key-value pair. 
 */
static struct seqlist_node * 
make_node(const struct element *item)
{
	struct seqlist_node *current;
	
	current = (struct seqlist_node *)algmalloc(sizeof(struct seqlist_node));
	
	current->item = *item;
	current->next = NULL;
	
	return current;
}
