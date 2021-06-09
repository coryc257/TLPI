/*
 * tree.c
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */


#include "lib/tree.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int
initialize(TREE *tree)
{
	pthread_mutexattr_t attr;
	int status;

	tree->error = 0;

	status = pthread_mutexattr_init(&attr);
	if (status != 0)
	{
		tree->error = status;
		return -1;
	}

	status = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	if (status != 0)
	{
		tree->error = status;
		return -1;
	}

	tree->root_lock = malloc(sizeof(pthread_mutex_t));
	if (tree->root_lock == NULL)
	{
		tree->error = ENOMEM;
		return -1;
	}

	status = pthread_mutex_init(tree->root_lock,&attr);
	if (status != 0)
	{
		tree->error = status;
		return -1;
	}

	tree->base = NULL;
	return 0;

}

static int
new_tree_node(char *key, void *value, TREE_ITEM **new)
{
	int status;
	pthread_mutexattr_t attr;
	if((status = pthread_mutexattr_init(&attr)) != 0)
		return status;
	if((pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_DEFAULT)) != 0)
		return status;
	if ((*new = malloc(sizeof(TREE_ITEM))) == NULL)
		return ENOMEM;

	if(((*new)->key = strdup(key)) == NULL)
	{
		free(*new);
		*new = NULL;
		return ENOMEM;
	}
	(*new)->value = value;
	(*new)->left = NULL;
	(*new)->right = NULL;

	if (((*new)->lock_left = malloc(sizeof(pthread_mutex_t))) == NULL) {
		free(*new);
		*new = NULL;
		return ENOMEM;
	}
	if (((*new)->lock_right = malloc(sizeof(pthread_mutex_t))) == NULL) {
		free((*new)->lock_left);
		free(*new);
		*new = NULL;
		return ENOMEM;
	}
	if (((*new)->lock_look = malloc(sizeof(pthread_mutex_t))) == NULL) {
		free((*new)->lock_left);
		free((*new)->lock_right);
		free(*new);
		*new = NULL;
		return ENOMEM;
	}
	if ((status = pthread_mutex_init((*new)->lock_left,&attr)) != 0) {
		free((*new)->lock_left);
		free((*new)->lock_right);
		free((*new)->lock_look);
		free(*new);
		*new = NULL;
		return status;
	}
	if ((status = pthread_mutex_init((*new)->lock_right,&attr)) != 0) {
		free((*new)->lock_left);
		free((*new)->lock_right);
		free((*new)->lock_look);
		free(*new);
		*new = NULL;
		return status;
	}
	if ((status = pthread_mutex_init((*new)->lock_look,&attr)) != 0) {
		free((*new)->lock_left);
		free((*new)->lock_right);
		free((*new)->lock_look);
		free(*new);
		*new = NULL;
		return status;
	}
	return 0;
}

static TF_TRAMPOLINE
t_emplace(TREE_ITEM **item, TREE_ITEM *new, int *status)
{
	void destroy_item(TREE_ITEM *new) {
		free(new->key);
		pthread_mutex_destroy(new->lock_left);
		free(new->lock_left);
		pthread_mutex_destroy(new->lock_right);
		free(new->lock_right);
		pthread_mutex_destroy(new->lock_look);
		free(new->lock_look);
		free(new);
	}
	TREE_ITEM *node, *swap;
	node = *item;
	*status = 0;
	int cmp;

	cmp = strcmp((new->key), (node->key));
	if (cmp > 0)
		cmp = 1;
	else if (cmp < 0)
		cmp = -1;

	switch (cmp) {
	case 0:
		node->value = new->value;
		pthread_mutex_unlock(node->lock_look);
		destroy_item(new);
		return NULL;
	case 1:
		pthread_mutex_lock(node->lock_right);
		if (node->right == NULL)
		{
			node->right = new;
			pthread_mutex_unlock(node->lock_right);
			pthread_mutex_unlock(node->lock_look);
			return NULL;
		}
		swap = node->right;
		pthread_mutex_unlock(node->lock_look);
		pthread_mutex_lock(swap->lock_look);
		pthread_mutex_unlock(node->lock_right);
		*item = swap;
		break;
	case -1:
		pthread_mutex_lock(node->lock_left);
		if (node->left == NULL)
		{
			node->left = new;
			pthread_mutex_unlock(node->lock_left);
			pthread_mutex_unlock(node->lock_look);
			return NULL;
		}
		swap = node->left;
		pthread_mutex_unlock(node->lock_look);
		pthread_mutex_lock(swap->lock_look);
		pthread_mutex_unlock(node->lock_left);
		*item = swap;
		break;
	}

	return (TF_TRAMPOLINE)&t_emplace;
}

