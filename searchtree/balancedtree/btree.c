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
#include "btree.h"
#include "singlelist.h"

#define BT_FREE_NDOE	1

static inline void children_init(struct btree_node *);
static struct btree_node * split(struct btree_node *);
static struct btree_node * insert(struct btree *, struct btree_node *,
	const void *, const void *, int);
static void search(const struct btree *, struct btree_node *, const void *,
	int, void **);
static struct btree_node * get_start_leaf(const struct btree *, 
	struct btree_node *, const void *, int);
static struct btree_node * get_leaf_node(const struct btree *,
	struct btree_node *, const void *, int);

#if 0
static struct btree_node * get_internal_node(const struct btree *,
	struct btree_node *, const void *, int);
static inline void union_left_entries(struct btree_node *, struct btree_node *);
#endif

static inline unsigned int get_entry_index(const struct btree *,
	struct btree_node *, const void *);
static void query(const struct btree *, const void *, const void *,
	struct single_list *);
static void release(struct btree_node *, int);
static void remove_leaf_entry(const struct btree *, struct btree_node *,
	const void *);
static void remove_internal_key(const struct btree *, struct btree_node *,
	const void *);
static inline void move_right_entries(struct btree_node *);
static inline void move_left_entries(struct btree_node *);
static void borrow_left_leaf(struct btree_node *);
static void borrow_left_key(unsigned int, struct btree_node *);
static void borrow_right_leaf(struct btree_node *);
static void borrow_right_key(unsigned int, struct btree_node *);
static inline void union_right_entries(struct btree_node *,
	struct btree_node *);
static struct btree_node * merge_left_leaf(struct btree_node *,
	struct btree_node *);
static struct btree_node * merge_left_internal(unsigned int,
	struct btree_node *, struct btree_node *);
static struct btree_node * merge_right_leaf(struct btree_node *,
	struct btree_node *);
static struct btree_node * merge_right_internal(unsigned int,
	struct btree_node *, struct btree_node *);
static void remove_entry(struct btree *, struct btree_node *, const void *,
	bool isleaf);

/* Initializes an empty B-tree. */
void
btree_init(struct btree *bt, unsigned int ksz, unsigned int vsz,
			algcomp_ft *cmp)
{
	assert(ksz != 0 && vsz != 0);

	/* root is a specific case */
	bt->root = (struct btree_node *)algmalloc(sizeof(struct btree_node));
	bt->root->sz = 0;
	bt->root->prev = NULL;
	bt->root->sibling = NULL;
	bt->root->parent = NULL;
	children_init(bt->root);

	bt->size = 0;
	bt->height = 0;
	bt->keysize = ksz;
	bt->valsize = vsz;
	bt->kcmp = cmp;
}

/* 
 * Inserts the key-value pair into the B-Tree, 
 * overwriting the old value with the new value 
 * if the key is already in the B-Tree. 
 */
void 
btree_put(struct btree *bt, const void *key, const void *val)
{
	struct btree_node *u, *t;
	unsigned int i;

	if(key == NULL)
		errmsg_exit("Argumment key to btree_put is null.\n");

	u = insert(bt, bt->root, key, val, bt->height);
	bt->size++;
	if(u == NULL)
		return;

	/* when overflowing a root, need to split root */
	t = (struct btree_node *)algmalloc(sizeof(struct btree_node));	
	t->sz = 2;
	t->prev = NULL;
	t->sibling = NULL;
	t->parent = NULL;
	children_init(t);

	for(i = 0; i < 2; i++) {
		t->children[i] = (struct btree_entry *)
			algmalloc(sizeof(struct btree_entry));
		t->children[i]->key = NULL;
		t->children[i]->value = NULL;
	}
	/* left node with smaller items */
	t->children[0]->key = bt->root->children[0]->key;
	t->children[0]->next = bt->root;
	bt->root->parent = t;

	/* 
	 * right node with larger items,
	 * move the smallest key in new 
	 * right node to the root. 
	 */
	t->children[1]->key = u->children[0]->key;
	t->children[1]->next = u;
	u->parent = t;

	/* now, node t beomce new root */
	bt->root = t;
	bt->root->parent = NULL;
	bt->height++;
}

