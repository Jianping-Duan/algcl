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
#include "redblackbst.h"
#include "singlelist.h"

#define RBBST_SIZE_NODE(node)	((node) == NULL ? 0 : (node)->size)
#define RBBST_ISRED(node)	((node) == NULL ? 0 : (node)->color == RED)
#define RBBST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

/* Flips the colors of node and its two children */
#define FLIP_COLORS(node)		do {					\
	if (node != NULL) {									\
		node->color = !node->color;						\
		if (node->left != NULL)							\
			node->left->color = !node->left->color;		\
		if (node->right != NULL)						\
			node->right->color = !node->right->color;	\
	}													\
} while (0)

static void * get_node(struct rbtree_node *, const void *, algcomp_ft *);
static struct rbtree_node * make_node(const void *, unsigned int);
static inline struct rbtree_node * rotate_right(struct rbtree_node *);
static inline struct rbtree_node * rotate_left(struct rbtree_node *);
static inline struct rbtree_node * balance(struct rbtree_node *);
static struct rbtree_node * put_node(const struct rbtree *,
	struct rbtree_node *, const void *);
static void release_subtree(struct rbtree_node *, unsigned int);
static void preorder_nodes(struct rbtree_node *, struct single_list *);
static struct rbtree_node * min_node(struct rbtree_node *);
static struct rbtree_node * max_node(struct rbtree_node *);
static struct rbtree_node * move_red_left(struct rbtree_node *);
static struct rbtree_node * move_red_right(struct rbtree_node *);
static struct rbtree_node * delete_min_node(struct rbtree_node *, unsigned int);
static struct rbtree_node * delete_max_node(struct rbtree_node *, unsigned int);
static struct rbtree_node * delete_node(const struct rbtree *,
	struct rbtree_node *, const void *);
static int isbst(const struct rbtree_node *, const void *, const void *,
	algcomp_ft *);
static int is23(const struct rbtree_node *, const struct rbtree_node *);
static inline int isbal_node(const struct rbtree_node *node, int blacks);
static int isbalanced(const struct rbtree_node *);
static int is_size_consistent(const struct rbtree_node *);
static int is_rank_consistent(const struct rbtree *);
static struct rbtree_node * floor_node(struct rbtree_node *, const void *,
	algcomp_ft *);
static struct rbtree_node * ceiling_node(struct rbtree_node *, const void *,
	algcomp_ft *);
static unsigned long rank_node(const struct rbtree_node *, const void *,
	algcomp_ft *);
static void * select_node(struct rbtree_node *, unsigned long);
static void keys_range(const struct rbtree_node *, const void *, const void *,
	algcomp_ft *, struct single_list *);

/* Initializes an empty Red-Black binary search tree. */
void
rbbst_init(struct rbtree *bst, unsigned int ksize, algcomp_ft *kcmp)
{
	bst->root = NULL;
	bst->keysize = ksize;
	bst->cmp = kcmp;
}

/* Returns item associated with the given key. */
void * 
rbbst_get(const struct rbtree *bst, const void *key)
{
	if (key == NULL)
		return NULL;
	return get_node(bst->root, key, bst->cmp);
}

/* Inserts the specified key into the Red-Black BST. */
int
rbbst_put(struct rbtree *bst, const void *key)
{
	if (key == NULL)
		return -1;
	bst->root = put_node(bst, bst->root, key);
	bst->root->color = BLACK;
	return 0;
}

/* Release the Red-Black tree */
void
rbbst_clear(struct rbtree *bst)
{
	if (!RBBST_ISEMPTY(bst))
		release_subtree(bst->root, bst->keysize);
}

/* Preorder traverse. */
void 
rbbst_preorder(const struct rbtree *bst, struct single_list *keys)
{
	slist_init(keys, 0, bst->cmp);
	if (!RBBST_ISEMPTY(bst))
		preorder_nodes(bst->root, keys);
}

/* Returns the smallest key in the Red-Black BST. */
void * 
rbbst_min(const struct rbtree *bst)
{
	if (RBBST_ISEMPTY(bst))
		return NULL;
	return (min_node(bst->root)->key);
}

/* Returns the largest key in the Red-Black BST. */
void *
rbbst_max(const struct rbtree *bst)
{
	if (RBBST_ISEMPTY(bst))
		return NULL;
	return (max_node(bst->root)->key);
}