static int
emplace(TREE *tree, char *key, void *value)
{

	TREE_ITEM *node, *new;
	int status;
	TF_TRAMPOLINE tramp;

	if ((status = new_tree_node(key,value,&new)) != 0)
	{
		pthread_mutex_unlock(tree->root_lock);
		return status;
	}
	tramp = (TF_TRAMPOLINE)&t_emplace;
	node = tree->base;
	pthread_mutex_lock(tree->base->lock_look);
	pthread_mutex_unlock(tree->root_lock);

	while ((tramp = (*tramp)(&node,new,&status)) != NULL){}
	/*while (tramp) {
		tramp = (*tramp)(&node,new,&status);
	}*/
	return status;
}

int
add(TREE *tree, char *key, void *value)
{
	void destroy_unlock(int status, TREE *tree) {
		free(tree->base->lock_left);
		free(tree->base->lock_right);
		free(tree->base->lock_look);
		free(tree->base);
		tree->base = NULL;
		tree->error = status;
		pthread_mutex_unlock(tree->root_lock);
	}
	int status;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_lock(tree->root_lock);

	if(tree->base == NULL)
	{
		tree->base = malloc(sizeof(TREE_ITEM));
		if (tree->base == NULL)
		{
			tree->error = ENOMEM;
			pthread_mutex_unlock(tree->root_lock);
			return -1;
		}


		if((tree->base->lock_left = malloc(sizeof(pthread_mutex_t))) == NULL)
		{
			free(tree->base);
			tree->base = NULL;
			tree->error = ENOMEM;
			pthread_mutex_unlock(tree->root_lock);
			return -1;
		}
		if ((tree->base->lock_right = malloc(sizeof(pthread_mutex_t))) == NULL)
		{
			free(tree->base->lock_left);
			free(tree->base);
			tree->error = ENOMEM;
			tree->base = NULL;
			pthread_mutex_unlock(tree->root_lock);
			return -1;
		}
		if ((tree->base->lock_look = malloc(sizeof(pthread_mutex_t))) == NULL)
		{
			free(tree->base->lock_left);
			free(tree->base->lock_right);
			free(tree->base);
			tree->error = ENOMEM;
			tree->base = NULL;
			pthread_mutex_unlock(tree->root_lock);
			return -1;
		}

		if ((status = pthread_mutex_init(tree->base->lock_left,&attr)) != 0)
		{
			destroy_unlock(status, tree);
			return -1;
		}
		if ((status = pthread_mutex_init(tree->base->lock_right,&attr)) != 0)
		{
			destroy_unlock(status, tree);
			return -1;
		}
		if ((status = pthread_mutex_init(tree->base->lock_look,&attr)) != 0)
		{
			destroy_unlock(status, tree);
			return -1;
		}


		tree->base->key = strdup(key);
		tree->base->value = value;
		pthread_mutex_unlock(tree->root_lock);
		return 0;
	}

	return emplace(tree, key, value);
}

Boolean peek(char *key, char *key2)
{
	return (strcmp(key,key2) == 0 ? TRUE : FALSE);
}


