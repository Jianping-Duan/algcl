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
#include "queue.h"

/* Returns the number of nodes in the subtree. */
#define AVLBST_SIZE_NODE(node)	((node) == NULL ? 0 : (node)->size)

/* Returns the height of the subtree */
#define AVLBST_HEIGHT_NODE(node)	((node) == NULL ? (-1) : (node)->height)

/* 
 * Returns the balance factor of the subtree. 
 * The balance factor is defined as the difference 
 * in height of the left subtree and right subtree, 
 * in this order. Therefore, a subtree with a balance 
 * factor of -1, 0 or 1 has the AVL property since the 
 * heights of the two child subtrees differ by 
 * at most one. 
 */
#define BALANCE_FACTOR(node)	\
	(AVLBST_HEIGHT_NODE((node)->left) - AVLBST_HEIGHT_NODE((node)->right))

static struct avl_node * get_node(struct avl_node *, const char *);
static struct avl_node * make_node(const struct element *);
static inline struct avl_node * rotate_right(struct avl_node *);
static inline struct avl_node * rotate_left(struct avl_node *);
static struct avl_node * balance(struct avl_node *);
static struct avl_node * put_node(struct avl_node *, const struct element *);
static void postorder_nodes(struct avl_node *, void (*)(struct avl_node *));
static inline void release_node(struct avl_node *);
static void preorder_nodes(const struct avl_node *, struct queue *);
static struct avl_node * min_node(struct avl_node *);
static struct avl_node * max_node(struct avl_node *);
static struct avl_node * delete_min_node(struct avl_node *);
static struct avl_node * delete_min(struct avl_node *);
static struct avl_node * delete_max_node(struct avl_node *);
static struct avl_node * delete_node(struct avl_node *, const char *);
static struct avl_node * floor_node(struct avl_node *, const char *);
static struct avl_node * ceiling_node(struct avl_node *, const char *);
static unsigned long rank_node(const char *, const struct avl_node *);
static struct element * select_node(unsigned long, struct avl_node *);
static void keys_range(const struct avl_node *, const char *, const char *,
	struct queue *);
static int isbst(const struct avl_node *, const char *,	const char *);
static int isavl(const struct avl_node *node);
static int is_size_consistent(const struct avl_node *node);
static int is_rank_consistent(const struct avl_tree *bst);

/* 
 * Returns element associated with the given key 
 * in the AVL tree. 
 */
struct element * 
avlbst_get(const struct avl_tree *avl, const char *key)
{
	struct avl_node *node;
	
	if(key == NULL)
		return NULL;
	
	if((node = get_node(avl->root, key)) == NULL)
		return NULL;
	return &(node->item);
}

/* 
 * Inserts the specified key-value pair into the AVL tree. 
 */
void 
avlbst_put(struct avl_tree *avl, const struct element *item)
{
	if(item == NULL)
		return;
	
	avl->root = put_node(avl->root, item);
}

/* Releases the AVL tree. */
void 
avlbst_clear(struct avl_tree *avl)
{
	if(!AVLBST_ISEMPTY(avl))
		postorder_nodes(avl->root, release_node);
}

void 
avlbst_preorder(const struct avl_tree *avl, struct queue *keys)
{
	preorder_nodes(avl->root, keys);
}

/* Returns the smallest key in the AVL BST */
char * 
avlbst_min(const struct avl_tree *avl)
{
	if(AVLBST_ISEMPTY(avl))
		return NULL;
	return min_node(avl->root)->item.key;
}

/* Returns the largest key in the AVL BST */
char *
avlbst_max(const struct avl_tree *avl)
{
	if(AVLBST_ISEMPTY(avl))
		return NULL;
	return max_node(avl->root)->item.key;
}

/* 
 * Removes the smallest key and associated with value 
 * from this AVL tree. 
 */
void 
avlbst_delete_min(struct avl_tree *avl)
{
	if(!AVLBST_ISEMPTY(avl))
		avl->root = delete_min_node(avl->root);
}

/* 
 * Removes the largest key and associated with value 
 * from this AVL tree. 
 */
void 
avlbst_delete_max(struct avl_tree *avl)
{
	if(!AVLBST_ISEMPTY(avl))
		avl->root = delete_max_node(avl->root);
}

/* 
 * Removes the specified key and its associated value 
 * from the AVL tree. 
 */
void 
avlbst_delete(struct avl_tree *avl, const char *key)
{
	if(AVLBST_ISEMPTY(avl))
		return;
	
	if(avlbst_get(avl, key) == NULL)
		return;
	
	avl->root = delete_node(avl->root, key);
}

/* 
 * Returns the largest key in the AVL tree 
 * less than or equal to Key.
 */
struct element * 
avlbst_floor(const struct avl_tree *avl, const char *key)
{
	struct avl_node *current;
	
	current = floor_node(avl->root, key);
	return current == NULL ? NULL : &(current->item);
}

/* 
 * Returns the smallest key in the AVL tree 
 * greater than or equal to Key.
 */
