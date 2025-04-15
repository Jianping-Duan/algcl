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
#include "queue.h"

#define RBBST_SIZE_NODE(node)	((node) == NULL ? 0 : (node)->size)
#define RBBST_ISRED(node)	((node) == NULL ? 0 : (node)->color == RED)
#define RBBST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

/* Flips the colors of node and its two children */
#define FLIP_COLORS(node)		do {					\
	if(node != NULL) {									\
		node->color = !node->color;						\
		if(node->left != NULL)							\
			node->left->color = !node->left->color;		\
		if(node->right != NULL)							\
			node->right->color = !node->right->color;	\
	}													\
} while(0)

static struct element * get_node(struct rbbst_node *, const char *);
static struct rbbst_node * make_node(const struct element *);
static inline struct rbbst_node * rotate_right(struct rbbst_node *);
static inline struct rbbst_node * rotate_left(struct rbbst_node *);
static inline struct rbbst_node * balance(struct rbbst_node *);
static struct rbbst_node * put_node(struct rbbst_node *,
	const struct element *);
static void postorder_nodes(struct rbbst_node *, void (*)(struct rbbst_node *));
static void release_node(struct rbbst_node *);
static void preorder_nodes(struct rbbst_node *, void (*)(struct element *));
static struct rbbst_node * min_node(struct rbbst_node *);
static struct rbbst_node * max_node(struct rbbst_node *);
static struct rbbst_node * move_red_left(struct rbbst_node *);
static struct rbbst_node * move_red_right(struct rbbst_node *);
static struct rbbst_node * delete_min_node(struct rbbst_node *);
static struct rbbst_node * delete_max_node(struct rbbst_node *);
static struct rbbst_node * delete_node(struct rbbst_node *, const char *);
static int isbst(const struct rbbst_node *, const char *, const char *);
static int is23(const struct rbbst_node *, const struct rbbst_node *);
static inline int isbal_node(const struct rbbst_node *node, int blacks);
static int isbalanced(const struct rbbst_node *);
static int is_size_consistent(const struct rbbst_node *);
static int is_rank_consistent(const struct redblack_bst *);
static struct rbbst_node * floor_node(struct rbbst_node *, const char *);
static struct rbbst_node * ceiling_node(struct rbbst_node *, const char *);
static unsigned long rank_node(const char *, const struct rbbst_node *);
static struct element * select_node(unsigned long, struct rbbst_node *);
static void keys_range(const struct rbbst_node *, const char *, const char *,
	struct queue *);

/* Returns item associated with the given key. */
struct element * 
rbbst_get(const struct redblack_bst *bst, const char *key)
{
	if(key == NULL)
		return NULL;
	return get_node(bst->root, key);
}

/* 
 * Inserts the specified key-value pair into 
 * the Red-Black BST.
 */
void
rbbst_put(struct redblack_bst *bst, const struct element *item)
{
	assert(item != NULL);
	
	bst->root = put_node(bst->root, item);
	bst->root->color = BLACK;
}

/* Release the Red-Black tree */
void
rbbst_clear(struct redblack_bst *bst)
{
	if(!RBBST_ISEMPTY(bst))
		postorder_nodes(bst->root, release_node);
}

/* Preorder traverse. */
void 
rbbst_preorder(const struct redblack_bst *bst, 
			void (*print)(struct element *item))
{
	preorder_nodes(bst->root, print);
}

/* Returns the smallest key in the Red-Black BST. */
char * 
rbbst_min(const struct redblack_bst *bst)
{
	if(RBBST_ISEMPTY(bst))
		return NULL;
	return min_node(bst->root)->item.key;
}

/* Returns the largest key in the Red-Black BST. */
char *
rbbst_max(const struct redblack_bst *bst)
{
	if(RBBST_ISEMPTY(bst))
		return NULL;
	return max_node(bst->root)->item.key;
}

/* 
 * Remove the smallest key associated with value from 
 * this Red-Black BST.
 */
