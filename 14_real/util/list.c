/*
 * list.c
 *
 *  Created on: May 22, 2021
 *      Author: cory
 */

#include "../lib/list_functions.h"
#include "tlpi_hdr.h"

static LIST_ITEM
*SUPER_CLOSE
(
		AUTO_SORTED_LIST *list,
		long val
)
{
	long 	x_c = val-list->max->value,
			y_c = val-list->max_mid->value,
			z_c = val-list->middle->value,
			r_c = val-list->min_mid->value,
			t_c = val-list->min->value;
	long pivot = x_c < y_c ? x_c : y_c;
	pivot = z_c < pivot ? z_c : pivot;
	pivot = r_c < pivot ? r_c : pivot;
	pivot = t_c < pivot ? t_c : pivot;
	if(pivot == x_c)
		return list->max;
	else if(pivot == y_c)
		return list->max_mid;
	else if(pivot == z_c)
		return list->middle;
	else if(pivot == r_c)
		return list->min_mid;
	else if(pivot == t_c)
		return list->min;
	else
		return list->middle;
}

static void
BALANCE_SHIMMY(AUTO_SORTED_LIST *list)
{
	LIST_ITEM *item;
	item = list->max;
	while(item->next != NULL)
		item = item->next;

	list->max = item;
	list->min = item->prev->prev->prev->prev->prev->prev->prev->prev;
	list->min_mid = item->prev->prev->prev->prev->prev->prev;
	list->middle = item->prev->prev->prev->prev;
	list->max_mid = item->prev->prev;
}

// TODO: 	track distance between to points
//			If one hop to the next gets to be too big,
//			shift neighbor away and update hop count
static void
LIST_SHIMMY(AUTO_SORTED_LIST *list, LIST_ITEM *shim, DIRECTION direction, LIST_ITEM *insert)
{
	#define MAGIC_NUMBER 10
	list->total++;
	int moved = 0;


	if (list->max == shim && direction == DOWN)
	{
		/*if(list->total >= MAGIC_NUMBER)
		{
			list->max_mid = list->max_mid->prev;
		}*/
	}
	else if (list->max == shim && direction == UP)
	{
		list->max = insert;
		moved = 1;
		/*if(list->total >= MAGIC_NUMBER)
		{
			list->max_mid = list->max_mid->prev;
		}*/
	}
	else if (list->max_mid == shim && direction == DOWN)
	{
		if(list->total >= MAGIC_NUMBER)
		{
			//list->middle = list->middle->prev;
			list->max_mid = list->max_mid->prev;
			moved = 1;
		}
	}
	else if (list->max_mid == shim && direction == UP)
	{
		if(list->total >= MAGIC_NUMBER)
		{
			list->max_mid = list->max_mid->next;
			moved = 1;
		}
	}
	else if (list->middle == shim && direction == UP)
	{
		if (list->total >= MAGIC_NUMBER)
		{
			list->middle = list->middle->next;
			//list->max_mid = list->max_mid->next;
			moved = 1;
		}
	}
	else if (list->middle == shim && direction == DOWN)
	{
		if (list->total >= MAGIC_NUMBER)
		{
			list->middle = list->middle->prev;
			//list->max_mid = list->max_mid->prev;
			moved = 1;
		}
	}
	else if (list->min_mid == shim && direction == UP)
	{
		if (list->total >= MAGIC_NUMBER)
		{
			//list->middle = list->middle->next;
			list->min_mid = list->min_mid->next;
			moved = 1;
		}
	}
	else if (list->min_mid == shim && direction == DOWN)
	{
		if (list->total >= MAGIC_NUMBER)
		{
			list->min_mid = list->min_mid->prev;
			moved = 1;
		}
	}
	else if (list->min == shim && direction == DOWN)
	{
		list->min = insert;
		moved = 1;
		/*if (list->total >= MAGIC_NUMBER)
		{
			list->min_mid = list->min_mid->next;
		}*/
	}

	if (list->min->value > insert->value && moved == 0)
	{
		list->min->prev = insert;
		insert->next = list->min;
		list->min = insert;
	}
	else if (list->max->value < insert->value && moved == 0)
	{
		list->max->next = insert;
		insert->prev = list->max;
		list->max = insert;
	}

	if (list->total == MAGIC_NUMBER - 1)
	{
		BALANCE_SHIMMY(list);
	}
	//else if (list->middle == shim && direction)

}

