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
#include "linearlist.h"

#define BST_NODE_SIZE(node)	((node) == NULL ? 0 : (node)->size)
#define BST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

static struct bstree_node * make_node(const void *, unsigned int);
static struct bstree_node * put_node(const struct bstree *,
	struct bstree_node *, const void *);
static void * get_node(struct bstree_node *, const void *, algcomp_ft *);
static void preorder_node(const struct bstree_node *, struct single_list *);
static void release_subtree(struct bstree_node *root, unsigned int);
static struct bstree_node * min_node(struct bstree_node *);
static struct bstree_node * max_node(struct bstree_node *);
static struct bstree_node * delete_min(struct bstree_node *, unsigned int);
static struct bstree_node * delete_min2(struct bstree_node *);
static struct bstree_node * delete_max(struct bstree_node *, unsigned int);
static struct bstree_node * delete_node(const struct bstree *,
	struct bstree_node *, const void *);
static unsigned long rank_node(const struct bstree_node *, const void *,
	algcomp_ft *);
static void * select_node(struct bstree_node *, unsigned long);
static struct bstree_node * floor_node(struct bstree_node *, const void *,
	algcomp_ft *);
static struct bstree_node * ceiling_node(struct bstree_node *, const void *,
	algcomp_ft *);
static long leaf_nodes(const struct bstree_node *);
static void keys_node(const struct bstree_node *, const void *, const void *,
	algcomp_ft *, struct single_list *);
static int is_size_consistent(const struct bstree_node *);
static int is_rank_consistent(const struct bstree *);
static int is_bst(const struct bstree_node *, const void *, const void *,
	algcomp_ft *);

/* Initializes an empty binary search tree */
void
bst_init(struct bstree *bst, unsigned int ksize, algcomp_ft *kcmp)
{
	bst->root = NULL;
	bst->keysize = ksize;
	bst->cmp = kcmp;
}

/* Inserts the specified key into the BST */
int
bst_put(struct bstree *bst, const void *key)
{
	if (key == NULL)
		return -1;
	bst->root = put_node(bst, bst->root, key);
	return 0;
}

/* Returns the Key with the given key. */
void * 
bst_get(const struct bstree *bst, const void *key)
{
	if (key == NULL)
		return NULL;
	
	return get_node(bst->root, key, bst->cmp);
}

/* Prints all elements using previous order traverse */
void
bst_preorder(const struct bstree *bst, struct single_list *keys)
{
	slist_init(keys, 0, bst->cmp);
	preorder_node(bst->root, keys);
}

/* Releases this binary search tree */
void
bst_clear(struct bstree *bst)
{
	release_subtree(bst->root, bst->keysize);
}

/* Returns the smallest key in the BST */
void *
bst_min(const struct bstree *bst)
{
	return (min_node(bst->root)->key);
}

/* Returns the largest key in the BST */
void *
bst_max(const struct bstree *bst)
{
	return (max_node(bst->root)->key);
}

/* Removes the smallest key from the BST. */
int
bst_delete_min(struct bstree *bst)
{
	if (BST_ISEMPTY(bst))
		return -1;
	bst->root = delete_min(bst->root, bst->keysize);
	return 0;
}

/* Removes the largest key from the BST. */
int
bst_delete_max(struct bstree *bst)
{
	if (BST_ISEMPTY(bst))
		return -1;
	bst->root = delete_max(bst->root, bst->keysize);
	return 0;
}

/* Removes the specified key from the BST. */
int
bst_delete(struct bstree *bst, const void *key)
{
	if (BST_ISEMPTY(bst))
		return -1;
	bst->root = delete_node(bst, bst->root, key);
	return 0;
}

/* 
 * Return the number of keys in the BST strictly 
 * less than the specified key.
 */
unsigned long 
bst_rank(const struct bstree *bst, const void *key)
{
	return rank_node(bst->root, key, bst->cmp);
}

/* 
 * Return the key in the BST of a given rank.
 * This key has the property that there are rank keys in the BST that are
 * smaller. In other words, this key is the (rank+1)st smallest key in the BST.
 */
void * 
bst_select(const struct bstree *bst, unsigned long rank)
{
	if (rank < BST_SIZE(bst))
		return select_node(bst->root, rank);
	return NULL;
}

/* 
 * Returns the largest key in the BST less than 
 * or equal to the specified key. 
 */
void * 
bst_floor(const struct bstree *bst, const void *key)
{
	struct bstree_node *current;
	
	current = floor_node(bst->root, key, bst->cmp);
	/* NULL is the specified key to small. */
	return current != NULL ? current->key : NULL; 
}

/* 
 * Returns the smallest key in the BST greater than 
 * or equal to the specified key. 
 */
void * 
bst_ceiling(const struct bstree *bst, const void *key)
{
	struct bstree_node *current;
	
	current = ceiling_node(bst->root, key, bst->cmp);
	/* NULL is the specified key to large. */
	return current != NULL ? current->key: NULL; 
}

