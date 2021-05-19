/*
 * mem.h
 *
 *  Created on: May 13, 2021
 *      Author: cory
 */

#ifndef MEM_H_
#define MEM_H_

#include <unistd.h>
void *my_malloc(size_t);
void my_free(void *ptr);

#define MALLOC_BLOCK_SIZE sizeof(MALLOC_BLOCK)

typedef struct __MALLOC_BLOCK{
	struct __MALLOC_BLOCK *next;
	struct __MALLOC_BLOCK *prev;
	size_t block_length;
	void *alignment;
} MALLOC_BLOCK;

#endif /* MEM_H_ */