/* Returns the value associated with the given key. */
void 
btree_get(const struct btree *bt, const void *key, void **val)
{
	if(key == NULL)
		errmsg_exit("Argumment key to btree_get is null.\n");

	search(bt, bt->root, key, bt->height, val);
}

/* Clears this B-Tree */
void
btree_clear(struct btree *bt)
{
	if(BTREE_ISEMPTY(bt))
		return;

	release(bt->root, bt->height);
	ALGFREE(bt->root);
	bt->size = 0;
	bt->height = 0;
	bt->keysize = 0;
	bt->valsize = 0;
	bt->kcmp = NULL;
}

/* Output query result that set between lokey and hikey */
void 
btree_range_query(const struct btree *bt, const void *lokey, const void *hikey,
				struct single_list *rec)
{
	if(lokey == NULL || hikey == NULL)
		errmsg_exit("Argumment key to btree_range_query is null.\n");

	slist_init(rec, 0, NULL);

	if(BTREE_ISEMPTY(bt))
		return;

	query(bt, lokey, hikey, rec);
}

/* Deletes a key-value pair from this B-Tree */
int
btree_delete(struct btree *bt, const void *key)
{
	struct btree_node *leaf;

	if((leaf = get_leaf_node(bt, bt->root, key, 
		bt->height)) == NULL) {
		return 0;
	}

	remove_entry(bt, leaf, key, true);
	bt->size--;

	return 1;
}

/* Returns the first key in this B-Tree. */
void *
btree_first_key(const struct btree *bt)
{
	struct btree_node *first;

	if(BTREE_ISEMPTY(bt))
		return NULL;

	if((first = bt->root->children[0]->next) == NULL)
		return bt->root->children[0]->key;

	while(first->children[0]->next != NULL)
		first = first->children[0]->next;
	
	return first->children[0]->key;
}

/* Returns the last key in this B-Tree. */
void *
btree_last_key(const struct btree *bt)
{
	struct btree_node *last;

	if(BTREE_ISEMPTY(bt))
		return NULL;

	if((last = bt->root->children[bt->root->sz - 1]->next) == NULL)
		return bt->root->children[bt->root->sz - 1]->key;

	while(last->children[last->sz - 1]->next != NULL)
		last = last->children[last->sz - 1]->next;

	return last->children[last->sz - 1]->key;
}

/******************** static function boundary ********************/

/* initializes an children of node */
static inline void 
children_init(struct btree_node *h)
{
	unsigned int i;
	for(i = 0; i < MAX_CHILDREN; i++)
		h->children[i] = NULL;
}

/* split node in half */
static struct btree_node * 
split(struct btree_node *h)
{
	struct btree_node *t;
	int i;

	t = (struct btree_node *)algmalloc(sizeof(struct btree_node));
	t->prev = NULL;
	t->sibling = NULL;
	t->parent = NULL;
	children_init(t);
	t->sz = MAX_CHILDREN / 2;
	h->sz = MAX_CHILDREN / 2;

	/*
	 * Original node with smaller items,
	 * New node with larger items.
	 */
	for(i = 0; i < MAX_CHILDREN / 2; i++) {
		t->children[i] = h->children[MAX_CHILDREN / 2 + i];
		if(h->children[MAX_CHILDREN / 2 + i]->next != NULL)
			h->children[MAX_CHILDREN / 2 + i]->next->parent = t;
		h->children[MAX_CHILDREN / 2 + i] = NULL;
	}

	t->parent = h->parent;
	
	t->sibling = h->sibling;
	if(h->sibling != NULL)
		h->sibling->prev = t;
	h->sibling = t;
	t->prev = h;

	return t;
}

