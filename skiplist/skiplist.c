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
#include "skiplist.h"
#include "singlelist.h"

/* returns a random value in [0...1] */
#define SL_FRACTION		((double)rand() / (double)RAND_MAX)

#define SL_PROBABILITY	0.5

static inline void pointers_init(struct skipl_node **, int);
static int random_level(double, int);

/* 
 * An element NIL is allocated and given a key greater 
 * than any legal key. 
 * All levels of all skip lists are terminated with NIL.
 * A new list is initialized so that the level of the 
 * list is equal to 0 and all forward pointers of the 
 * list’s header point to NIL.
 *
 * Since we can safely cap levels at L(n), we should choose 
 * MaxLevel = L(N) (where N is an upper bound on the number
 * of elements in a skip list). 
 * If p = 1/2, using MaxLevel = 16 is appropriate for data 
 * structures containing up to 216 elements.
 */
void 
skipl_init(struct skip_list *sl, int maxlvl, unsigned int ksize,
		algcomp_ft *cmp)
{
	sl->maxlevel = maxlvl;
	sl->level = 0;
	sl->size = 0;
	sl->keysize = ksize;
	sl->cmp = cmp;
	sl->head = (struct skipl_node *)algmalloc(sizeof(struct skipl_node));

	sl->head->key = NULL;
	sl->head->forward = (struct skipl_node **)
		algcalloc(sl->maxlevel + 1, sizeof(struct skipl_node *));
	pointers_init(sl->head->forward, maxlvl);

	SET_RANDOM_SEED;
}

/* 
 * We search for an element by traversing forward pointers
 * that do not overshoot the node containing the element
 * being searched for this function. 
 * When no more progress can be made at the current level
 * of forward pointers, the search moves down to the next
 * level. When we can make no more progress at level 0,
 * we must be immediately in front of the node that
 * contains the desired element (if it is in the list).
 */
void *
skipl_get(const struct skip_list *sl, const void *key)
{
	struct skipl_node *current;
	int i;

	if (key == NULL)
		errmsg_exit("calls skipl_get() with null argument.\n");

	current = sl->head;
	for (i = sl->level; i >= 0; i--)
		while (current->forward[i] != NULL &&
			sl->cmp(current->forward[i]->key, key) == 1) {
			current = current->forward[i];
		}

	if ((current = current->forward[0]) != NULL &&
		sl->cmp(current->key, key) == 0) {
		return current->key;
	}
	return NULL;
}

/* 
 * To insert or delete a node, we simply search and splice,  
 * A vector update is maintained so that when the search is
 * complete (and we are ready to perform the splice), 
 * update[i] contains a pointer to the rightmost node of
 * level i or higher that is to the left of the location
 * of the insertion/deletion.
 *
 * If an insertion generates a node with a level greater
 * than the previous maximum level of the list, 
 * we update the maximum level of the list and initialize
 * the appropriate portions of the update vector. 
 * After each deletion, we check if we have deleted the
 * maximum element of the list and if so, 
 * decrease the maximum level of the list.
 */
void
skipl_put(struct skip_list *sl, const void *key)
{
	struct skipl_node **update, *current, *newnode;
	int i, lvl;

	if (key == NULL)
		errmsg_exit("calls skipl_put() with null argument.\n");

	update = (struct skipl_node **)algcalloc(sl->maxlevel + 1, 
		sizeof(struct skipl_node *));
	pointers_init(update, sl->maxlevel);

	current = sl->head;	
	for (i = sl->level; i >= 0; i--) {
		while (current->forward[i] != NULL && 
			  sl->cmp(current->forward[i]->key, key) == 1) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];
	if (current == NULL || sl->cmp(current->key, key) != 0) {
		lvl = random_level(SL_PROBABILITY, sl->maxlevel);
		if (lvl > sl->level) {
			for (i = sl->level + 1; i <= lvl; i++)
				update[i] = sl->head;
			sl->level = lvl;
		}

		newnode = (struct skipl_node *)algmalloc(sizeof(struct skipl_node));
		if (sl->keysize != 0) {
			newnode->key = algmalloc(sl->keysize);
			memcpy(newnode->key, key, sl->keysize);
		} else
			newnode->key = (void *)key;
		newnode->forward = (struct skipl_node **)
			algcalloc(lvl + 1, sizeof(struct skipl_node *));
		pointers_init(newnode->forward, lvl);
		for (i = 0; i <= lvl; i++) {
			newnode->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = newnode;
		}

		sl->size++;
	}

	ALGFREE(update);
}

/* 
 * Removes the specified key and its associated with value
 * from this skip list. 
 */
