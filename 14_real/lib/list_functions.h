/*
 * list_functions.h
 *
 *  Created on: May 22, 2021
 *      Author: cory
 */

#ifndef LIB_LIST_FUNCTIONS_H_
#define LIB_LIST_FUNCTIONS_H_

#define ADD_LIST_ITEM(list, item) __add_to_list(&list, item)
#define NEW_LIST_ITEM() __new_list_item()
#define PRINT_ASCENDING(list) __list_print(list,UP)
#define SET_ITEM(list,number,dir) __list_set_item(list,number,dir)
#include <unistd.h>

typedef enum {
	UP,
	DOWN
} DIRECTION;


typedef struct list_item {
	long value;
	char *file_name;
	struct list_item *next;
	struct list_item *prev;
} LIST_ITEM;

typedef struct linked_item {
	LIST_ITEM *item;
	struct linked_item *next;
	struct linked_item *prev;
} LINKED_ITEM;

typedef struct linked_list {
	LINKED_ITEM *head;
	LINKED_ITEM *tail;
} LINKED_LIST;

typedef struct autosorted_list {
	LIST_ITEM *max;
	LIST_ITEM *middle;
	LIST_ITEM *min;
	LIST_ITEM *max_mid;
	LIST_ITEM *min_mid;
	LINKED_LIST *link;
	long total;
} AUTO_SORTED_LIST;

void __add_to_list(AUTO_SORTED_LIST **list, LIST_ITEM *item);
LIST_ITEM *__new_list_item();
void __list_print(AUTO_SORTED_LIST *list, DIRECTION direction);
LIST_ITEM *__list_set_item(LIST_ITEM *item, long number, char *directory);


#endif /* LIB_LIST_FUNCTIONS_H_ */
