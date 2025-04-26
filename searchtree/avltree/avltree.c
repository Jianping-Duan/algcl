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
#include "avltree.h"
#include "singlelist.h"
#include "queue.h"

/* Returns the number of nodes in the subtree. */
#define AVLBST_SIZE_NODE(node)	((node) == NULL ? 0 : (node)->size)

/* Returns the height of the subtree */
#define AVLBST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

/* 
 * Returns the balance factor of the subtree. 
 * The balance factor is defined as the difference in height of the left
 * subtree and right subtree, in this order.
 * Therefore, a subtree with a balance factor of -1, 0 or 1 has the AVL
 * property since the heights of the two child subtrees differ by at most one.
 */
#define BALANCE_FACTOR(node)	\
	(AVLBST_HEIGHT_NODE((node)->left) - AVLBST_HEIGHT_NODE((node)->right))

static struct avl_node * get_node(struct avl_node *, const void *,
	algcomp_ft *);
static struct avl_node * make_node(const void *, unsigned int);
static inline struct avl_node * rotate_right(struct avl_node *);
static inline struct avl_node * rotate_left(struct avl_node *);
static struct avl_node * balance(struct avl_node *);
static struct avl_node * put_node(const struct avl_tree *, struct avl_node *,
	const void *);
static void release_subtree(struct avl_node *, unsigned int);
static void preorder_nodes(const struct avl_node *, struct single_list *);
static struct avl_node * min_node(struct avl_node *);
static struct avl_node * max_node(struct avl_node *);
static struct avl_node * delete_min_node(struct avl_node *, unsigned int);
static struct avl_node * delete_min(struct avl_node *);
static struct avl_node * delete_max_node(struct avl_node *, unsigned int);
static struct avl_node * delete_node(const struct avl_tree *, struct avl_node *,
	const void *);
static struct avl_node * floor_node(struct avl_node *, const void *,
	algcomp_ft *);
static struct avl_node * ceiling_node(struct avl_node *, const void *,
	algcomp_ft *);
static unsigned long rank_node(const struct avl_node *, const void *,
	algcomp_ft *);
static void * select_node(unsigned long, struct avl_node *);
static void keys_range(const struct avl_node *, const void *, const void *,
	algcomp_ft *, struct single_list *);
static int isbst(const struct avl_node *, const void *,	const void *,
	algcomp_ft *);
static int isavl(const struct avl_node *node);
static int is_size_consistent(const struct avl_node *node);
static int is_rank_consistent(const struct avl_tree *bst);

/* Initializes an empty AVL tree */
void
avlbst_init(struct avl_tree *avl, unsigned int ksize, algcomp_ft *cmp)
{
	avl->root = NULL;
	avl->keysize = ksize;
	avl->cmp = cmp;
}

/* Returns the key in this avl tree by the given key. */
void * 
avlbst_get(const struct avl_tree *avl, const void *key)
{
	struct avl_node *node;
	
	if (key == NULL)
		return NULL;
	
	if ((node = get_node(avl->root, key, avl->cmp)) == NULL)
		return NULL;
	return (node->key);
}

/* Inserts the key into the AVL tree. */
void 
avlbst_put(struct avl_tree *avl, const void *key)
{
	if (key == NULL)
		return;
	
	avl->root = put_node(avl, avl->root, key);
}

/* Releases the AVL tree. */
void 
avlbst_clear(struct avl_tree *avl)
{
	if (!AVLBST_ISEMPTY(avl))
		release_subtree(avl->root, avl->keysize);
}

void 
avlbst_preorder(const struct avl_tree *avl, struct single_list *keys)
{
	slist_init(keys, 0, avl->cmp);
	preorder_nodes(avl->root, keys);
}

/* Returns the smallest key in the AVL BST */
void * 
avlbst_min(const struct avl_tree *avl)
{
	if (AVLBST_ISEMPTY(avl))
		return NULL;
	return min_node(avl->root)->key;
}

/* Returns the largest key in the AVL BST */
void *
avlbst_max(const struct avl_tree *avl)
{
	if (AVLBST_ISEMPTY(avl))
		return NULL;
	return max_node(avl->root)->key;
}

/* Removes the smallest key and associated with value from this AVL tree. */
void 
avlbst_delete_min(struct avl_tree *avl)
{
	if (!AVLBST_ISEMPTY(avl))
		avl->root = delete_min_node(avl->root, avl->keysize);
}

/* Removes the largest key from this AVL tree. */
void 
avlbst_delete_max(struct avl_tree *avl)
{
	if (!AVLBST_ISEMPTY(avl))
		avl->root = delete_max_node(avl->root, avl->keysize);
}