/* 
 * Inserts the key-value pair into 
 * the subtree rooted at H.
 */
static struct btree_node *
insert(struct btree *bt, struct btree_node *h, const void *key,
		const void *val, int ht)
{
	unsigned int i, j;
	struct btree_entry *t = NULL;
	struct btree_node *u = NULL;

	if(ht == 0) { /* external node */
		t = (struct btree_entry *)algmalloc(sizeof(struct btree_entry));
		t->key = algmalloc(bt->keysize);
		t->value = algmalloc(bt->valsize);
		t->next = NULL;
		memmove(t->key, key, bt->keysize);
		memmove(t->value, val, bt->valsize);

		for(i = 0; i < h->sz; i++)
			if(bt->kcmp(key, h->children[i]->key) == 1)
				break;
	}
	else { /* internal node */
		for(i = 0; i < h->sz; i++)
			if(i + 1 == h->sz || (i + 1 < h->sz && 
				bt->kcmp(key, h->children[i + 1]->key) == 1)) {
				if((u = insert(bt, h->children[i++]->next, key, val, ht - 1))
					== NULL) {
					return NULL;
				}

				/* 
				 * if a node had splited, attach the new node to its parent.
				 */
				t = (struct btree_entry *)algmalloc(sizeof(struct btree_entry));

				/* 
				 * move the smallest key to parent in sorted order.
				 */
				t->key = u->children[0]->key;
				t->value = NULL;
				t->next = u;

				break;
			}
	}

	/* 
	 * overwriting the old value with the new value 
	 * if the B-Tree already contains the specified key. 
	 */
	if(i != 0 && h->children[i - 1] != NULL &&
	   h->children[i - 1]->next == NULL && 
	   bt->kcmp(key, h->children[i - 1]->key) == 0) {
		memcpy(h->children[i - 1]->value, val, bt->valsize);
		ALGFREE(t);
		bt->size--;
		return NULL;
	}
	else { 
		/* 
		 * move the elements of children array and inserts a new entry.
		 */
		for(j = h->sz; j > i; j--)
			h->children[j] = h->children[j - 1];
		h->children[i] = t;
		h->sz++;

		if(u != NULL)
			u->parent = h;
	}
	
	if(h->sz < MAX_CHILDREN)
		return NULL;
	else
		return split(h);
}

/* 
 * Returns the value associated with the given key from 
 * the subtree rooted at X.
 */
static void 
search(const struct btree *bt, struct btree_node *x,
	const void *key, int ht, void **val)
{
	unsigned int i;

	if(ht == 0) {
		for(i = 0; i < x->sz; i++)
			if(bt->kcmp(key, x->children[i]->key) == 0) {
				memmove(*val, x->children[i]->value, bt->valsize);
				return; 
			}
	}
	else {
		for(i = 0; i < x->sz; i++)
			if(i + 1 == x->sz || (i + 1 < x->sz && 
			   bt->kcmp(key, x->children[i + 1]->key) == 1)) {
				search(bt, x->children[i]->next, key, ht - 1, val);
			}
	}
}

/* returns a starting leaf node in the leaf linked list */
static struct btree_node * 
get_start_leaf(const struct btree *bt, struct btree_node *x,
			const void *key, int ht)
{
	unsigned int i;

	if(x == NULL)
		return NULL;

	if(ht == 0) {
		for(i = 0; i < x->sz; i++)
			if(bt->kcmp(key, x->children[i]->key) == 0 || 
			   bt->kcmp(key, x->children[i]->key) == 1) {
				return x;
			}

		if(x->sibling == NULL)
			return NULL;
		else 
			return get_start_leaf(bt, x->sibling, key, 0);
	}
	else 
		for(i = 0; i < x->sz; i++)
			if(i + 1 == x->sz || (i + 1 < x->sz && 
			   bt->kcmp(key, x->children[i + 1]->key) == 1)) {
				return get_start_leaf(bt, x->children[i]->next, key, ht - 1);
			}

	return NULL;
}

