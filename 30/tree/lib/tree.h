/*
 * tree.h
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */

#ifndef TREE_LIB_TREE_H_
#define TREE_LIB_TREE_H_

#include <pthread.h>

typedef struct thread_tree_item {
	pthread_mutex_t *lock_left;
	pthread_mutex_t *lock_right;
	pthread_mutex_t *lock_look;

	struct thread_tree_item *left;  // LESS    THAN
	struct thread_tree_item *right; // GREATER THAN

	char *key;
	void *value;
} TREE_ITEM;

typedef struct thread_tree {
	pthread_mutex_t *root_lock;
	TREE_ITEM *base;
	int error;
} TREE;

int initialize(TREE *tree);
int add(TREE *tree, char *key, void *value);

#endif /* TREE_LIB_TREE_H_ */