/* Returns the number of leaf nodes */
long
bst_leaf_nodes(const struct bstree *bst)
{
	return leaf_nodes(bst->root);
}

/* Breadth-first search traverse. */
void
bst_breadth_first(const struct bstree *bst, struct single_list *keys)
{
	struct queue qp;
	struct bstree_node *proot;
	
	if (BST_ISEMPTY(bst))
		return;
	
	QUEUE_INIT(&qp, 0);
	proot = bst->root;
	if (proot != NULL)
		enqueue(&qp, proot);
	
	while (!QUEUE_ISEMPTY(&qp)) {
		dequeue(&qp, (void **)&proot);
		if (proot != NULL)
			slist_append(keys, proot->key);
		
		if (proot->left != NULL)
			enqueue(&qp, proot->left);
		if (proot->right != NULL)
			enqueue(&qp, proot->right);
	}
	queue_clear(&qp);
}

/* Gets all keys in the BST in the given range in ascending order. */
void 
bst_keys(const struct bstree *bst, const void *lokey, const void *hikey,
	struct single_list *keys)
{
	slist_init(keys, 0, bst->cmp);
	keys_node(bst->root, lokey, hikey, bst->cmp, keys);
}

/* Check integrity of BST data structure. */
int 
bst_check(const struct bstree *bst)
{
	int flag = 1;

	if (!is_bst(bst->root, NULL, NULL, bst->cmp)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if (!is_size_consistent(bst->root)) {
		printf("Subtree counts not consistent.\n");
		flag = 0;
	}
	
	if (!is_rank_consistent(bst)) {
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
static struct bstree_node * 
make_node(const void *key, unsigned int ksize)
{
	struct bstree_node *current;
	
	current = (struct bstree_node *)algmalloc(sizeof(struct bstree_node));
	
	if (ksize == 0)
		current->key = (void *)key;
	else {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
	current->size = 1;
	current->left = NULL;
	current->right = NULL;
	current->height = 0;	/* one node is 0 */
	
	return current;
}

static struct bstree_node * 
put_node(const struct bstree *bst, struct bstree_node *node, const void *key)
{
	int cr;
	
	if (node == NULL)
		return make_node(key, bst->keysize);
	
	cr = bst->cmp(key, node->key);
	if (cr == 1)
		node->left = put_node(bst, node->left, key);
	else if (cr == -1)
		node->right = put_node(bst, node->right, key);
	else
		return node;
	
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left),
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static void * 
get_node(struct bstree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	
	if (node == NULL)
		return NULL;
	
	cr = kcmp(key, node->key);
	if (cr == 1)
		return get_node(node->left, key, kcmp);
	else if (cr == -1)
		return get_node(node->right, key, kcmp);
	else
		return (node->key);
}

static void
preorder_node(const struct bstree_node *root, struct single_list *keys)
{
	if (root != NULL) {
		slist_append(keys, root->key);
		preorder_node(root->left, keys);
		preorder_node(root->right, keys);
	}
}

static void
release_subtree(struct bstree_node *root, unsigned int ksize)
{
	if (root != NULL) {
		release_subtree(root->left, ksize);
		release_subtree(root->right, ksize);
		if (ksize != 0)
			ALGFREE(root->key);
		ALGFREE(root);
	}
}

static struct bstree_node *
min_node(struct bstree_node *node)
{
	if (node->left == NULL)
		return node;
	else
		return min_node(node->left);
}

static struct bstree_node *
max_node(struct bstree_node *node)
{
	if (node->right == NULL)
		return node;
	else
		return max_node(node->right);
}

static struct bstree_node *
delete_min(struct bstree_node *node, unsigned int ksize)
{
	struct bstree_node *current;
	
	if (node->left == NULL) {
		current = node->right;
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return current;
	}
	
	node->left = delete_min(node->left, ksize);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bstree_node *
delete_min2(struct bstree_node *node)
{
	if (node->left == NULL)
		return node->right;
	
	node->left = delete_min2(node->left);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bstree_node * 
delete_max(struct bstree_node *node, unsigned int ksize)
{
	struct bstree_node *current;
	
	if (node->right == NULL) {
		current = node->left;
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return current;
	}
	
	node->right = delete_max(node->right, ksize);
	node->size = 1 + BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right);
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

static struct bstree_node * 
delete_node(const struct bstree *bst, struct bstree_node *node, const void *key)
{
	int cr;
	struct bstree_node *current;
	
	if (node == NULL)	/* not found key will be deleted */
		return NULL;
	
	cr = bst->cmp(key, node->key);
	if (cr == 1)
		node->left = delete_node(bst, node->left, key);
	else if (cr == -1)
		node->right = delete_node(bst, node->right, key);
	else {
		if (node->left == NULL) {
			current = node->right;
			if (bst->keysize != 0)
				ALGFREE(node->key);
			ALGFREE(node);
			return current;
		}
		
		if (node->right == NULL) {
			current = node->left;
			if (bst->keysize != 0)
				ALGFREE(node->key);
			ALGFREE(node);
			return current;
		}
		current = node;
		node = min_node(current->right);
		node->right = delete_min2(current->right);
		node->left = current->left;
		if (bst->keysize != 0)
			ALGFREE(current->key);
		ALGFREE(current);
	}
	node->size = BST_NODE_SIZE(node->left) + BST_NODE_SIZE(node->right) + 1;
	node->height = 1 + MAX(BST_HEIGHT_NODE(node->left), 
		BST_HEIGHT_NODE(node->right));
	
	return node;
}

/* Number of keys in the subtree less than key. */
static unsigned long 
rank_node(const struct bstree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	
	if (node == NULL)
		return 0;
	
	cr = kcmp(key, node->key);
	if (cr == 1)
		return rank_node(node->left, key, kcmp);
	else if (cr == -1) {
		return rank_node(node->right, key, kcmp) +
			BST_NODE_SIZE(node->left) + 1;
	} else
		return BST_NODE_SIZE(node->left);
}

/* 
 * Return element in BST rooted at node of given rank. 
 * Precondition: rank is in legal range.
 */
static void * 
select_node(struct bstree_node *node, unsigned long rank)
{
	unsigned long leftsize;
	
	if (node == NULL)
		return NULL;

	leftsize = BST_NODE_SIZE(node->left);
	if (rank < leftsize)
		return select_node(node->left, rank);
	else if (rank > leftsize)
		return select_node(node->right, rank - leftsize - 1);
	else
		return (node->key);
}

static struct bstree_node * 
floor_node(struct bstree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct bstree_node *rnode;
	
	if (node == NULL)	/* not found to the specified key */
		return NULL;
	
	cr = kcmp(key, node->key);
	if(cr == 1)
		return floor_node(node->left, key, kcmp);
	else if (cr == 0)
		return node;
	else {
		rnode = floor_node(node->right, key, kcmp);
		if (rnode != NULL)
			return rnode;
		else
			return node;
	}
}

static struct bstree_node * 
ceiling_node(struct bstree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct bstree_node *lnode;
	
	if (node == NULL)
		return NULL;
	
	cr = kcmp(key, node->key);
	if (cr == 0)
		return node;
	else if (cr == -1)
		return ceiling_node(node->right, key, kcmp);
	else {
		lnode = ceiling_node(node->left, key, kcmp);
		if (lnode != NULL)
			return lnode;
		else
			return node;
	}
}

static long
leaf_nodes(const struct bstree_node *node)
{
	if (node == NULL)
		return 0;
	else if (node->left == NULL && node->right == NULL)
		return 1;
	else
		return leaf_nodes(node->left) + leaf_nodes(node->right);
}

static void
keys_node(const struct bstree_node *node, const void *lokey, const void *hikey,
	algcomp_ft *kcmp, struct single_list *keys)
{
	int cmplo, cmphi;
	
	if (node == NULL)
		return;
	
	cmplo = kcmp(lokey, node->key);
	cmphi = kcmp(hikey, node->key);

	if (cmplo == 1)
		keys_node(node->left, lokey, hikey, kcmp, keys);
	if ((cmplo == 1 || cmplo == 0) && (cmphi == -1 || cmphi == 0))
		slist_append(keys, node->key);
	if (cmphi == -1)
		keys_node(node->right, lokey, hikey, kcmp, keys);
}

/* are the size fields correct? */
static int 
is_size_consistent(const struct bstree_node *node)
{
	if (node == NULL)
		return 1;	/* treat as empty constraint */
	if (node->size != BST_NODE_SIZE(node->left) +
	   BST_NODE_SIZE(node->right) + 1) {
		return 0;
	}
	return is_size_consistent(node->left) &&
		is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int
is_rank_consistent(const struct bstree *bst)
{
	unsigned long i;
	void *key, *el;
	struct single_list keys;
	struct slist_node *loc;
	
	for (i = 0; i < BST_SIZE(bst); i++) {
		el = bst_select(bst, i);
		if (i != bst_rank(bst, el))
			return 0;
	}

	bst_keys(bst, bst_min(bst), bst_max(bst), &keys);
	slist_rewind(&keys, &loc);
	while (slist_has_next(loc)) {
		key = slist_next_key(&loc);
		el = bst_select(bst, bst_rank(bst, key));
		if (bst->cmp(key, el) != 0)
			return 0;

	}
	slist_clear(&keys);
	
	return 1;
}

/* 
 * Is the tree rooted at Node a BST with all keys strictly between min and max
 * (if min or max is null, treat as empty constraint).
 */
static int
is_bst(const struct bstree_node *node, const void *minkey, const void *maxkey,
	algcomp_ft *kcmp)
{
	if (node == NULL)
		return 1;
	
	if (minkey != NULL && kcmp(node->key, minkey) == 1) 
		return 0;
	if (maxkey != NULL && kcmp(node->key, maxkey) == -1)
		return 0;
	return is_bst(node->left, minkey, node->key, kcmp) &&
		is_bst(node->right, node->key, maxkey, kcmp);
}