/* returns the leaf node in this B-Tree. */
static struct btree_node * 
get_leaf_node(const struct btree *bt, struct btree_node *x,
			const void *key, int ht)
{
	unsigned int i;

	if(x == NULL)
		return NULL;

#if BT_FREE_NDOE == 1
	if(x->sz >= MAX_CHILDREN) {
		printf("--------!!! Warning 0#: This is a bug.\n");
		return NULL;
	}
#endif

	if(ht == 0) {
		for(i = 0; i < x->sz; i++)
			if(bt->kcmp(key, x->children[i]->key) == 0) 
				return x;
	}
	else 
		for(i = 0; i < x->sz; i++)
			if(i + 1 == x->sz || ( i + 1 < x->sz && 
			   bt->kcmp(key, x->children[i + 1]->key) == 1)) {
				return get_leaf_node(bt, x->children[i]->next, key, ht - 1);
			}

	return NULL;
}

#if 0
/* returns the internal node in this B-Tree. */
static struct btree_node * 
get_internal_node(const struct btree *bt, struct btree_node *x,
				const void *key, int ht)
{
	unsigned int i;

	if(x == NULL)
		return NULL;

	if(ht == 1) {
		for(i = 0; i < x->sz; i++)
			if(bt->kcmp(key, x->children[i]->key) == 0)
				return x;
	}
	else 
		for(i = 0; i < x->sz; i++)
			if(i + 1 == x->sz || (i + 1 < x->sz && 
			   bt->kcmp(key, x->children[i + 1]->key) == 1)) {
				if(i < x->sz && bt->kcmp(key, x->children[i]->key) == 0)
					return x;
				return get_internal_node(bt, x->children[i]->next, key, ht - 1);
			}

	return NULL;
}
#endif

/* returns an entry of index in this node */
static inline unsigned int 
get_entry_index(const struct btree *bt, struct btree_node *x, const void *key)
{
	unsigned int i;

	for(i = 0; i < x->sz; i++)
		if(bt->kcmp(key, x->children[i]->key) == 0)
			return i;
	return MAX_CHILDREN;
}

/* Output query result that set between lokey and hikey */
static void
query(const struct btree *bt, const void *lokey, const void *hikey,
	struct single_list *rec)
{
	unsigned int i;
	struct btree_node *leaf;
	bool ended = false;
	
	leaf = get_start_leaf(bt, bt->root, lokey, bt->height);
	while(leaf != NULL) {
		for(i = 0; i < leaf->sz; i++) {
			if(bt->kcmp(lokey, leaf->children[i]->key) == 1 || 
			   bt->kcmp(lokey, leaf->children[i]->key) == 0) {
				if(bt->kcmp(hikey, leaf->children[i]->key) == -1 || 
				   bt->kcmp(hikey, leaf->children[i]->key) == 0) {
					slist_append(rec, leaf->children[i]);
				}
				else {
					ended = true;
					break;
				}
			}
		}

		if(ended)
			break;

		leaf = leaf->sibling;
	}
}

/* Release the subtrees rooted at NODE */
static void 
release(struct btree_node *node, int ht)
{
	unsigned int i;

	if(node == NULL)
		return;

#if BT_FREE_NDOE == 1
	if(node->sz >= MAX_CHILDREN) {
		printf("--------!!! Warning 4#: This is a bug.\n");
		return;
	}
#endif

	if(ht == 0) 
		for(i = 0; i < node->sz; i++) {
			ALGFREE(node->children[i]->key);
			ALGFREE(node->children[i]->value);
			ALGFREE(node->children[i]);
		}
	else
		for(i = 0; i < node->sz; i++) {
			release(node->children[i]->next, ht - 1);
			ALGFREE(node->children[i]);
		}
}

