/*
 * io.c
 *
 *  Created on: May 22, 2021
 *      Author: cory
 */
#include "../lib/io.h"
#include "tlpi_hdr.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void
LIST_GENERATE_FILES_RANDOM(LINKED_LIST *list)
{
	int count;
	struct tms start, finish;
	clock_t c_start, c_finish;
	LINKED_ITEM *item;
	int fd;

	count = 0;
	item = list->head;
	c_start = times(&start);

	while(item != NULL)
	{
		fd = open(item->item->file_name,
				O_CREAT | O_TRUNC | O_RDWR,
				S_IRUSR | S_IWUSR);
		write(fd,"x",1);
		close(fd);
		item = item->next;
		count++;
	}

	c_finish = times(&finish);
	printf("CreationTime: %ld WITH: %d\n", (long)(c_finish-c_start), count);
}

void
LIST_DELETE_FILES_ASCENDING(AUTO_SORTED_LIST *list)
{
	struct tms start, finish;
	clock_t c_start, c_finish;
	LIST_ITEM *item;
	int count = 0;

	item = list->min;
	c_start = times(&start);

	while(item != NULL)
	{
		if(remove(item->file_name) == -1)
			errExit("remove");
		item = item->next;
		count++;
	}

	c_finish = times(&finish);
	printf("DeletionTime: %ld WITH: %d\n", (long)(c_finish-c_start), count);
}
