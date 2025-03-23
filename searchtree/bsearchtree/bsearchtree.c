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
#include "bsearchtree.h"
#include "queue.h"

#define BST_NODE_SIZE(node)	((node) == NULL ? 0 : (node)->size)
#define BST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

static struct bst_node * make_node(const struct element *);
static struct bst_node * put_node(struct bst_node *, const struct element *);
static struct element * get_node(struct bst_node *, const char *);
static void preorder_node(const struct bst_node *,
	void (*)(const struct element *));
static void postorder_node(struct bst_node *root, void (*)(struct bst_node *));
static inline void clear_node(struct bst_node *);
static struct bst_node * min_node(struct bst_node *);
static struct bst_node * max_node(struct bst_node *);
static struct bst_node * delete_min(struct bst_node *);
static struct bst_node * delete_min2(struct bst_node *);
static struct bst_node * delete_max(struct bst_node *);
static struct bst_node * delete_node(struct bst_node *, const char *);
static unsigned long rank_node(const char *, const struct bst_node *);
static struct element * select_node(unsigned long, struct bst_node *);
static struct bst_node * floor_node(struct bst_node *, const char *);
static struct bst_node * ceiling_node(struct bst_node *, const char *);
static long leaf_nodes(const struct bst_node *);
static void keys_node(const struct bst_node *, const char *, const char *,
	struct queue *);
static int is_size_consistent(const struct bst_node *);
static int is_rank_consistent(const struct bsearch_tree *);
static int is_bst(const struct bst_node *, const char *, const char *);

/* Inserts the specified key-value pair into the BST */
void
bst_put(struct bsearch_tree *bst, const struct element *item)
{
	assert(item != NULL);
	
	bst->root = put_node(bst->root, item);
}

/* Returns the value associated with the given key. */
struct element * 
bst_get(const struct bsearch_tree *bst, const char *key)
{
	assert(key != NULL);
	
	return get_node(bst->root, key);
}

/* Prints all elements using previous order traverse */
void
bst_preorder(const struct bsearch_tree *bst, 
			void (*print)(const struct element *el))
{
	preorder_node(bst->root, print);
}

/* Releases this binary search tree */
void
bst_clear(struct bsearch_tree *bst)
{
	postorder_node(bst->root, clear_node);
}

/* Returns the smallest key is the BST */
char *
bst_min(const struct bsearch_tree *bst)
{
	return min_node(bst->root)->item.key;
}

/* Returns the largest key is the BST */
char *
bst_max(const struct bsearch_tree *bst)
{
	return max_node(bst->root)->item.key;
}

/* 
 * Removes the smallest key and associated 
 * with value from the BST.
 */
void
bst_delete_min(struct bsearch_tree *bst)
{
	if(!BST_ISEMPTY(bst))
		bst->root = delete_min(bst->root);
}

/* 
 * Removes the largest key and associated 
 * with value from the BST.
 */
void
bst_delete_max(struct bsearch_tree *bst)
{
	if(!BST_ISEMPTY(bst))
		bst->root = delete_max(bst->root);
}

/* 
 * Removes the specified key and its associated 
 * value from the BST.
 */
void
bst_delete(struct bsearch_tree *bst, const char *key)
{
	if(!BST_ISEMPTY(bst))
		bst->root = delete_node(bst->root, key);
}

/* 
 * Return the number of keys in the BST strictly 
 * less than the specified key.
 */
unsigned long 
bst_rank(const struct bsearch_tree *bst, const char *key)
{
	return rank_node(key, bst->root);
}

/* 
 * Return the key in the BST of a given rank.
 * This key has the property that there are rank keys in
 * the BST that are smaller. In other words, this key is
 * the (rank+1)st smallest key in the BST.
 */
struct element * 
bst_select(const struct bsearch_tree *bst, unsigned long rank)
{
	if(rank < BST_SIZE(bst))
		return select_node(rank, bst->root);
	return NULL;
}

/* 
 * Returns the largest key in the BST less than 
 * or equal to the specified key. 
 */
struct element * 
bst_floor(const struct bsearch_tree *bst, const char *key)
{
	struct bst_node *current;
	
	current = floor_node(bst->root, key);
	/* NULL is the specified key to small. */
	return current != NULL ? &(current->item) : NULL; 
}

/* 
 * Returns the smallest key in the BST greater than 
 * or equal to the specified key. 
 */
struct element * 
bst_ceiling(const struct bsearch_tree *bst, const char *key)
{
	struct bst_node *current;
	
	current = ceiling_node(bst->root, key);
	/* NULL is the specified key to large. */
	return current != NULL ? &(current->item) : NULL; 
}