/* remove a entry from the leaf node of the entry */
static void 
remove_leaf_entry(const struct btree *bt, struct btree_node *x,
				 const void *key)
{
	unsigned int i, j;
	void *oldkey = NULL;

	for(i = 0; i < x->sz; i++)
		if(bt->kcmp(key, x->children[i]->key) == 0) {
			oldkey = x->children[i]->key;
			ALGFREE(x->children[i]->value);
			ALGFREE(x->children[i]);

			/* eliminates empty the location of index */
			for(j = i; j < x->sz; j++)
				x->children[j] = x->children[j + 1];
			x->sz--;

			break;
		}

	/* if the first of the key of x not in its parent */
	if(x->parent == NULL || i != 0) 
		ALGFREE(oldkey);
}

/* remmove a key from the internal node */
static void 
remove_internal_key(const struct btree *bt, struct btree_node *x,
					const void *key)
{
	unsigned int i;
	struct btree_node *ml;

	if(x == NULL)
		return;

#if BT_FREE_NDOE == 1
	if(x->sz >= MAX_CHILDREN) {
		printf("--------!!! Warning 1#: This is a bug.\n");
		return;
	}
#endif

	if((i = get_entry_index(bt, x, key)) != MAX_CHILDREN) {
		ml = x->children[i]->next;
		while(ml != NULL && ml->children[0] != NULL && 
			  ml->children[0]->next != NULL) {
			ml = ml->children[0]->next;
		}

		if(ml == NULL || ml->children[0] == NULL)
			return;
		
		if(i != 0 || x->parent == NULL)
			ALGFREE(x->children[i]->key);
		x->children[i]->key = ml->children[0]->key;
	}
}

static inline void 
move_right_entries(struct btree_node *h)
{
	unsigned int i, k;

	k = h->prev->sz - 1;	/* last entry */
	for(i = h->sz; i > 0; i--)
		h->children[i] = h->children[i - 1];
	h->children[0] = h->prev->children[k];
	h->sz++;

	if(h->prev->children[k]->next != NULL)
		h->prev->children[i]->next->parent = h;

	h->prev->children[k] = NULL;
	h->prev->sz--;
}

static inline void 
move_left_entries(struct btree_node *h)
{
	unsigned int i;

	h->children[h->sz] = h->sibling->children[0];
	if(h->sibling->children[0]->next != NULL)
		h->sibling->children[0]->next->parent = h;
	h->sibling->children[0] = NULL;
	for(i = 0; i < h->sibling->sz; i++) {
		h->sibling->children[i] = 
		h->sibling->children[i + 1];
	}
	h->sz++;
	h->sibling->sz--;
}

/* borrow a left leaf entry to current leaf node */
static void
borrow_left_leaf(struct btree_node *h)
{
	unsigned int i;

	assert(h->prev != NULL && h->parent != NULL);
	assert(h->prev->parent == h->parent);

	move_right_entries(h);

	for(i = 0; i < h->parent->sz; i++)
		if(h->parent->children[i]->next == h) {
			h->parent->children[i]->key = 
			h->children[0]->key;
			break;
		}
}

/* 
 * Borrrow a left internal key to the current internal node.
 */
static void
borrow_left_key(unsigned int pos, struct btree_node *h)
{
	assert(h->prev != NULL && h->parent != NULL);
	assert(h->prev->parent == h->parent);

	move_right_entries(h);
	h->parent->children[pos]->key = h->children[0]->key;
}

/* 
 * Borrow a right leaf entry to the current leaf node.
 */
static void
borrow_right_leaf(struct btree_node *h)
{
	unsigned int i;

	assert(h->sibling != NULL && h->parent != NULL);
	assert(h->sibling->parent == h->parent);

	move_left_entries(h);
	for(i = 0; i < h->parent->sz; i++)
		if(h->parent->children[i]->next == h->sibling) {
			h->parent->children[i]->key = 
			h->sibling->children[0]->key;
			break;
		}
}