static T_LOOKUP *
t_sub(T_LOOKUP *look)
{
	int cmp;
	TREE_ITEM *swap;
	cmp = strcmp(look->key,look->node->key);
	if (cmp > 0)
		cmp = 1;
	else if (cmp < 0)
		cmp = -1;

	switch (cmp) {
	case 0:
		look->value = look->node->value;
		pthread_mutex_unlock(look->node->lock_look);
		look->callback = NULL;
		break;
	case 1:
		pthread_mutex_lock(look->node->lock_right);
		if (look->node->right == NULL)
		{
			look->callback = NULL;
			pthread_mutex_unlock(look->node->lock_right);
			pthread_mutex_unlock(look->node->lock_look);
			break;
		}
		swap = look->node->right;
		pthread_mutex_unlock(look->node->lock_look);
		pthread_mutex_lock(swap->lock_look);
		pthread_mutex_unlock(look->node->lock_right);
		look->node = swap;
		break;
	case -1:
		pthread_mutex_lock(look->node->lock_left);
		if (look->node->left == NULL)
		{
			look->callback = NULL;
			pthread_mutex_unlock(look->node->lock_left);
			pthread_mutex_unlock(look->node->lock_look);
			break;
		}
		swap = look->node->left;
		pthread_mutex_unlock(look->node->lock_look);
		pthread_mutex_lock(swap->lock_look);
		pthread_mutex_unlock(look->node->lock_left);
		look->node = swap;
		break;
	}
	return look;
}

static T_LOOKUP *
node_hop(T_LOOKUP *node)
{
	LOCK_LIST *item;
	item = malloc(sizeof(LOCK_LIST));
	item->lock = node->node->lock_right;

	if(node->mutex_chain != NULL)
		item->next = node->mutex_chain;
	node->mutex_chain = item;

	pthread_mutex_lock(item->lock);

	if (node->node->right == NULL)
	{
		node->node->right = node->emplacement;
		node->callback = NULL;
		return node;
	}
	else
	{
		item = malloc(sizeof(LOCK_LIST));
		item->lock = node->node->right->lock_look;
		item->next = node->mutex_chain;
		node->mutex_chain = item;
		pthread_mutex_lock(item->lock);
		node->node = node->node->right;
		return node;
	}
}

static TREE_ITEM *
node_delete(TREE_ITEM *node)
{
	int cmp;
	T_LOOKUP *look;
	LOCK_LIST *lk, *sk;
	pthread_mutex_lock(node->lock_left);
	pthread_mutex_lock(node->lock_right);
	// WHY? because I CAN
	// Once I lock them once, nobody else
	// can because I'm locking the looker
	pthread_mutex_unlock(node->lock_left);
	pthread_mutex_unlock(node->lock_right);
	if (node->left != NULL)
	{
		pthread_mutex_lock(node->left->lock_look);
	}
	if (node->right != NULL)
	{
		pthread_mutex_lock(node->right->lock_look);
	}

	if (node->left == NULL && node->right != NULL)
	{
		pthread_mutex_unlock(node->right->lock_look);
		return node->right;
	}

	if (node->right == NULL && node->left != NULL)
	{
		pthread_mutex_unlock(node->left->lock_look);
		return node->left;
	}

	if (node->right == NULL && node->left == NULL)
		goto node_delete_out;

	look = malloc(sizeof(T_LOOKUP));
	look->callback = &node_hop;
	look->emplacement = node->right;
	look->node = node->left;

	while (look->callback != NULL && (look == look->callback(look))){}
	if (look->mutex_chain != NULL) {
		lk = look->mutex_chain;
		while(lk) {
			pthread_mutex_unlock(lk->lock);
			sk = lk;
			lk = lk->next;
			free(sk);
		}
	}
	node_delete_out:
	if (node->left != NULL)
		pthread_mutex_unlock(node->left->lock_look);
	if (node->right != NULL)
		pthread_mutex_unlock(node->right->lock_look);
	return node->left;
}