/* Removes the specified key from the AVL tree. */
int
avlbst_delete(struct avl_tree *avl, const void *key)
{
	if (AVLBST_ISEMPTY(avl))
		return -1;
	
	if (avlbst_get(avl, key) == NULL)
		return -2;
	
	avl->root = delete_node(avl, avl->root, key);
	return 0;
}

/* Returns the largest key in the AVL tree less than or equal to Key. */
void * 
avlbst_floor(const struct avl_tree *avl, const void *key)
{
	struct avl_node *current;
	
	current = floor_node(avl->root, key, avl->cmp);
	return current == NULL ? NULL : (current->key);
}

/* Returns the smallest key in the AVL tree greater than or equal to Key. */
void * 
avlbst_ceiling(const struct avl_tree *avl, const void *key)
{
	struct avl_node *current;
	
	current = ceiling_node(avl->root, key, avl->cmp);
	return current == NULL ? NULL : (current->key);
}

/* Returns the number of keys in the AVL BST strictly less than Key. */
unsigned long 
avlbst_rank(const struct avl_tree *avl, const void *key)
{
	return rank_node(avl->root, key, avl->cmp);
}

/* 
 * Return the key in the AVL BST of a given rank.
 * This key has the property that there are rank keys in the AVL BST that are
 * smaller.
 * In other words, this key is the (rank+1)st smallest key in the AVL BST.
 */
void * 
avlbst_select(const struct avl_tree *avl, unsigned long rank)
{
	if (rank < AVLBST_SIZE(avl))
		return select_node(rank, avl->root);
	return NULL;
}

/* Returns all keys in the AVL tree in the given range. */
void
avlbst_keys(const struct avl_tree *avl, const void *lokey, const void *hikey,
		struct single_list *keys)
{
	slist_init(keys, 0, avl->cmp);
	keys_range(avl->root, lokey, hikey, avl->cmp, keys);
}