/* borrow a right interanl key to the current right node */
static void
borrow_right_key(unsigned int pos, struct btree_node *h)
{
	assert(h->sibling != NULL && h->parent != NULL);
	assert(h->sibling->parent == h->parent);

	move_left_entries(h);
	h->parent->children[pos]->key = 
	h->sibling->children[0]->key;
}

#if 0
static inline void 
union_left_entries(struct btree_node *current, 
					struct btree_node *left)
{
	unsigned int i, j;
	
	for(i = 0; i < left->sz; i++) {
		for(j = current->sz; j > i; j--)
			current->children[j] = current->children[j - 1];
		current->sz++;
	}

	for(j = 0; j < left->sz; j++) {
		current->children[j] = left->children[j];
		if(left->children[j]->next != NULL)
			left->children[j]->next->parent = current;
		left->children[j] = NULL;
	}
	left->sz = 0;
}
#endif

static inline void 
union_right_entries(struct btree_node *current, struct btree_node *right)
{
	unsigned int i, j;

	/* combines two children of nodes become one */
	for(i = current->sz, j = 0; j < right->sz; i++, j++) {
		current->children[i] = right->children[j];
		current->sz++;
		if(right->children[j]->next != NULL)
			right->children[j]->next->parent = current;
		right->children[j] = NULL;
	}
	right->sz = 0;
}

/* merge left leaf node to the current node. */
static struct btree_node * 
merge_left_leaf(struct btree_node *current, struct btree_node *left)
{
	unsigned int i, j;
	bool flag = false;

	assert(current->sz + left->sz < MAX_CHILDREN);
	assert(current->parent == left->parent);

	left->sibling = current->sibling;
	if(current->sibling != NULL)
		current->sibling->prev = left;
	
	union_right_entries(left, current);

	for(i = 0; i < left->parent->sz; i++)
		if(left->parent->children[i]->next == current) {
			flag = true;
			ALGFREE(left->parent->children[i]);
			for(j = i; j < left->parent->sz; j++) {
				left->parent->children[j] = 
				left->parent->children[j + 1];
			}
			left->parent->sz--;
			break;
		}
	
	if(flag)
		ALGFREE(current);

	return left;
}

/* merge left internal node to the current node. */
static struct btree_node * 
merge_left_internal(unsigned int pos, struct btree_node *current,
					struct btree_node *left)
{
	unsigned int i;

	assert(current->sz + left->sz < MAX_CHILDREN);

	left->sibling = current->sibling;
	if(current->sibling != NULL)
		current->sibling->prev = left;

	union_right_entries(left, current);

	ALGFREE(current->parent->children[pos]);
	for(i = pos; i < current->parent->sz; i++) {
		current->parent->children[i] = 
		current->parent->children[i + 1];
	}
	current->parent->sz--;

#if BT_FREE_NDOE == 1
	/* Warning: this a bug. */
	ALGFREE(current); 
#endif
	return left;
}

/* merge right leaf node to the current node. */
static struct btree_node * 
merge_right_leaf(struct btree_node *current, struct btree_node *right)
{
	unsigned int i, j, flag = 0;

	assert(current->sz + right->sz < MAX_CHILDREN);
	assert(current->parent == right->parent);

	/* adjust linked-list of pointer*/
	current->sibling = right->sibling;
	if(right->sibling != NULL)
		right->sibling->prev = current;

	union_right_entries(current, right);

	/* adjust its parent of key */
	for(i = 0; i < right->parent->sz; i++)
		if(right->parent->children[i]->next == right) {
			flag = 1;
			ALGFREE(right->parent->children[i]);
			for(j = i; j < right->parent->sz; j++) {
				right->parent->children[j] = 
				right->parent->children[j + 1];
			}
			right->parent->sz--;
			break;
		}

	if(flag)
		ALGFREE(right);

	return current;
}