/* Remove the smallest key from this Red-Black BST. */
int
rbbst_delete_min(struct rbtree *bst)
{
	if (RBBST_ISEMPTY(bst))
		return -1;
	
	/* if both children of root are black, set root to red. */
	if (!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	
	bst->root = delete_min_node(bst->root, bst->keysize);
	if (!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;

	return 0;
}

/* 
 * Remove the largest key from this Red-Black BST.
 */
int
rbbst_delete_max(struct rbtree *bst)
{
	if (RBBST_ISEMPTY(bst))
		return -1;
	
	/* if both children of root are black, set root to red. */
	if (!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	
	bst->root = delete_max_node(bst->root, bst->keysize);
	if (!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;

	return 0;
}

/* Removes the specified key from this Red-Black BST. */
int
rbbst_delete(struct rbtree *bst, const void *key)
{
	if (RBBST_ISEMPTY(bst))
		return -1;
	
	if (rbbst_get(bst, key) == NULL)
		return -2;
	
	if (!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	bst->root = delete_node(bst, bst->root, key);
	
	if (!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;
	
	return 0;
}

/* Check integrity of red-black tree data structure. */
int 
rbbst_check(const struct rbtree *bst)
{
	int flag = 1;
	
	if (!isbst(bst->root, NULL, NULL, bst->cmp)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if (!is23(bst->root, bst->root)) {
		printf("Not a 2-3 tree.\n");
		flag = 0;
	}
	
	if (!isbalanced(bst->root)) {
		printf("Not balanced.\n");
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

/* Returns the largest key in the Red-Black tree less than or equal to Key. */
void * 
rbbst_floor(const struct rbtree *bst, const void *key)
{
	struct rbtree_node *current;
	
	current = floor_node(bst->root, key, bst->cmp);
	/* NULL is the specified key to small. */
	return (current == NULL ? NULL : current->key);	
}

/* Returns the smallest key in the Red-Black greater than or equal to Key. */
void * 
rbbst_ceiling(const struct rbtree *bst, const void *key)
{
	struct rbtree_node *current;
	
	current = ceiling_node(bst->root, key, bst->cmp);
	/* NULL is the specified key to large. */
	return (current == NULL ? NULL : current->key);
}

/* Return the number of keys in the Red-Black BST strictly less than Key. */
unsigned long 
rbbst_rank(const struct rbtree *bst, const void *key)
{
	return rank_node(bst->root, key, bst->cmp);
}

/* 
 * Return the key in the BST of a given rank.
 * This key has the property that there are rank keys in the BST that are
 * smaller.
 * In other words, this key is the (rank+1)st smallest key in the BST.
 */
void * 
rbbst_select(const struct rbtree *bst, unsigned long rank)
{
	if(rank < RBBST_SIZE(bst))
		return select_node(bst->root, rank);
	return NULL;
}

/* 
 * Returns all keys in the Red-Black BST in the given range in ascending order.
 */
void
rbbst_keys(const struct rbtree *bst, const void *lokey, const void *hikey,
		struct single_list *keys)
{
	slist_init(keys, 0, bst->cmp);
	keys_range(bst->root, lokey, hikey, bst->cmp, keys);
}

/******************** static function boundary ********************/

/* 
 * The Key with the given key in subtree rooted at node;
 * if null no search key.
 */
static void * 
get_node(struct rbtree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct rbtree_node *proot;
	
	proot = node;
	while (proot != NULL) {
		cr = kcmp(key, proot->key);
		if (cr == 1)
			proot = proot->left;
		else if (cr == -1)
			proot = proot->right;
		else
			return (proot->key);
	}
	return NULL;
}

/* 
 * New create Red-Black BST node using 
 * the specified key-value pair. 
 */
static struct rbtree_node * 
make_node(const void *key, unsigned int ksize)
{
	struct rbtree_node *current;
	
	current = (struct rbtree_node *)algmalloc(sizeof(struct rbtree_node));
	
	if (ksize != 0) {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	} else
		current->key = (void *)key;
	current->left = NULL;
	current->right = NULL;
	current->color = RED;	/* default red link */
	current->size = 1;
	current->height = 0;
	
	return current;
}

/* Make a left-leaning link lean to the right */
static inline struct rbtree_node * 
rotate_right(struct rbtree_node *hnode)
{
	struct rbtree_node *lnode;
	
	assert(hnode != NULL && RBBST_ISRED(hnode->left));
	
	lnode = hnode->left;
	hnode->left = lnode->right;
	lnode->right = hnode;
	lnode->color = hnode->color;	/* update color */
	hnode->color = RED;
	lnode->size = hnode->size;		/* update size */
	
	/* update the size of high node. */
	hnode->size = RBBST_SIZE_NODE(hnode->left) +
		RBBST_SIZE_NODE(hnode->right) + 1;
	
	/* update the height of high node and low node */
	hnode->height = 1 + MAX(RBBST_HEIGHT_NODE(hnode->left),
		RBBST_HEIGHT_NODE(hnode->right));
	lnode->height = 1 + MAX(RBBST_HEIGHT_NODE(lnode->left),
		RBBST_HEIGHT_NODE(lnode->right));
	
	return lnode;	/* now this is a left high node */
}

/* Make a right-leaning link lean to the left */
static inline struct rbtree_node * 
rotate_left(struct rbtree_node *hnode)
{
	struct rbtree_node *lnode;
	
	assert(hnode != NULL && RBBST_ISRED(hnode->right));
	
	lnode = hnode->right;
	hnode->right = lnode->left;
	lnode->left = hnode;
	lnode->color = hnode->color;	/* update color */
	hnode->color = RED;
	lnode->size = hnode->size;		/* update size */
	
	/* update the size of high node. */
	hnode->size = RBBST_SIZE_NODE(hnode->left) +
		RBBST_SIZE_NODE(hnode->right) + 1;
	
	/* update the height of high node and low node */
	hnode->height = 1 + MAX(RBBST_HEIGHT_NODE(hnode->left),
		RBBST_HEIGHT_NODE(hnode->right));
	lnode->height = 1 + MAX(RBBST_HEIGHT_NODE(lnode->left),
		RBBST_HEIGHT_NODE(lnode->right));
	
	return lnode;	/* now this is a right high node */
}

/* Restore red-black BST invariant */
static inline struct rbtree_node * 
balance(struct rbtree_node *hnode)
{
	if (RBBST_ISRED(hnode->right) && !RBBST_ISRED(hnode->left))
		hnode = rotate_left(hnode);
	if (RBBST_ISRED(hnode->left) && RBBST_ISRED(hnode->left->left))
		hnode = rotate_right(hnode);
	if (RBBST_ISRED(hnode->left) && RBBST_ISRED(hnode->right))
		FLIP_COLORS(hnode);
	
	hnode->size = RBBST_SIZE_NODE(hnode->left) +
		RBBST_SIZE_NODE(hnode->right) + 1;
	hnode->height = 1 + MAX(RBBST_HEIGHT_NODE(hnode->left),
		RBBST_HEIGHT_NODE(hnode->right));
	
	return hnode;
}

/* Insert the key-value pair in the subtree rooted as hnode. */
static struct rbtree_node * 
put_node(const struct rbtree *bst, struct rbtree_node *hnode, const void *key)
{
	int cr;
	
	if (hnode == NULL)
		return make_node(key, bst->keysize);
	
	cr = bst->cmp(key, hnode->key);
	if (cr == 1)
		hnode->left = put_node(bst, hnode->left, key);
	else if (cr == -1)
		hnode->right = put_node(bst, hnode->right, key);
	else
		return hnode;
	
	return balance(hnode);
}

static void 
release_subtree(struct rbtree_node *root, unsigned int ksize)
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
preorder_nodes(struct rbtree_node *root, struct single_list *keys)
{
	if (root != NULL) {
		slist_append(keys, root->key);
		preorder_nodes(root->left, keys);
		preorder_nodes(root->right, keys);
	}
}

/* 
 * The smallest key in the subtree rooted at node; 
 * null if no search key.
 */
static struct rbtree_node * 
min_node(struct rbtree_node *node)
{
	if (node->left == NULL)
		return node;
	return min_node(node->left);
}

/* 
 * The largest key in the subtree rooted at node;
 * null if no search key.
 */
static struct rbtree_node * 
max_node(struct rbtree_node *node)
{
	if (node->right == NULL)
		return node;
	return max_node(node->right);
}

/* 
 * Assuming that hnode is red and both hnode->left and 
 * hnode->left->left are black, make hnode->left or its children red.
 */
static struct rbtree_node * 
move_red_left(struct rbtree_node *hnode)
{
	FLIP_COLORS(hnode);
	if (hnode != NULL && hnode->right != NULL &&
	   RBBST_ISRED(hnode->right->left)) {
		hnode->right = rotate_right(hnode->right);
		hnode = rotate_left(hnode);
		FLIP_COLORS(hnode);
	}
	
	return hnode;
}

/* 
 * Assuming that hnode is red and both hnode->right and 
 * hnode->right->left are black, make hnode->right or its children red.
 */
static struct rbtree_node * 
move_red_right(struct rbtree_node *hnode)
{
	FLIP_COLORS(hnode);
	if (RBBST_ISRED(hnode->left) && RBBST_ISRED(hnode->left->left)) {
		hnode = rotate_right(hnode);
		FLIP_COLORS(hnode);
	}
	
	return hnode;
}

/* Delete the minimum key rooted at node. */
static struct rbtree_node * 
delete_min_node(struct rbtree_node *node, unsigned int ksize)
{	
	if (node->left == NULL) {
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return NULL;
	}

	/* node is 2-node */
	if (!RBBST_ISRED(node->left) && !RBBST_ISRED(node->left->left))
		node = move_red_left(node);
	node->left = delete_min_node(node->left, ksize);
	
	return balance(node);
}

/* Delete the maximum key rooted at node. */
static struct rbtree_node * 
delete_max_node(struct rbtree_node *node, unsigned int ksize)
{
	if (RBBST_ISRED(node->left))		/* node is 3-node */
		node = rotate_right(node);
		
	if (node->right == NULL) {
		if (ksize != 0)
			ALGFREE(node->key);
		ALGFREE(node);
		return NULL;
	}

	/* left node is 2-node */
	if (!RBBST_ISRED(node->right) && !RBBST_ISRED(node->right->left))
		node = move_red_right(node);
	node->right = delete_max_node(node->right, ksize);
	
	return balance(node);
}

/* 
 * Delete the key-value pair with 
 * the given key rooted at NODE.
 */
static struct rbtree_node * 
delete_node(const struct rbtree *bst, struct rbtree_node *node, const void *key)
{
	struct rbtree_node *minnode;
	
	if (node == NULL)
		return NULL;
	
	if (bst->cmp(key, node->key) == 1) {
		if (!RBBST_ISRED(node->left) && !RBBST_ISRED(node->left->left))
			node = move_red_left(node);
		node->left = delete_node(bst, node->left, key);
	} else {
		if (RBBST_ISRED(node->left))
			node = rotate_right(node);
	
		/* may max key */
		if (bst->cmp(key, node->key) == 0 && node->right == NULL) {
			if (bst->keysize != 0)
				ALGFREE(node->key);
			ALGFREE(node);
			return NULL;
		}
		
		if (!RBBST_ISRED(node->right) && !RBBST_ISRED(node->right->left))
			node = move_red_right(node);
		
		if (bst->cmp(key, node->key) == 0) {
			minnode = min_node(node->right);
			/* coping delete */
			if (bst->keysize == 0)
				node->key = minnode->key;
			else
				memcpy(node->key, minnode->key, bst->keysize);
			node->right = delete_min_node(node->right, bst->keysize);
		} else
			node->right = delete_node(bst, node->right, key);
	}
	
	return balance(node);
}

/* 
 * Is the tree rooted at Node a BST with all keys strictly between min and max
 * (if min or max is null, treat as empty constraint).
 */
static int
isbst(const struct rbtree_node *node, const void *minkey, const void *maxkey,
	algcomp_ft *kcmp)
{
	if (node == NULL)
		return 1;	/* empty tree */
	
	if (minkey != NULL && kcmp(node->key, minkey) == 1)
		return 0;
	if (maxkey != NULL && kcmp(node->key, maxkey) == -1)
		return 0;
	return isbst(node->left, minkey, node->key, kcmp) &&
		isbst(node->right, node->key, maxkey, kcmp);
}

/* 
 * Does the tree have no red right links, 
 * and at most one (left) red links in a row on any path? 
 */
static int
is23(const struct rbtree_node *root, const struct rbtree_node *node)
{
	if (node == NULL)
		return 1;
	if (RBBST_ISRED(node->right))
		return 0;
	if (root != node && RBBST_ISRED(node) && RBBST_ISRED(node->left))
		return 0;
	
	return is23(root, node->left) && is23(root, node->right);
}

/* 
 * Does every path from the root to a leaf have 
 * he given number of black links? 
 */
static inline int 
isbal_node(const struct rbtree_node *node, int blacks)
{
	if (node == NULL)
		return blacks == 0;
	if (!RBBST_ISRED(node))
		blacks--;
	return isbal_node(node->left, blacks) && isbal_node(node->right, blacks);
}

/* Do all paths from root to leaf have same number of black links? */
static int 
isbalanced(const struct rbtree_node *root)
{
	const struct rbtree_node *proot;
	int blacks = 0;	/* number of black links on path from root to min */
	
	proot = root;
	while (proot != NULL) {
		if (!RBBST_ISRED(proot))
			blacks++;
		proot = proot->left;
	}
	
	return isbal_node(root, blacks);
}

/* Are the size of fields correct ? */
static int 
is_size_consistent(const struct rbtree_node *node)
{
	if (node == NULL)
		return 1;	/* empty consistent */
	if (RBBST_SIZE_NODE(node) != RBBST_SIZE_NODE(node->left) +
		RBBST_SIZE_NODE(node->right) + 1) {
		return 0;
	}
	return is_size_consistent(node->left) && is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int 
is_rank_consistent(const struct rbtree *bst)
{
	unsigned long i;
	void *key, *el;
	struct single_list keys;
	struct slist_node *loc;
	
	for(i = 0; i < RBBST_SIZE(bst); i++) {
		el = rbbst_select(bst, i);
		if(i != rbbst_rank(bst, el))
			return 0;
	}
	
	rbbst_keys(bst, rbbst_min(bst), rbbst_max(bst), &keys);
	slist_rewind(&keys, &loc);
	while (slist_has_next(loc)) {
		key = slist_next_key(&loc);
		el = rbbst_select(bst, rbbst_rank(bst, key));
		if (bst->cmp(key, el) != 0)
			return 0;

	}
	slist_clear(&keys);
	
	return 1;
}

/* 
 * The largest key in the subtree rooted at Node less than 
 * or equal to the given key.
 */
static struct rbtree_node * 
floor_node(struct rbtree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct rbtree_node *rnode;
	
	if (node == NULL)	/* not found the specified key */
		return NULL;

	if ((cr = kcmp(key, node->key)) == 0)
		return node;
	if(cr == 1)
		return floor_node(node->left, key, kcmp);
	if ((rnode = floor_node(node->right, key, kcmp)) != NULL)
		return rnode;
	else
		return node;
}

/* 
 * The smallest key in the subtree rooted at Node greater than
 * or equal to the given key.
 */
static struct rbtree_node * 
ceiling_node(struct rbtree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	struct rbtree_node *lnode;
	
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

/* Number of keys less than key in the subtree rooted at Node. */
static unsigned long 
rank_node(const struct rbtree_node *node, const void *key, algcomp_ft *kcmp)
{
	int cr;
	
	if (node == NULL)
		return 0;
	
	if ((cr = kcmp(key, node->key)) == 1)
		return rank_node(node->left, key, kcmp);
	if (cr == -1)
		return 1 + RBBST_SIZE_NODE(node->left) + rank_node(node->right, key, kcmp);
	else
		return RBBST_SIZE_NODE(node->left);
}

/* 
 * Return key in BST rooted at Node of given rank.
 * Precondition: rank is in legal range. 
 */
static void * 
select_node(struct rbtree_node *node, unsigned long rank)
{
	unsigned long leftsize;
	
	if (node == NULL)
		return NULL;
	
	leftsize = RBBST_SIZE_NODE(node->left);
	if (rank < leftsize)
		return select_node(node->left, rank);
	if (rank > leftsize)
		return select_node(node->right, rank - leftsize - 1);
	else
		return (node->key);
}

/* 
 * Add the keys between lokey and hikey in the subtree 
 * rooted at Node to queue.
 */
static void 
keys_range(const struct rbtree_node *node, const void *lokey, const void *hikey,
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
	if(cmphi == -1)
		keys_range(node->right, lokey, hikey, kcmp, keys);
}