void
rbbst_delete_min(struct redblack_bst *bst)
{
	if(RBBST_ISEMPTY(bst))
		return;
	
	/* 
	 * if both children of root are black,
	 * set root to red. 
	 */
	if(!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	
	bst->root = delete_min_node(bst->root);
	
	if(!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;
}

/* 
 * Remove the largest key associated with value from 
 * this Red-Black BST.
 */
void
rbbst_delete_max(struct redblack_bst *bst)
{
	if(RBBST_ISEMPTY(bst))
		return;
	
	/* 
	 * if both children of root are black, 
	 * set root to red. 
	 */
	if(!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	
	bst->root = delete_max_node(bst->root);
	
	if(!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;
}

/* 
 * Removes the specified key and its associated with value 
 * from this Red-Black BST. 
 */
void
rbbst_delete(struct redblack_bst *bst, const char *key)
{
	if(RBBST_ISEMPTY(bst))
		return;
	
	if(rbbst_get(bst, key) == NULL)
		return;
	
	if(!RBBST_ISRED(bst->root->left) && !RBBST_ISRED(bst->root->right))
		bst->root->color = RED;
	
	bst->root = delete_node(bst->root, key);
	
	if(!RBBST_ISEMPTY(bst))
		bst->root->color = BLACK;
}

/* Check integrity of red-black tree data structure. */
int 
rbbst_check(const struct redblack_bst *bst)
{
	int flag = 1;
	
	if(!isbst(bst->root, NULL, NULL)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if(!is23(bst->root, bst->root)) {
		printf("Not a 2-3 tree.\n");
		flag = 0;
	}
	
	if(!isbalanced(bst->root)) {
		printf("Not balanced.\n");
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

/* 
 * Returns the largest key in the symbol table less than 
 * or equal to Key.
 */
struct element * 
rbbst_floor(const struct redblack_bst *bst, const char *key)
{
	struct rbbst_node *current;
	
	current = floor_node(bst->root, key);
	/* NULL is the specified key to small. */
	return current == NULL ? NULL : &(current->item);	
}

/* 
 * Returns the smallest key in the symbol table 
 * greater than or equal to Key.
 */
struct element * 
rbbst_ceiling(const struct redblack_bst *bst, const char *key)
{
	struct rbbst_node *current;
	
	current = ceiling_node(bst->root, key);
	/* NULL is the specified key to large. */
	return current == NULL ? NULL : &(current->item);
}

/* 
 * Return the number of keys in the Red-Black BST 
 * strictly less than Key.
 */
unsigned long 
rbbst_rank(const struct redblack_bst *bst, const char *key)
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
rbbst_select(const struct redblack_bst *bst, unsigned long rank)
{
	if(rank < RBBST_SIZE(bst))
		return select_node(rank, bst->root);
	return NULL;
}

/* 
 * Returns all keys in the Red-Black BST in 
 * the given range in ascending order. 
 */
void
rbbst_keys(const struct redblack_bst *bst, const char *lokey,
				const char *hikey, struct queue *keys)
{
	keys_range(bst->root, lokey ,hikey, keys);
}

/******************** static function boundary ********************/

/* 
 * Item associated with the given key in subtree 
 * rooted at NODE; if null no search key.
 */
static struct element * 
get_node(struct rbbst_node *node, const char *key)
{
	int cmp;
	struct rbbst_node *proot;
	
	proot = node;
	while(proot != NULL) {
		cmp = strcmp(key, proot->item.key);
		if(cmp < 0)
			proot = proot->left;
		else if(cmp > 0)
			proot = proot->right;
		else
			return &(proot->item);
	}
	return NULL;
}

/* 
 * New create Red-Black BST node using 
 * the specified key-value pair. 
 */
static struct rbbst_node * 
make_node(const struct element *item)
{
	struct rbbst_node *current;
	
	current = (struct rbbst_node *)algmalloc(sizeof(struct rbbst_node));
	
	current->item = *item;
	current->left = NULL;
	current->right = NULL;
	current->color = RED;	/* default red link */
	current->size = 1;
	current->height = 0;
	
	return current;
}

/* Make a left-leaning link lean to the right */
static inline struct rbbst_node * 
rotate_right(struct rbbst_node *hnode)
{
	struct rbbst_node *lnode;
	
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
static inline struct rbbst_node * 
rotate_left(struct rbbst_node *hnode)
{
	struct rbbst_node *lnode;
	
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
static inline struct rbbst_node * 
balance(struct rbbst_node *hnode)
{
	if(RBBST_ISRED(hnode->right) &&
		!RBBST_ISRED(hnode->left))
		hnode = rotate_left(hnode);
	if(RBBST_ISRED(hnode->left) &&
		RBBST_ISRED(hnode->left->left))
		hnode = rotate_right(hnode);
	if(RBBST_ISRED(hnode->left) &&
		RBBST_ISRED(hnode->right))
		FLIP_COLORS(hnode);
	
	hnode->size = RBBST_SIZE_NODE(hnode->left) + 
		RBBST_SIZE_NODE(hnode->right) + 1;
	hnode->height = 1 + MAX(RBBST_HEIGHT_NODE(hnode->left), 
		RBBST_HEIGHT_NODE(hnode->right));
	
	return hnode;
}

/* 
 * Insert the key-value pair in 
 * the subtree rooted as HNODE.
 */
static struct rbbst_node * 
put_node(struct rbbst_node *hnode,
		const struct element *item)
{
	int cmp;
	
	if(hnode == NULL)
		return make_node(item);
	
	cmp = strcmp(item->key, hnode->item.key);
	if(cmp < 0)
		hnode->left = put_node(hnode->left, item);
	else if(cmp > 0)
		hnode->right = put_node(hnode->right, item);
	else
		hnode->item.value = item->value;
	
	return balance(hnode);
}

static void 
postorder_nodes(struct rbbst_node *root, 
				void (*release)(struct rbbst_node *node))
{
	if(root != NULL) {
		postorder_nodes(root->left, release);
		postorder_nodes(root->right, release);
		(*release)(root);
	}
}

static inline void 
release_node(struct rbbst_node *node)
{
	ALGFREE(node);
}

static void
preorder_nodes(struct rbbst_node *root, 
			void (*print)(struct element *item))
{
	if(root != NULL) {
		(*print)(&(root->item));
		preorder_nodes(root->left, print);
		preorder_nodes(root->right, print);
	}
}

/* 
 * The smallest key in the subtree rooted at NODE; 
 * null if no search key.
 */
static struct rbbst_node * 
min_node(struct rbbst_node *node)
{
	if(node->left == NULL)
		return node;
	return min_node(node->left);
}

/* 
 * The largest key in the subtree rooted at NODE;
 * null if no search key.
 */
static struct rbbst_node * 
max_node(struct rbbst_node *node)
{
	if(node->right == NULL)
		return node;
	return max_node(node->right);
}

/* 
 * Assuming that hnode is red and both hnode->left and 
 * hnode->left->left are black, make hnode->left or 
 * its children red.
 */
static struct rbbst_node * 
move_red_left(struct rbbst_node *hnode)
{
	FLIP_COLORS(hnode);
	if(hnode != NULL && hnode->right != NULL && 
	   RBBST_ISRED(hnode->right->left)) {
		hnode->right = rotate_right(hnode->right);
		hnode = rotate_left(hnode);
		FLIP_COLORS(hnode);
	}
	
	return hnode;
}

/* 
 * Assuming that hnode is red and both hnode->right and 
 * hnode->right->left are black, make hnode->right or 
 * its children red.
 */
static struct rbbst_node * 
move_red_right(struct rbbst_node *hnode)
{
	FLIP_COLORS(hnode);
	if(RBBST_ISRED(hnode->left) && 
		RBBST_ISRED(hnode->left->left)) {
		hnode = rotate_right(hnode);
		FLIP_COLORS(hnode);
	}
	
	return hnode;
}

/* 
 * Delete the key-value pair with 
 * the minimum key rooted at NODE.
 */
static struct rbbst_node * 
delete_min_node(struct rbbst_node *node)
{	
	if(node->left == NULL) {
		release_node(node);
		return NULL;
	}

	/* node is 2-node */
	if(!RBBST_ISRED(node->left) &&
		!RBBST_ISRED(node->left->left))
		node = move_red_left(node);
		
	node->left = delete_min_node(node->left);
	
	return balance(node);
}

/* 
 * Delete the key-value pair with 
 * the maximum key rooted at NODE.
 */
static struct rbbst_node * 
delete_max_node(struct rbbst_node *node)
{
	if(RBBST_ISRED(node->left))		/* node is 3-node */
		node = rotate_right(node);
		
	if(node->right == NULL) {
		release_node(node);
		return NULL;
	}

	/* left node is 2-node */
	if(!RBBST_ISRED(node->right) &&
		!RBBST_ISRED(node->right->left))
		node = move_red_right(node);
		
	node->right = delete_max_node(node->right);
	
	return balance(node);
}

/* 
 * Delete the key-value pair with 
 * the given key rooted at NODE.
 */
static struct rbbst_node * 
delete_node(struct rbbst_node *node, const char *key)
{
	struct rbbst_node *minnode;
	
	if(node == NULL)
		return NULL;
	
	if(strcmp(key, node->item.key) < 0) {
		if(!RBBST_ISRED(node->left) && 
			!RBBST_ISRED(node->left->left))
			node = move_red_left(node);
		node->left = delete_node(node->left, key);
	} 
	else {
		if(RBBST_ISRED(node->left))
			node = rotate_right(node);
	
		/* may max key */
		if(strcmp(key, node->item.key) == 0 &&
			node->right == NULL) {
			release_node(node);
			return NULL;
		}
		
		if(!RBBST_ISRED(node->right) && 
			!RBBST_ISRED(node->right->left))
			node = move_red_right(node);
		
		if(strcmp(key, node->item.key) == 0) {
			minnode = min_node(node->right);
			/* coping delete */
			node->item = minnode->item;
			node->right = delete_min_node(node->right);
		}
		else
			node->right = delete_node(node->right, key);
	}
	
	return balance(node);
}

/* 
 * Is the tree rooted at Node a BST with all keys 
 * strictly between min and max (if min or max is null,
 * treat as empty constraint).
 */
static int
isbst(const struct rbbst_node *node, const char *minkey, const char *maxkey)
{
	if(node == NULL)
		return 1;	/* empty tree */
	
	if(minkey != NULL && strcmp(node->item.key, minkey) <= 0)
		return 0;
	if(maxkey != NULL && strcmp(node->item.key, maxkey) >= 0)
		return 0;
	return isbst(node->left, minkey, node->item.key) && 
		isbst(node->right, node->item.key, maxkey);
}

/* 
 * Does the tree have no red right links, 
 * and at most one (left) red links in a row on any path? 
 */
static int
is23(const struct rbbst_node *root, const struct rbbst_node *node)
{
	if(node == NULL)
		return 1;
	if(RBBST_ISRED(node->right))
		return 0;
	if(root != node && RBBST_ISRED(node) && RBBST_ISRED(node->left))
		return 0;
	
	return is23(root, node->left) && is23(root, node->right);
}

/* 
 * Does every path from the root to a leaf have 
 * he given number of black links? 
 */
static inline int 
isbal_node(const struct rbbst_node *node, int blacks)
{
	if(node == NULL)
		return blacks == 0;
	if(!RBBST_ISRED(node))
		blacks--;
	return isbal_node(node->left, blacks) && 
		isbal_node(node->right, blacks);
}

/* 
 * Do all paths from root to leaf have same 
 * number of black links?
 */
static int 
isbalanced(const struct rbbst_node *root)
{
	const struct rbbst_node *proot;
	int blacks = 0;	/* number of black links on path from root to min */
	
	proot = root;
	while(proot != NULL) {
		if(!RBBST_ISRED(proot))
			blacks++;
		proot = proot->left;
	}
	
	return isbal_node(root, blacks);
}

/* Are the size of fields correct ? */
static int 
is_size_consistent(const struct rbbst_node *node)
{
	if(node == NULL)
		return 1;	/* empty consistent */
	if(RBBST_SIZE_NODE(node) != RBBST_SIZE_NODE(node->left) + 
		RBBST_SIZE_NODE(node->right) + 1) {
		return 0;
	}
	return is_size_consistent(node->left) && 
		is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int 
is_rank_consistent(const struct redblack_bst *bst)
{
	unsigned long i;
	char *key;
	struct element *el;
	struct queue keys;
	
	for(i = 0; i < RBBST_SIZE(bst); i++) {
		el = rbbst_select(bst, i);
		if(i != rbbst_rank(bst, el->key))
			return 0;
	}
	
	QUEUE_INIT(&keys, 0);
	rbbst_keys(bst, rbbst_min(bst), rbbst_max(bst), &keys);
	while(!QUEUE_ISEMPTY(&keys)) {
		dequeue(&keys, (void **)&key);
		el = rbbst_select(bst, rbbst_rank(bst, key));
		if(strcmp(key, el->key) != 0)
			return 0;
	}
	queue_clear(&keys);
	
	return 1;
}

/* 
 * The largest key in the subtree rooted at Node less than 
 * or equal to the given key.
 */
static struct rbbst_node * 
floor_node(struct rbbst_node *node, const char *key)
{
	int cmp;
	struct rbbst_node *rnode;
	
	if(node == NULL)	/* not found the specified key */
		return NULL;

	if((cmp = strcmp(key, node->item.key)) == 0)
		return node;
	if(cmp < 0)
		return floor_node(node->left, key);
	if((rnode = floor_node(node->right, key)) != NULL)
		return rnode;
	else
		return node;
}

/* 
 * The smallest key in the subtree rooted at Node 
 * greater than or equal to the given key.
 */
static struct rbbst_node * 
ceiling_node(struct rbbst_node *node, const char *key)
{
	int cmp;
	struct rbbst_node *lnode;
	
	if(node == NULL)
		return NULL;
	
	if((cmp = strcmp(key, node->item.key)) == 0)
		return node;
	if(cmp > 0)
		return ceiling_node(node->right, key);
	if((lnode = ceiling_node(node->left, key)) != NULL)
		return lnode;
	else
		return node;
}

/* 
 * Number of keys less than key in 
 * the subtree rooted at Node.
 */
static unsigned long 
rank_node(const char *key, const struct rbbst_node *node)
{
	int cmp;
	
	if(node == NULL)
		return 0;
	
	if((cmp = strcmp(key, node->item.key)) < 0)
		return rank_node(key, node->left);
	if(cmp > 0)
		return 1 + RBBST_SIZE_NODE(node->left) + 
			rank_node(key, node->right);
	else
		return RBBST_SIZE_NODE(node->left);
}

/* 
 * Return key in BST rooted at Node of given rank.
 * Precondition: rank is in legal range. 
 */
static struct element * 
select_node(unsigned long rank, struct rbbst_node *node)
{
	unsigned long leftsize;
	
	if(node == NULL)
		return NULL;
	
	leftsize = RBBST_SIZE_NODE(node->left);
	if(rank < leftsize)
		return select_node(rank, node->left);
	if(rank > leftsize)
		return select_node(rank - leftsize - 1,
			node->right);
	else
		return &(node->item);
}

/* 
 * Add the keys between lokey and hikey in the subtree 
 * rooted at Node to queue.
 */
static void 
keys_range(const struct rbbst_node *node, const char *lokey,
		const char *hikey, struct queue *qp)
{
	int cmplo, cmphi;
	
	if(node == NULL)
		return;
	
	cmplo = strcmp(lokey, node->item.key);
	cmphi = strcmp(hikey, node->item.key);
	
	if(cmplo < 0)
		keys_range(node->left, lokey, hikey, qp);
	if(cmplo <= 0 && cmphi >= 0)
		enqueue(qp, node->item.key);
	if(cmphi > 0)
		keys_range(node->right, lokey, hikey, qp);
}
