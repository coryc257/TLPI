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

typedef void* (*TF_TRAMPOLINE)(TREE_ITEM **item, TREE_ITEM *new, int *status);

typedef TF_TRAMPOLINE (*X_CALLBACK)(TREE_ITEM **item, TREE_ITEM *new, int *status);


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