// Next is bigger
// Prev is Smaller
static void
insert_list(AUTO_SORTED_LIST *list, LIST_ITEM *item)
{
	LIST_ITEM *search, *shim;
	search = shim = SUPER_CLOSE(list,item->value);
	int cont = 1;

	DIRECTION direction;
	while(cont)
	{
		if (item->value > search->value)
		{
			direction = UP;
			if (search->next == NULL)
			{
				search->next = item;
				item->prev = search;
				cont = 0;
			}
			else if (search->next->value > item->value)
			{
				item->prev = search;
				item->next = search->next;
				search->next->prev = item;
				search->next = item;
				cont = 0;
			}
			else
				search = search->next;
		}
		else if (item->value < search->value)
		{
			direction = DOWN;
			if(search->prev == NULL)
			{
				search->prev = item;
				item->next = search;
				cont = 0;
			}
			else if (search->prev->value < item->value)
			{
				item->next = search;
				item->prev = search->prev;
				search->prev->next = item;
				search->prev = item;
				cont = 0;
			}
			else
				search = search->prev;
		}
		else
		{
			cont = 0;
			if (item->value < list->min->value)
			{
				direction = DOWN;
				list->min->prev = item;
				item->next = list->min;
				list->min = item;
			}
			else if (item->value > list->max->value)
			{
				direction = UP;
				list->max->next = item;
				item->prev = list->max;
				list->max = item;
			}
		}
	}
	LIST_SHIMMY(list,shim,direction,item);
}

void
__add_to_list(AUTO_SORTED_LIST **list, LIST_ITEM *item)
{
	LINKED_ITEM *x_item;
	LINKED_LIST *x_list;
	if (*list == NULL)
	{
		*list = malloc(sizeof(AUTO_SORTED_LIST));

		if (*list == NULL)
		{
			errExit("__add_to_list::malloc()\n");
		}
		(*list)->max = item;
		(*list)->max_mid = item;
		(*list)->middle = item;
		(*list)->min = item;
		(*list)->min_mid = item;
		(*list)->total = 1;

		x_list = malloc(sizeof(LINKED_LIST));
		x_item = malloc(sizeof(LINKED_ITEM));
		if(x_item == NULL || x_list == NULL)
			errExit("malloc\n");

		(*list)->link = x_list;
		x_list->head = x_item;
		x_list->tail = x_item;
		x_item->item = item;
		/*(*list)->link = malloc(sizeof(LINKED_LIST));
		(*list)->link->head = malloc(sizeof(struct linked_item));
		(*list)->link->tail = (*list)->link->head;
		(*list)->link->tail->next = NULL;
		(*list)->link->tail->prev = NULL;
		(*list)->link->head = item;
		(*list)->link->tail = item;*/
		return;
	}
	x_list = (*list)->link;
	x_item = malloc(sizeof(LINKED_ITEM));
	x_item->item = item;
	x_item->prev = x_list->tail;
	x_list->tail->next = x_item;
	x_list->tail = x_item;
	insert_list(*list,item);
}

LIST_ITEM
*__new_list_item()
{
	LIST_ITEM *x;
	x = malloc(sizeof(LIST_ITEM));
	x->file_name = NULL;
	x->next = NULL;
	x->prev = NULL;
	return x;
}

void
__list_print(AUTO_SORTED_LIST *list, DIRECTION direction)
{
	LIST_ITEM *x;
	printf("LIST OUTPUT (%s):\n", direction == UP ? "ASCENDING" : "DESCENDING");
	if (direction == UP)
	{
		x = list->min;
		while(x != NULL){
			printf("\tLIST ITEM:%ld\n", x->value);
			x = x->next;
		}
	}
	else if (direction == DOWN)
	{
		x = list->max;
		while (x != NULL) {
			printf("\tLIST ITEM:%ld\n", x->value);
			x = x->prev;
		}
	}
}

LIST_ITEM
*__list_set_item(LIST_ITEM *item, long number, char *directory)
{
	char *file_name;
	file_name = malloc(sizeof(char)*256 + strlen(directory)+2);
	sprintf(file_name, "%s/x%06ld", directory, number);
	item->file_name = file_name;
	item->value = number;
	return item;
}