static T_LOOKUP *
d_sub(T_LOOKUP *look)
{
	int cmp;
	TREE_ITEM *swap;
	cmp = strcmp(look->key,look->node->key);
	if (cmp > 0)
		cmp = 1;
	else if (cmp < 0)
		cmp = -1;

	switch (cmp) {
	case 0:
		*(look->insertion_point) = node_delete(look->node);
		pthread_mutex_unlock(look->node->lock_look);
		if (look->barrier != NULL)
		{
			pthread_mutex_unlock(look->barrier);
			look->barrier = NULL;
		}
		look->callback = NULL;
		break;
	case 1:
		pthread_mutex_lock(look->node->lock_right);
		if (look->node->right == NULL)
		{
			look->callback = NULL;
			pthread_mutex_unlock(look->node->lock_right);
			pthread_mutex_unlock(look->node->lock_look);
			break;
		}
		swap = look->node->right;
		pthread_mutex_lock(swap->lock_look);
		if (look->barrier != NULL)
			pthread_mutex_unlock(look->barrier);
		if (peek(swap->key,look->key)==FALSE) {
			look->barrier = NULL;
			pthread_mutex_unlock(look->node->lock_right);
		} else {
			look->barrier = look->node->lock_right;
			look->insertion_point = (void**)&(look->node->right);
		}
		pthread_mutex_unlock(look->node->lock_look);
		look->node = swap;
		break;
	case -1:
		pthread_mutex_lock(look->node->lock_left);
		if (look->node->left == NULL)
		{
			look->callback = NULL;
			pthread_mutex_unlock(look->node->lock_left);
			pthread_mutex_unlock(look->node->lock_look);
			break;
		}
		swap = look->node->left;
		pthread_mutex_lock(swap->lock_look);
		if (look->barrier != NULL)
			pthread_mutex_unlock(look->barrier);
		if (peek(look->key,swap->key)==FALSE) {
			look->barrier = NULL;
			pthread_mutex_unlock(look->node->lock_left);
		} else {
			look->barrier = look->node->lock_left;
			look->insertion_point = (void**)&(look->node->left);
		}
		pthread_mutex_unlock(look->node->lock_look);
		look->node = swap;
		break;
	}
	return look;
}





static __thread int look_type;
static T_LOOKUP *
t_lookup(T_LOOKUP *look)
{
	pthread_mutex_lock(look->tree->root_lock);
	if (look->tree->base == NULL)
	{
		look->callback = NULL;
		pthread_mutex_unlock(look->tree->root_lock);
		return look;
	}
	pthread_mutex_lock(look->tree->base->lock_look);
	look->node = look->tree->base;

	switch (look_type) {
	case 0:
		look->callback = &t_sub;
		break;
	case 1:
		if (peek(look->key,look->tree->base->key)==TRUE)
		{
			look->tree->base = node_delete(look->tree->base);
			pthread_mutex_unlock(look->tree->base->lock_look);
			pthread_mutex_unlock(look->tree->root_lock);
			look->callback = NULL;
			return look;
		}
		look->callback = &d_sub;
		break;
	}

	pthread_mutex_unlock(look->tree->root_lock);
	return look;
}
static T_LOOKUP*
generate_lookup(T_LOOKUP *look, char *key, TREE *tree)
{
	look = malloc(sizeof(T_LOOKUP));
	look->key = strdup(key);
	look->value = NULL;
	look->callback = &t_lookup;
	look->tree = tree;
	look->mutex_chain = NULL;
	look->barrier = NULL;
	look->insertion_point = NULL;
	return look;
}

Boolean
lookup(TREE *tree, char *key, void **value)
{
	T_LOOKUP *look;
	look = generate_lookup(look, key, tree);
	look_type = 0;

	while (look->callback && (look = look->callback(look)) != NULL){}

	if (look->value != NULL)
	{
		*value = look->value;
		return TRUE;
	}

	return FALSE;
}

void
delete(TREE *tree, char *key)
{
	T_LOOKUP *look;
	look = generate_lookup(look, key, tree);
	look_type = 1;
	while (look->callback && (look = look->callback(look)) != NULL){}

	/*if (look->value != NULL)
	{
		look->callback =
	}*/
}
