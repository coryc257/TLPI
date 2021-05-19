/*
 * mem.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#include "lib/mem.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "tlpi_hdr.h"

static MALLOC_BLOCK base;
static MALLOC_BLOCK *head = NULL;
static MALLOC_BLOCK *tail = NULL;
static void *__brk_head;
extern int errno;

static void
*__MALLOC_INIT()
{
	if((__brk_head = sbrk(0)) == (void*)-1)
	{
		errno = ENOMEM;
		return (void*)-1;
	}

	head = &base;
	head->prev= NULL;
	head->alignment = NULL;
	head->block_length = 0;
	head->next = (void*)-1;
	return (void*)0;
}

static void
*__brk(size_t requested_size, size_t total_blocks, size_t total_requested)
{
	MALLOC_BLOCK *m_s;
	m_s = (MALLOC_BLOCK *)__brk_head;
	MALLOC_BLOCK *pos;
	void *start;

	if((start = sbrk(total_requested)) == (void*)-1)
	{
		errno = ENOMEM;
		return (void*)-1;
	}

	if(total_blocks == 0)
	{
		m_s->block_length = MALLOC_BLOCK_SIZE;
		m_s->prev = head;
		m_s->alignment = NULL;
		tail = m_s;
		pos = m_s;
		head->next = pos;
	}
	else
	{
		pos = (m_s+(total_blocks/MALLOC_BLOCK_SIZE));
		pos->prev = tail;
		tail->next = pos;
		tail = pos;
	}

	pos->alignment = (void*)(pos+1);
	pos->block_length = total_requested;
	pos->next = (void*)-1;

	return pos->alignment;
}

static void
*__inject(MALLOC_BLOCK *insertion_point, size_t total_requested)
{
	MALLOC_BLOCK *splitter;
	if(insertion_point->block_length > total_requested)
	{
		splitter = insertion_point + (total_requested/MALLOC_BLOCK_SIZE);
		splitter->alignment = NULL;
		splitter->prev = insertion_point;
		splitter->next = insertion_point->next;
		splitter->block_length = insertion_point->block_length-total_requested;
		insertion_point->next = splitter;
	}
	insertion_point->block_length=total_requested;
	return (insertion_point->alignment = (void*)(insertion_point+1));
}

static void
__cascade(MALLOC_BLOCK *ptr)
{
	size_t total_free = ptr->block_length;
	MALLOC_BLOCK *n = ptr->next;

	while(ptr->prev != head && ptr->prev->alignment == NULL)
	{
		total_free += ptr->prev->block_length;
		ptr = ptr->prev;
	}

	while(n != (void*)-1 && n->alignment == NULL)
	{
		total_free += n->block_length;
		n = n->next;
	}

	ptr->block_length = total_free;
	ptr->next = n;
	if(n != (void*)-1)
		n->prev = ptr;
}

void *my_malloc(size_t size)
{
	MALLOC_BLOCK *pos;
	void *p;
	size_t msize = sizeof(MALLOC_BLOCK);

	size_t total_requested = size
			+ /*ForcedAlignment*/(size%32 == 0 ? 0 : (32-(size%32)))
			+ MALLOC_BLOCK_SIZE;
	size_t total_blocks = 0;

	if(size == 0)
		return NULL;

	if(head == NULL)
		if(__MALLOC_INIT(size) == (void*)-1)
			return NULL;

	if((pos = head->next)==(void*)-1)
		goto __g_brk; // First one

	while(pos->next != (void*)-1)
	{
		total_blocks += pos->block_length;
		if(pos->alignment == NULL && pos->block_length >= total_requested)
		{
			return (p = __inject(pos, total_requested)) == (void *)-1 ? NULL : p;
		}
		pos = pos->next;
	}
	total_blocks+=pos->block_length;

	__g_brk:
	// Add to the end
	if((p =__brk(size, total_blocks, total_requested)) == (void*)-1)
		return NULL;

	return p;
}

void my_free(void *ptr)
{
	MALLOC_BLOCK *memory;
	memory = ptr;
	memory--;
	if(memory->alignment == ptr)
	{
		memory->alignment = NULL;
		__cascade(memory);
		return;
	}
	errExit("BAD FREE AHHHHH\n");
}