struct element * 
avlbst_ceiling(const struct avl_tree *avl, const char *key)
{
	struct avl_node *current;
	
	current = ceiling_node(avl->root, key);
	return current == NULL ? NULL : &(current->item);
}

/* 
 * Returns the number of keys in the AVL BST 
 * strictly less than Key.
 */
unsigned long 
avlbst_rank(const struct avl_tree *avl, const char *key)
{
	return rank_node(key, avl->root);
}

/* 
 * Return the key in the AVL BST of a given rank.
 * This key has the property that there are rank 
 * keys in the AVL BST that are smaller. 
 * In other words, this key is the 
 * (rank+1)st smallest key in the AVL BST.
 */
struct element * 
avlbst_select(const struct avl_tree *avl, unsigned long rank)
{
	if(rank < AVLBST_SIZE(avl))
		return select_node(rank, avl->root);
	return NULL;
}

/* Returns all keys in the AVL tree in the given range. */
void
avlbst_keys(const struct avl_tree *avl, const char *lokey, const char *hikey,
		struct queue *keys)
{
	keys_range(avl->root, lokey, hikey, keys);
}

void 
avlbst_breadth_first(const struct avl_tree *avl, struct queue *keys)
{
	struct queue qp;
	struct avl_node *proot;
	
	if(AVLBST_ISEMPTY(avl))
		return;
	
	QUEUE_INIT(&qp, 0);
	proot = avl->root;
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

/* Check integrity of AVL tree data structure. */
int
avlbst_check(const struct avl_tree *avl)
{
	int flag = 1;
	
	if(!isbst(avl->root, NULL, NULL)) {
		printf("Not in symmetric order.\n");
		flag = 0;
	}
	
	if(!isavl(avl->root)) {
		printf("Not a avl tree.\n");
		flag = 0;
	}
	
	if(!is_size_consistent(avl->root)) {
		printf("Subtree counts not consistent.\n");
		flag = 0;
	}
	
	if(!is_rank_consistent(avl)) {
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
get_node(struct avl_node *node, const char *key)
{
	int cmp;
	
	if(node == NULL)
		return NULL;	/* not found key */
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		return get_node(node->left, key);
	else if(cmp > 0)
		return get_node(node->right, key);
	else
		return node;
}

/* 
 * Make the AVL node for AVL tree using 
 * the specified item.
 */
static struct avl_node * 
make_node(const struct element *item)
{
	struct avl_node *current;
	
	current = (struct avl_node *)algmalloc(sizeof(struct avl_node));
	
	current->item = *item;
	current->left = NULL;
	current->right = NULL;
	current->height = 0;	/* just one node height is 0 */
	current->size = 1;
	
	return current;
}

/* 
 * Rotates the given subtree to the right, meanwhile, 
 * updates the size and height of subtree. 
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
 * Rotates the given subtree to the left, meanwhile, 
 * updates the size and height of subtree. 
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

/* Restores the AVL tree property of the subtree. */
static struct avl_node * 
balance(struct avl_node *node)
{
	if(BALANCE_FACTOR(node) < -1) {
		if(BALANCE_FACTOR(node->right) > 0)
			node->right = rotate_right(node->right);
		node = rotate_left(node);
	} else if(BALANCE_FACTOR(node) > 1) {
		if(BALANCE_FACTOR(node->left) < 0)
			node->left = rotate_left(node->left);
		node = rotate_right(node);
	}
	
	return node;
}

/* 
 * Inserts the key-value pair in the subtree. 
 * It overrides the old value with the new value 
 * if the AVL tree already contains the specified key. 
 */
static struct avl_node * 
put_node(struct avl_node *node, const struct element *item)
{
	int cmp;
	
	if(node == NULL)
		return make_node(item);
	
	cmp = strcmp(item->key, node->item.key);
	if(cmp < 0)
		node->left = put_node(node->left, item);
	else if(cmp > 0)
		node->right = put_node(node->right, item);
	else {
		node->item.value = item->value;
		return node;
	}
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

static void 
postorder_nodes(struct avl_node *root, 
			void (*release)(struct avl_node *node))
{
	if(root != NULL) {
		postorder_nodes(root->left, release);
		postorder_nodes(root->right, release);
		(*release)(root);
	}
}

static inline void
release_node(struct avl_node *node)
{
	ALGFREE(node);
}

static void 
preorder_nodes(const struct avl_node *root, struct queue *keys)
{
	if(root != NULL) {
		enqueue(keys, root->item.key);
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
	if(node->left == NULL)
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
	if(node->right == NULL)
		return node;
	return max_node(node->right);
}

/* 
 * Delete the key-value pair with 
 * the minimum key rooted at Node. 
 */
static struct avl_node * 
delete_min_node(struct avl_node *node)
{
	struct avl_node *current;
	
	if(node->left == NULL) {
		current = node->right;
		release_node(node);
		return current;
	}
	
	node->left = delete_min_node(node->left);
	
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
	
	if(node->left == NULL) {
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

/* 
 * Delete the key-value pair with 
 * the maximum key rooted at Node. 
 */
static struct avl_node * 
delete_max_node(struct avl_node *node)
{
	struct avl_node *current;
	
	if(node->right == NULL) {
		current = node->left;
		release_node(node);
		return current;
	}
	
	node->right = delete_max_node(node->right);
	
	node->size = 1 + AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right);
	node->height = 1 + MAX(AVLBST_HEIGHT_NODE(node->left), 
		AVLBST_HEIGHT_NODE(node->right));
	
	return balance(node);
}

/* 
 * Delete the key-value pair with 
 * the given key rooted at Node. 
 */
static struct avl_node * 
delete_node(struct avl_node *node, const char *key)
{
	int cmp;
	struct avl_node *current;
	
	if(node == NULL)
		return NULL;
	
	cmp = strcmp(key, node->item.key);
	if(cmp < 0)
		node->left = delete_node(node->left, key);
	else if(cmp > 0)
		node->right = delete_node(node->right, key);
	else {
		if(node->left == NULL) {
			current = node->right;
			release_node(node);
			return current;
		} 
		else if(node->right == NULL) {
			current = node->left;
			release_node(node);
			return current;
		}
		else {
			current = node;
			node = min_node(current->right);
			node->right = delete_min(current->right);
			node->left = current->left;
			release_node(current);
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
floor_node(struct avl_node *node, const char *key)
{
	int cmp;
	struct avl_node *rnode;
	
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
static struct avl_node * 
ceiling_node(struct avl_node *node, const char *key)
{
	int cmp;
	struct avl_node *lnode;
	
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
 * Number of keys less than the given key 
 * in the subtree rooted at Node.
 */
static unsigned long 
rank_node(const char *key, const struct avl_node *node)
{
	int cmp;
	
	if(node == NULL)
		return 0;
	
	if((cmp = strcmp(key, node->item.key)) < 0)
		return rank_node(key, node->left);
	if(cmp > 0)
		return 1 + AVLBST_SIZE_NODE(node->left) + 
			rank_node(key, node->right);
	else
		return AVLBST_SIZE_NODE(node->left);
}

/* 
 * Return key in BST rooted at Node of given rank.
 * Precondition: rank is in legal range. 
 */
static struct element * 
select_node(unsigned long rank, struct avl_node *node)
{
	unsigned long leftsize;
	
	if(node == NULL)
		return NULL;
	
	leftsize = AVLBST_SIZE_NODE(node->left);
	if(rank < leftsize)
		return select_node(rank, node->left);
	if(rank > leftsize)
		return select_node(rank - leftsize - 1,
			node->right);
	else
		return &(node->item);
}

/* 
 * Adds the keys between Lokey and Hikey in the subtree 
 * rooted at Node to the Queue. 
 */
static void 
keys_range(const struct avl_node *node, const char *lokey,
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

/* 
 * Checks if the tree rooted at Note is a BST with all 
 * keys strictly between min and max 
 * (if min or max is null, treat as empty constraint).
 */
static int 
isbst(const struct avl_node *node, const char *minkey, const char *maxkey)
{
	if(node == NULL)
		return 1;		/* empty constraint */
	
	if(minkey != NULL && strcmp(node->item.key, minkey) < 0)
		return 0;
	if(maxkey != NULL && strcmp(node->item.key, maxkey) > 0)
		return 0;
	
	return isbst(node->left, minkey, node->item.key) && 
	   isbst(node->right, node->item.key, maxkey);
}

/* Checks if AVL property is consistent in the subtree. */
static int 
isavl(const struct avl_node *node)
{
	int bf;
	
	if(node == NULL)
		return 1;
	
	bf = BALANCE_FACTOR(node);
	if(bf < -1 || bf > 1)
		return 0;
	
	return isavl(node->left) && isavl(node->right);
}

/* are the size fields correct? */
static int 
is_size_consistent(const struct avl_node *node)
{
	if(node == NULL)
		return 1;		/* treat as empty constraint */
	if(node->size != AVLBST_SIZE_NODE(node->left) + 
		AVLBST_SIZE_NODE(node->right) + 1) {
		return 0;
	}
	return is_size_consistent(node->left) && 
		is_size_consistent(node->right);
}

/* check that ranks are consistent */
static int 
is_rank_consistent(const struct avl_tree *bst)
{
	unsigned long i;
	char *key;
	struct queue qu;
	struct element *el;
	
	for(i = 0; i < AVLBST_SIZE(bst); i++) {
		el = avlbst_select(bst, i);
		if(i != avlbst_rank(bst, el->key))
			return 0;
	}
	
	QUEUE_INIT(&qu, 0);
	avlbst_keys(bst, avlbst_min(bst), avlbst_max(bst), &qu);
	while(!QUEUE_ISEMPTY(&qu)) {
		dequeue(&qu, (void **)&key);
		el = avlbst_select(bst, avlbst_rank(bst, key));
		if(strcmp(key, el->key) != 0)
			return 0;
	}
	
	queue_clear(&qu);

	return 1;
}