/* merge right internal node to the current node. */
static struct btree_node * 
merge_right_internal(unsigned int pos, struct btree_node *current,
					struct btree_node *right)
{
	unsigned int i;

	assert(current->sz + right->sz < MAX_CHILDREN);

	/* adjust linked-list of pointer*/
	current->sibling = right->sibling;
	if(right->sibling != NULL)
		right->sibling->prev = current;

	union_right_entries(current, right);

	free(right->parent->children[pos + 1]);
	for(i = pos + 1; i < right->parent->sz; i++) {
		right->parent->children[i] = 
		right->parent->children[i + 1];
	}
	right->parent->sz--;

#if BT_FREE_NODE == 1
	/* Warning: this a bug. */
	ALGFREE(right);
#endif
	return current;
}

static void 
remove_entry(struct btree *bt, struct btree_node *x, const void *key,
			bool isleaf)
{
	const unsigned int minsz = MAX_CHILDREN / 2;
	struct btree_node *left, *right, *oldroot;
	int pos = -1, i;

	if(x == NULL)
		return;

#if BT_FREE_NDOE == 1
	if(x->sz >= MAX_CHILDREN) {
		printf("--------!!! Warning 3#: This is a bug.\n");
		return;
	}
#endif

	if(isleaf)
		remove_leaf_entry(bt, x, key);
	else 
		remove_internal_key(bt, x, key);

	if(x->sz < minsz) {
		if(x == bt->root && !isleaf) {
			if(bt->root->sz == 1 && 
				bt->root->children[0]->next != NULL) {
				bt->root->children[0]->next->parent = NULL;
				oldroot = bt->root;
				bt->root = bt->root->children[0]->next;
				bt->root->parent = NULL;
				bt->height--;
				ALGFREE(oldroot->children[0]);
				ALGFREE(oldroot);
			}
			return;
		}
		else if(isleaf) {
			left = x->prev;
			right = x->sibling;

			if(left != NULL && left->parent == x->parent && left->sz > minsz)
				borrow_left_leaf(x);
			else if(right != NULL && right->parent == x->parent &&
				right->sz > minsz) {
				borrow_right_leaf(x);
			}
			else if(left != NULL && left->parent == x->parent && 
					left->sz <= minsz) {
				x = merge_left_leaf(x, left);
			}
			else if(right != NULL && right->parent == x->parent && 
					right->sz <= minsz) {
				x = merge_right_leaf(x, right);
			}
			else	/* Nothing do it. */
				while(0);
		}
		else {
			if(x->parent == NULL)
				return;

			if(x->parent->sz >= MAX_CHILDREN) {
				printf("~Warning 3#: This is a bug.\n");
				return;
			}

			for(i = 0; i < (int)x->parent->sz; i++) {
				if(x->parent->children[i]->next == x) {
					pos = i;
					break;
				}
			}

			if(pos == -1)
				return;

			if((int)x->parent->sz > pos + 1)
				right = x->parent->children[pos + 1]->next;
			else
				right = NULL;

			if(pos > 0)
				left = x->parent->children[pos - 1]->next;
			else
				left = NULL;

			if(left != NULL && left->parent == x->parent && left->sz > minsz)
				borrow_left_key(pos, x);
			else if(right != NULL && right->parent == x->parent && 
					right->sz > minsz) {
				borrow_right_key(pos, x);
			}
			else if(left != NULL && left->parent == x->parent && 
					left->sz <= minsz) {
				x = merge_left_internal(pos, x, left);
			}
			else if(right != NULL && right->parent == x->parent && 
					right->sz <= minsz) {
				x = merge_right_internal(pos, x, right);
			}
			else	/* Nothing do it. */
				while(0);
		}
	}

	if(x->parent != NULL)
		remove_entry(bt, x->parent, key, false);
}