void 
skipl_delete(struct skip_list *sl, const void *key)
{
	struct skipl_node **update, *current;
	int i;

	if (key == NULL)
		errmsg_exit("calls skipl_delete() with null argument.\n");

	update = (struct skipl_node **)
		algcalloc(sl->maxlevel + 1,	sizeof(struct skipl_node *));
	pointers_init(update, sl->maxlevel);

	current = sl->head;	
	for (i = sl->level; i >= 0; i--) {
		while (current->forward[i] != NULL && 
			sl->cmp(current->forward[i]->key, key) == 1) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];
	if (current != NULL && sl->cmp(current->key, key) == 0) {
		for (i = 0; i <= sl->level; i++) {
			if (update[i]->forward[i] != current)
				break;
			update[i]->forward[i] = current->forward[i];
		}
		if (sl->keysize != 0)
			ALGFREE(current->key);
		ALGFREE(current);

		while (sl->level > 0 && sl->head->forward[i] == NULL)
			sl->level--;

		sl->size--;
	}

	ALGFREE(update);
}

/* Gets all keys from this skip list */
void 
skipl_keys(const struct skip_list *sl, struct single_list *keys)
{
	struct skipl_node *current;

	slist_init(keys, 0, sl->cmp);
	current = sl->head;
	while (current->forward[0] != NULL) {
		slist_append(keys, current->forward[0]->key);
		current = current->forward[0];
	}
}

/* Clears this skip list. */
void 
skipl_clear(struct skip_list *sl)
{
	struct skipl_node *current, *next;

	current = sl->head;
	while (current != NULL) {
		next = current->forward[0];
		if (sl->keysize != 0)
			ALGFREE(current->key);
		ALGFREE(current->forward);
		ALGFREE(current);
		current = next;
	}

	sl->size = 0;
	sl->level = 0;
	sl->keysize = 0;
	sl->cmp = NULL;
}

/* Returns the smallest key in the skip list. */
void * 
skipl_min(const struct skip_list *sl)
{
	if (SKIPL_ISEMPTY(sl))
		return NULL;
	return sl->head->forward[0]->key;
}

/* Returns ths largest key in ths skip list. */
void * 
skipl_max(const struct skip_list *sl)
{
	struct skipl_node *current;
	int i;

	current = sl->head;
	for (i = sl->level; i >= 0; i--) {
		while (current->forward[i] != NULL)
			current = current->forward[i];
		if (current->forward[i] == NULL)
			break;
	}

	if (i + 1 == 0 && current->forward[0] == NULL)
		return current->key;

	while (current->forward[0] != NULL)
		current = current->forward[0];

	return current->key;
}

/* 
 * Returns the largest key in the BST less than or 
 * equal to the specified key. 
 */
void * 
skipl_floor(const struct skip_list *sl, const void *key)
{
	struct skipl_node *current;
	int i;

	if (key == NULL)
		errmsg_exit("calls skipl_floor() with null argument.\n");

	if (SKIPL_ISEMPTY(sl))
		errmsg_exit("calls skipl_floor() with empty skip list.\n");

	current = sl->head;
	for (i = sl->level; i >= 0; i--)
		while (current->forward[i] != NULL && 
			  sl->cmp(current->forward[i]->key, key) == 1) {
			current = current->forward[i];
		}

	if (current == sl->head)
		errmsg_exit("argument to skipl_floor() is too small.\n");

	if (current->forward[0] != NULL && 
		sl->cmp(current->forward[0]->key, key) == 0) {
		return (current->forward[0]->key);
	}
	return (current->key);

}

/* 
 * Returns the smallest key in the BST greater than 
 * or equal to the specified key. 
 */
void * 
skipl_ceiling(const struct skip_list *sl, const void *key)
{
	struct skipl_node *current;
	int i;

	if (key == NULL)
		errmsg_exit("calls skipl_floor() with null argument.\n");

	if (SKIPL_ISEMPTY(sl))
		errmsg_exit("calls skipl_floor() with empty skip list.\n");

	current = sl->head;
	for (i = sl->level; i >= 0; i--)
		while (current->forward[i] != NULL && 
			  sl->cmp(current->forward[i]->key,	key) == 1) {
			current = current->forward[i];
		}
	
	if (current->forward[0] == NULL)
		errmsg_exit("argument to skipl_floor() is too small.\n");

	return (current->forward[0]->key);
}

/******************** static function boundary ********************/

static inline void
pointers_init(struct skipl_node **ptrs, int len)
{
	int i;
	for (i = 0; i <= len; i++)
		ptrs[i] = NULL;
}

/* 
 * Initially, we discussed a probability distribution 
 * where half of the nodes that have level i pointers 
 * also have level i+1 pointers. To get away from magic 
 * constants, we say that a fraction  p of the nodes with 
 * level i pointers also have level i+1 pointers.
 * (for our original discussion, p = 1/2).  
 * Levels are generated without reference to 
 * the number of elements in the list.
 */
static int
random_level(double p, int maxlvl)
{
	int lvl = 0;

	while (SL_FRACTION < p && lvl < maxlvl)
		lvl++;
	return lvl;
}