void 
avlbst_breadth_first(const struct avl_tree *avl, struct single_list *keys)
{
	struct queue qp;
	struct avl_node *proot;
	
	if (AVLBST_ISEMPTY(avl))
		return;
	
	QUEUE_INIT(&qp, 0);
	slist_init(keys, 0, avl->cmp);
	proot = avl->root;
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

/* Check integrity of AVL tree data structure. */
int
avlbst_check(const struct avl_tree *avl)
{
	int flag = 1;
	
	if (!isbst(avl->root, NULL, NULL, avl->cmp)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if (!isavl(avl->root)) {
		printf("Not a avl tree.\n");
		flag = 0;
	}
	
	if (!is_size_consistent(avl->root)) {
		printf("Subtree counts not consistent.\n");
		flag = 0;
	}
	
	if (!is_rank_consistent(avl)) {
		printf("Ranks not consistent.\n");
		flag = 0;
	}

	return flag;
}

/******************** static function boundary ********************/

/* 
 * Returns AVL Node associated with 
 * the given key in the subtree.
 */
static struct avl_node * 
get_node(struct avl_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	
	if (node == NULL)
		return NULL;	/* not found key */
	
	cr = kcmp(key, node->key);
	if (cr == 1)
		return get_node(node->left, key, kcmp);
	else if (cr == -1)
		return get_node(node->right, key, kcmp);
	else
		return node;
}

/* Make the AVL node for AVL tree using the specified key. */
static struct avl_node * 
make_node(const void *key, unsigned int ksize)
{
	struct avl_node *current;
	
	current = (struct avl_node *)algmalloc(sizeof(struct avl_node));
	
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;
	current->left = NULL;
	current->right = NULL;
	current->height = 0;	/* just one node height is 0 */
	current->size = 1;
	
	return current;
}

/* 
 * Rotates the given subtree to the right, meanwhile, updates the size and
 * height of subtree. 
 */
static inline struct avl_node * 
rotate_right(struct avl_node *hnode)
{
	struct avl_node *lnode;
	
	lnode = hnode->left;
	hnode->left = lnode->right;
	lnode->right = hnode;
	lnode->size = hnode->size;
	hnode->size = 1 + AVLBST_SIZE_NODE(hnode->left) + 
		AVLBST_SIZE_NODE(hnode->right);
	hnode->height = 1 + MAX(AVLBST_HEIGHT_NODE(hnode->left), 
		AVLBST_HEIGHT_NODE(hnode->right));
	lnode->height = 1 + MAX(AVLBST_HEIGHT_NODE(lnode->left), 
		AVLBST_HEIGHT_NODE(lnode->right));
	
	return lnode;
}

/* 
 * Rotates the given subtree to the left, meanwhile, updates the size and
 * height of subtree. 
 */
static inline struct avl_node * 
rotate_left(struct avl_node *hnode)
{
	struct avl_node *lnode;
	
	lnode = hnode->right;
	hnode->right = lnode->left;
	lnode->left = hnode;
	lnode->size = hnode->size;
	hnode->size = 1 + AVLBST_SIZE_NODE(hnode->left) + 
		AVLBST_SIZE_NODE(hnode->right);
	hnode->height = 1 + MAX(AVLBST_HEIGHT_NODE(hnode->left), 
		AVLBST_HEIGHT_NODE(hnode->right));
	lnode->height = 1 + MAX(AVLBST_HEIGHT_NODE(lnode->left), 
		AVLBST_HEIGHT_NODE(lnode->right));
	
	return lnode;
}

/* Restores the AVL treevvvproperty of the subtree. */
static struct avl_node * 
balance(struct avl_node *node)
{
	if (BALANCE_FACTOR(node) < -1) {
		if (BALANCE_FACTOR(node->right) > 0)
			node->right = rotate_right(node->right);
		node = rotate_left(node);
	} else if (BALANCE_FACTOR(node) > 1) {
		if (BALANCE_FACTOR(node->left) < 0)
			node->left = rotate_left(node->left);
		node = rotate_right(node);
	}
	
	return node;
}

/* 
 * Inserts the key-value pair in the subtree. 
 * It overrides the old value with the new value if the AVL tree already
 * contains the specified key. 
 */
static struct avl_node * 
put_node(const struct avl_tree *avl, struct avl_node *node, const void *key)
{
	int cr;
	
	if(node == NULL)
		return make_node(key, avl->keysize);
	
	cr = avl->cmp(key, node->key);
	if (cr == 1)
		node->left = put_node(avl, node->left, key);
	else if (cr == -1)
		node->right = put_node(avl, node->right, key);
	else
		return node;
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

static void 
release_subtree(struct avl_node *root, unsigned int ksize)
{
	if (root != NULL) {
		release_subtree(root->left, ksize);
		release_subtree(root->right, ksize);
		if (ksize != 0)
			ALGFREE(root->key);
		ALGFREE(root);
	}
}

static void 
preorder_nodes(const struct avl_node *root, struct single_list *keys)
{
	if (root != NULL) {
		slist_append(keys, root->key);
		preorder_nodes(root->left, keys);
		preorder_nodes(root->right, keys);
	}
}

/* 
 * The smallest key in the subtree rooted at Node; 
 * null if no search key.
 */
static struct avl_node * 
min_node(struct avl_node *node)
{
	if (node->left == NULL)
		return node;
	return min_node(node->left);
}

/* 
 * The largest key in the subtree rooted at Node; 
 * null if no search key.
 */
static struct avl_node * 
max_node(struct avl_node *node)
{
	if (node->right == NULL)
		return node;
	return max_node(node->right);
}

/* Delete the keywith the minimum key rooted at Node. */
static struct avl_node * 
delete_min_node(struct avl_node *node, unsigned int ksize)
{
	struct avl_node *current;
	
	if (node->left == NULL) {
		current = node->right;
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return current;
	}
	
	node->left = delete_min_node(node->left, ksize);
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

static struct avl_node * 
delete_min(struct avl_node *node)
{
	struct avl_node *current;
	
	if (node->left == NULL) {
		current = node->right;
		return current;
	}
	
	node->left = delete_min(node->left);
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

/* Delete the key with the maximum key rooted at Node. */
static struct avl_node * 
delete_max_node(struct avl_node *node, unsigned int ksize)
{
	struct avl_node *current;
	
	if (node->right == NULL) {
		current = node->left;
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return current;
	}
	
	node->right = delete_max_node(node->right, ksize);
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

/* Delete the key by the given key rooted at Node. */
static struct avl_node * 
delete_node(const struct avl_tree *avl, struct avl_node *node, const void *key)
{
	int cr;
	struct avl_node *current;
	
	if (node == NULL)
		return NULL;
	
	cr = avl->cmp(key, node->key);
	if (cr == 1)
		node->left = delete_node(avl, node->left, key);
	else if (cr == -1)
		node->right = delete_node(avl, node->right, key);
	else {
		if (node->left == NULL) {
			current = node->right;
			if (avl->keysize != 0)
				ALGFREE(node->key);
			ALGFREE(node);
			return current;
		} else if (node->right == NULL) {
			current = node->left;
			if (avl->keysize != 0)
				ALGFREE(node->key);
			ALGFREE(node);
			return current;
		} else {
			current = node;
			node = min_node(current->right);
			node->right = delete_min(current->right);
			node->left = current->left;
			if (avl->keysize != 0)
				ALGFREE(current->key);
			ALGFREE(current);
		}
	}
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

/* 
 * The largest key in the subtree rooted at Node 
 * less than or equal to the given key.
 */
static struct avl_node * 
floor_node(struct avl_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct avl_node *rnode;
	
	if (node == NULL)	/* not found the specified key */
		return NULL;

	if ((cr = kcmp(key, node->key)) == 0)
		return node;
	if (cr == 1)
		return floor_node(node->left, key, kcmp);
	if ((rnode = floor_node(node->right, key, kcmp)) != NULL)
		return rnode;
	else
		return node;
}

/* 
 * The smallest key in the subtree rooted at Node 
 * greater than or equal to the given key.
 */
static struct avl_node * 
ceiling_node(struct avl_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct avl_node *lnode;
	
	if (node == NULL)
		return NULL;
	
	if ((cr = kcmp(key, node->key)) == 0)
		return node;
	if (cr == -1)
		return ceiling_node(node->right, key, kcmp);
	if ((lnode = ceiling_node(node->left, key, kcmp)) != NULL)
		return lnode;
	else
		return node;
}

/* 
 * Number of keys less than the given key 
 * in the subtree rooted at Node.
 */
static unsigned long 
rank_node(const struct avl_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	
	if (node == NULL)
		return 0;
	
	if ((cr = kcmp(key, node->key)) == 1)
		return rank_node(node->left, key, kcmp);
	if (cr == -1) {
		return 1 + AVLBST_SIZE_NODE(node->left) +
			rank_node(node->right, key, kcmp);
	} else
		return AVLBST_SIZE_NODE(node->left);
}

/* 
 * Return key in BST rooted at Node of given rank.
 * Precondition: rank is in legal range. 
 */
static void * 
select_node(unsigned long rank, struct avl_node *node)
{
	unsigned long leftsize;
	
	if (node == NULL)
		return NULL;
	
	leftsize = AVLBST_SIZE_NODE(node->left);
	if (rank < leftsize)
		return select_node(rank, node->left);
	if (rank > leftsize)
		return select_node(rank - leftsize - 1, node->right);
	else
		return (node->key);
}

/* 
 * Adds the keys between Lokey and Hikey in the subtree 
 * rooted at Node to the Queue. 
 */
static void 
keys_range(const struct avl_node *node, const void *lokey, const void *hikey,
		algcomp_ft *kcmp, struct single_list *keys)
{
	int cmplo, cmphi;
	
	if (node == NULL)
		return;
	
	cmplo = kcmp(lokey, node->key);
	cmphi = kcmp(hikey, node->key);
	
	if (cmplo == 1)
		keys_range(node->left, lokey, hikey, kcmp, keys);
	if ((cmplo == 1 || cmplo == 0) && (cmphi == -1 || cmphi == 0))
		slist_append(keys, node->key);
	if (cmphi == -1)
		keys_range(node->right, lokey, hikey, kcmp, keys);
}

/* 
 * Checks if the tree rooted at Note is a BST with all keys strictly between
 * min and max (if min or max is null, treat as empty constraint).
 */
static int 
isbst(const struct avl_node *node, const void *minkey, const void *maxkey,
	algcomp_ft *kcmp)
{
	if (node == NULL)
		return 1;		/* empty constraint */
	
	if (minkey != NULL && kcmp(node->key, minkey) == 1)
		return 0;
	if (maxkey != NULL && kcmp(node->key, maxkey) == -1)
		return 0;
	
	return isbst(node->left, minkey, node->key, kcmp) &&
	   isbst(node->right, node->key, maxkey, kcmp);
}

/* Checks if AVL property is consistent in the subtree. */
static int 
isavl(const struct avl_node *node)
{
	int bf;
	
	if (node == NULL)
		return 1;
	
	bf = BALANCE_FACTOR(node);
	if (bf < -1 || bf > 1)
		return 0;
	
	return isavl(node->left) && isavl(node->right);
}

/* are the size fields correct? */
static int 
is_size_consistent(const struct avl_node *node)
{
	if (node == NULL)
		return 1;		/* treat as empty constraint */
	if (node->size != AVLBST_SIZE_NODE(node->left) +
		AVLBST_SIZE_NODE(node->right) + 1) {
		return 0;
	}
	return is_size_consistent(node->left) && is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int 
is_rank_consistent(const struct avl_tree *bst)
{
	unsigned long i;
	void *key, *el;
	struct single_list keys;
	struct slist_node *loc;

	for (i = 0; i < AVLBST_SIZE(bst); i++) {
		el = avlbst_select(bst, i);
		if (i != avlbst_rank(bst, el))
			return 0;
	}

	avlbst_keys(bst, avlbst_min(bst), avlbst_max(bst), &keys);
	slist_rewind(&keys, &loc);
	while (slist_has_next(loc)) {
		key = slist_next_key(&loc);
		el = avlbst_select(bst, avlbst_rank(bst, key));
		if (bst->cmp(key, el) != 0)
			return 0;
	}
	slist_clear(&keys);

	return 1;
}