/* Returns the number of leaf nodes */
long
bst_leaf_nodes(const struct bsearch_tree *bst)
{
	return leaf_nodes(bst->root);
}

/* Breadth-first search traverse. */
void
bst_breadth_first(const struct bsearch_tree *bst, struct queue *keys)
{
	struct queue qp;
	struct bst_node *proot;
	
	if(BST_ISEMPTY(bst))
		return;
	
	QUEUE_INIT(&qp, 0);
	proot = bst->root;
	if(proot != NULL)
		enqueue(&qp, proot);
	
	while(!QUEUE_ISEMPTY(&qp)) {
		dequeue(&qp, (void **)&proot);
		if(proot != NULL)
			enqueue(keys, proot->item.key);
		
		if(proot->left != NULL)
			enqueue(&qp, proot->left);
		if(proot->right != NULL)
			enqueue(&qp, proot->right);
	}
	
	queue_clear(&qp);
}

/* 
 * Gets all keys in the BST in the given range 
 * in ascending order.
 */
void 
bst_keys(const struct bsearch_tree *bst, const char *lokey,
		const char *hikey, struct queue *keys)
{
	keys_node(bst->root, lokey, hikey, keys);
}

/* Check integrity of BST data structure. */
int 
bst_check(const struct bsearch_tree *bst)
{
	int flag = 1;
	
	if(!is_bst(bst->root, NULL, NULL)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if(!is_size_consistent(bst->root)) {
		printf("Subtree counts not consistent.\n");
		flag = 0;
	}
	
	if(!is_rank_consistent(bst)) {
		printf("Ranks not consistent.\n");
		flag = 0;
	}
	
	return flag;
}

/******************** static function boundary ********************/

/* 
 * New create binary search tree node using 
 * the specified key-value pair. 
 */
static struct bst_node * 
make_node(const struct element *item)
{
	struct bst_node *current;
	
	current = (struct bst_node *)algmalloc(sizeof(struct bst_node));
	
	current->item = *item;
	current->size = 1;
	current->left = NULL;
	current->right = NULL;
	current->height = 0;	/* one node is 0 */
	
	return current;
}

static struct bst_node * 
put_node(struct bst_node *node, const struct element *item)
{
	int cmp;
	
	if(node == NULL)
		return make_node(item);
	
	cmp = strcmp(item->key, node->item.key);
	if(cmp < 0)
		node->left = put_node(node->left, item);
	else if(cmp > 0)
		node->right = put_node(node->right, item);
	else
		/* delete duplicate key */
		node->item.value = item->value;
	
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct element * 
get_node(struct bst_node *node, const char *key)
{
	int cmp;
	
	if(node == NULL)
		return NULL;
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		return get_node(node->left, key);
	else if(cmp > 0)
		return get_node(node->right, key);
	else
		return &(node->item);
}

static void
preorder_node(const struct bst_node *root, 
			void (*print)(const struct element *el))
{
	if(root != NULL) {
		(*print)(&(root->item));
		preorder_node(root->left, print);
		preorder_node(root->right, print);
	}
}

static void
postorder_node(struct bst_node *root, 
			void (*clear)(struct bst_node *node))
{
	if(root != NULL) {
		postorder_node(root->left, clear);
		postorder_node(root->right, clear);
		(*clear)(root);
	}
}

static inline void 
clear_node(struct bst_node *node)
{
	ALGFREE(node);
}

static struct bst_node *
min_node(struct bst_node *node)
{
	if(node->left == NULL)
		return node;
	else
		return min_node(node->left);
}

static struct bst_node *
max_node(struct bst_node *node)
{
	if(node->right == NULL)
		return node;
	else
		return max_node(node->right);
}

static struct bst_node *
delete_min(struct bst_node *node)
{
	struct bst_node *current;
	
	if(node->left == NULL) {
		current = node->right;
		clear_node(node);
		return current;
	}
	
	node->left = delete_min(node->left);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bst_node *
delete_min2(struct bst_node *node)
{
	if(node->left == NULL) {
		return node->right;
	}
	
	node->left = delete_min2(node->left);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bst_node * 
delete_max(struct bst_node *node)
{
	struct bst_node *current;
	
	if(node->right == NULL) {
		current = node->left;
		clear_node(node);
		return current;
	}
	
	node->right = delete_max(node->right);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bst_node * 
delete_node(struct bst_node *node, const char *key)
{
	int cmp;
	struct bst_node *current;
	
	if(node == NULL)	/* not found key will be deleted */
		return NULL;
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		node->left = delete_node(node->left, key);
	else if(cmp > 0)
		node->right = delete_node(node->right, key);
	else {
		if(node->left == NULL) {
			current = node->right;
			clear_node(node);
			return current;
		}
		
		if(node->right == NULL) {
			current = node->left;
			clear_node(node);
			return current;
		}
		current = node;
		node = min_node(current->right);
		node->right = delete_min2(current->right);
		node->left = current->left;
		clear_node(current);
	}
	node->size = BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right) + 1;
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

/* Number of keys in the subtree less than key. */
static unsigned long 
rank_node(const char *key, const struct bst_node *node)
{
	int cmp;
	
	if(node == NULL)
		return 0;
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		return rank_node(key, node->left);
	else if(cmp > 0)
		return rank_node(key, node->right) + BST_NODE_SIZE(node->left) + 1;
	else
		return BST_NODE_SIZE(node->left);
}

/* 
 * Return element in BST rooted at node of given rank. 
 * Precondition: rank is in legal range.
 */
static struct element * 
select_node(unsigned long rank, struct bst_node *node)
{
	unsigned long leftsize;
	
	if(node == NULL)
		return NULL;
	
	leftsize = BST_NODE_SIZE(node->left);
	if(rank < leftsize)
		return select_node(rank, node->left);
	else if(rank > leftsize)
		return select_node(rank - leftsize - 1,	node->right);
	else
		return &(node->item);
}

static struct bst_node * 
floor_node(struct bst_node *node, const char *key)
{
	int cmp;
	struct bst_node *rnode;
	
	if(node == NULL) /* not found to the specified key */
		return NULL;
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		return floor_node(node->left, key);
	else if(cmp == 0)
		return node;
	else {
		rnode = floor_node(node->right, key);
		if(rnode != NULL)
			return rnode;
		else
			return node;
	}
}

static struct bst_node * 
ceiling_node(struct bst_node *node, const char *key)
{
	int cmp;
	struct bst_node *lnode;
	
	if(node == NULL)
		return NULL;
	
	cmp = strcmp(key, node->item.key);
	if(cmp == 0)
		return node;
	else if(cmp > 0)
		return ceiling_node(node->right, key);
	else {
		lnode = ceiling_node(node->left, key);
		if(lnode != NULL)
			return lnode;
		else
			return node;
	}
}

static long
leaf_nodes(const struct bst_node *node)
{
	if(node == NULL)
		return 0;
	else if(node->left == NULL && node->right == NULL)
		return 1;
	else
		return leaf_nodes(node->left) + leaf_nodes(node->right);
}

static void
keys_node(const struct bst_node *node, const char *lokey, const char *hikey,
		struct queue *qp)
{
	int cmplo, cmphi;
	
	if(node == NULL)
		return;
	
	cmplo = strcmp(lokey, node->item.key);
	cmphi = strcmp(hikey, node->item.key);
	
	if(cmplo < 0)
		keys_node(node->left, lokey, hikey, qp);
	if(cmplo <= 0 && cmphi >= 0)
		enqueue(qp, node->item.key);
	if(cmphi > 0)
		keys_node(node->right, lokey, hikey, qp);
}

/* are the size fields correct? */
static int 
is_size_consistent(const struct bst_node *node)
{
	if(node == NULL)
		return 1;		/* treat as empty constraint */
	if(node->size != BST_NODE_SIZE(node->left) + 
	   BST_NODE_SIZE(node->right) + 1)
		return 0;
	return is_size_consistent(node->left) && is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int
is_rank_consistent(const struct bsearch_tree *bst)
{
	unsigned long i;
	char *key;
	struct queue qu;
	struct element *el;
	
	for(i = 0; i < BST_SIZE(bst); i++) {
		el = bst_select(bst, i);
		if(i != bst_rank(bst, el->key))
			return 0;
	}
	
	QUEUE_INIT(&qu, 0);
	bst_keys(bst, bst_min(bst), bst_max(bst), &qu);
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&key);
		el = bst_select(bst, bst_rank(bst, key));
		if(strcmp(key, el->key) != 0)
			return 0;
	}
	
	queue_clear(&qu);

	return 1;
}

/* 
 * Is the tree rooted at Node a BST with all keys strictly
 * between min and max (if min or max is null, treat as 
 * empty constraint).
 */
static int
is_bst(const struct bst_node *node, const char *minkey,	const char *maxkey)
{
	if(node == NULL)
		return 1;
	
	if(minkey != NULL &&
		strcmp(node->item.key, minkey) <= 0)
		return 0;
	if(maxkey != NULL &&
		strcmp(node->item.key, maxkey) >= 0)
		return 0;
	return is_bst(node->left, minkey, node->item.key) && 
		is_bst(node->right, node->item.key, maxkey);
}
