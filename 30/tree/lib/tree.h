/*
 * tree.h
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */

#ifndef TREE_LIB_TREE_H_
#define TREE_LIB_TREE_H_

#include <pthread.h>
#include "tlpi_hdr.h"

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

typedef struct LOCK_LIST {
	pthread_mutex_t *lock;
	struct LOCK_LIST *next;
} LOCK_LIST;

//typedef void* (*TREE_LOOKUP)(TREE_ITEM **item, char *key);
typedef struct T_LOOKUP {
	struct T_LOOKUP *(*callback)(struct T_LOOKUP*);
	void *value;
	char *key;
	TREE *tree;
	TREE_ITEM *node;
	TREE_ITEM *emplacement;
	LOCK_LIST *mutex_chain;
	pthread_mutex_t *barrier;
	void **insertion_point;
} T_LOOKUP;



typedef void* (*TF_TRAMPOLINE)(TREE_ITEM **item, TREE_ITEM *new, int *status);

typedef TF_TRAMPOLINE (*X_CALLBACK)(TREE_ITEM **item, TREE_ITEM *new, int *status);

int initialize(TREE *tree);
int add(TREE *tree, char *key, void *value);
Boolean lookup(TREE *tree, char *key, void **value);
void delete(TREE *tree, char *key);

#endif /* TREE_LIB_TREE_H_ */
