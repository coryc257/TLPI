/*
 * main.c
 *
 *  Created on: May 29, 2021
 *      Author: cory
 */


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tlpi_hdr.h"
#include <string.h>

struct path_list {
	char *name;
	__ino_t inode;
	__dev_t device;
	struct path_list *next;
	struct path_list *prev;
};

struct path_head {
	struct path_list *first;
	struct path_list *last;
	size_t strlen;
	char *full_path;
};

static void
add_item_to_ph(struct path_head *ph, struct dirent *ent, __dev_t device)
{
	struct path_list *item;
	item = malloc(sizeof(struct path_list));
	item->name = strdup(ent->d_name);
	item->inode = ent->d_ino;
	item->device = device;
	item->next = NULL;
	item->prev = NULL;

	if (ph->last == NULL)
	{
		ph->last = item;
		ph->first = item;
		ph->strlen = strlen(item->name);
	}
	else
	{
		item->next = ph->first;
		ph->first->prev = item;
		ph->first = item;
		ph->strlen += strlen(item->name)+1;
	}
}

static void
build_path(struct path_head *ph)
{
	DIR *dir;
	struct dirent *ent;
	struct stat *st, *ost;
	int fd;
	__ino_t now;
	__ino_t then;

	ph->first = NULL;
	ph->last = NULL;
	then = 0;
	ost = NULL;
	st = NULL;
	for (;;){
		if ((dir = opendir(".")) == NULL)
			errExit("opendir");
		fd = dirfd(dir);
		if((st = malloc(sizeof(struct stat))) == NULL)
			errExit("malloc\n");
		fstat(fd,st);
		while((ent = readdir(dir)) != NULL)
		{

			if (strcmp(ent->d_name,".") == 0)
			{
				now = ent->d_ino;
				if (now == then)
				{
					ent->d_name[0] = '/';
					ent->d_name[1] = '\0';
					goto build_path__out;
				}
			}

			if (then == ent->d_ino)
			{
				add_item_to_ph(ph, ent, ost->st_dev);
			}
		}
		if (ost != NULL)
			free(ost);
		then = now;
		ost = st;
		closedir(dir);
		chdir("..");
	}
	build_path__out:
	if (ost != NULL)
		free(ost);
	if (st != NULL)
		free(st);
	ph->strlen++;
	return;
}

static void
construct_path(struct path_head *ph)
{
	char *full_path;
	struct path_list *cur, *swap;

	full_path = malloc(sizeof(char)*(ph->strlen+1));
	if (full_path == NULL)
		errExit("malloc\n");

	full_path[0] = '/';
	full_path[1] = '\0';
	cur = ph->first;
	swap = NULL;

	while(cur != NULL)
	{
		strcat(full_path,cur->name);
		if (swap != NULL)
		{
			free(swap->name);
			free(swap);
		}
		swap = cur;
		cur = cur->next;
		if (cur != NULL)
			strcat(full_path,"/");
	}

	ph->full_path = full_path;
}

static char
*my_getcwd(char *buf, size_t size)
{
	struct path_head ph;
	struct stat s;
	char *ret;

	build_path(&ph);
	construct_path(&ph);
	ret = NULL;
	if (stat(ph.full_path,&s) == -1)
	{
		free((ph.last->name));
		free((ph.last));
		free((ph.full_path));
		return NULL;
	}

	if (s.st_ino == ph.last->inode && s.st_dev == ph.last->device)
		ret = buf;
	else
	{
		free(ph.full_path);
		errno = ENOENT;
		return NULL;
	}

	free((ph.last->name));
	free((ph.last));

	if (strlen(ph.full_path)+1 > size)
	{
		errno = ERANGE;
		ret = NULL;
		free(ph.full_path);
		return NULL;
	}
	buf[0] = '\0';
	strcat(buf,ph.full_path);

	return ret;
}

int
main(int argc, char *argv[])
{
	char x[PATH_MAX];
	if (my_getcwd(x,PATH_MAX) != NULL)
		printf("Current Working Directory: %s\n",x);
}
