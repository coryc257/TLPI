/*
 * ftw.c
 *
 *  Created on: May 29, 2021
 *      Author: cory
 */
#include "../../lib/ftw.h"
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

struct dir_list {
	struct dir_list_entry *head;
	struct dir_list_entry *last;
};

struct dir_list_entry {
	char *directory;
	char *name;
	char *full_path;
	struct stat *st;
	struct dir_list_entry *next;
	struct dir_list_entry *prev;
	int is_directory;
};

static void
dir_list_entry_destroy(struct dir_list_entry *x)
{
	free(x->directory);
	free(x->full_path);
	free(x->name);
	free(x->st);
	x->next = NULL;
	x->prev = NULL;
	free(x);
}

static int
scan_directory__add(struct dir_list *list, const char *dirpath, const struct dirent *entry)
{
	struct dir_list_entry *new;
	char *x;
	if((new = malloc(sizeof(struct dir_list_entry))) == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	new->directory = strdup(dirpath);
	new->name = strdup(entry != NULL ? entry->d_name : "");
	new->next = NULL;
	new->prev = NULL;
	if(new->directory == NULL || new->name == NULL)
	{
		if (new->directory != NULL)
			free(new->directory);
		if (new->name != NULL)
			free(new->name);
		free(new);
		errno = ENOMEM;
		return -1;
	}
	if((new->full_path = malloc(sizeof(char)*(strlen(new->directory)+strlen(new->name)+2))) == NULL)
	{
		free(new->directory);
		free(new->name);
		free(new);
		errno = ENOMEM;
		return -1;
	}

	new->full_path[0] = '\0';

	strcat(new->full_path,new->directory);
	strcat(new->full_path,"/");
	strcat(new->full_path,new->name);

	// Split the string if the last char is /
	x =	strrchr(new->full_path,'/');
	x++;
	if (*x == '\0')
	{
		x--;
		*x = '\0';
		x = strrchr(new->full_path,'/');
		free(new->name);
		new->name = strdup(++x);
		x = strrchr(new->directory, '/');
		*x = '\0';
	}


	if((new->st = malloc(sizeof(struct stat))) == NULL)
	{
		free(new->directory);
		free(new->name);
		free(new->full_path);
		free(new);
		errno = ENOMEM;
		return -1;
	}

	if ((stat(new->full_path,new->st) == -1))
	{
		free(new->directory);
		free(new->name);
		free(new->full_path);
		free(new->st);
		free(new);
		return -1;
	}


	if (((new->st->st_mode) & S_IFMT) == S_IFDIR)
		new->is_directory = 1;
	else
		new->is_directory = 0;

	if(list->head == NULL)
	{
		list->head = new;
		list->last = new;
		return 0;
	}

	list->last->next = new;
	new->prev = list->last;
	list->last = new;

	return 0;
}

static struct dir_list
*scan_directory(const char *dirpath)
{
	struct dir_list *list;
	struct dir_list_entry *cl, *cs;
	DIR *dir;
	struct dirent *entry;

	if ((list = malloc(sizeof(struct dir_list))) == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	list->head = NULL;
	list->last = NULL;

	if ((dir = opendir(dirpath)) == NULL)
	{
		free(list);
		return NULL;
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
			continue;
		if (scan_directory__add(list,dirpath,entry) == -1)
			goto scan_directory_die;
	}

	return list;
	scan_directory_die:
	if (list->head != NULL)
	{
		cl = list->head;
		while (cl != NULL)
		{
			cs = cl;
			cl = cl->next;
			dir_list_entry_destroy(cs);
		}
		free(list);
	}
	return NULL;
}

static int
yank_files(struct dir_list *list, int (*fn)(const char *fpath, const struct stat *sb,
		int typeflag, struct FTW *ftwbuf))
{
	int num_dirs;
	struct dir_list_entry *item, *swap;
	struct dir_list_entry *p, *n;
	item = list->head;
	num_dirs = 0;
	while (item != NULL)
	{
		if (item->is_directory == 0)
		{
			(fn)(item->full_path,NULL,0,NULL);

			p = item->prev;
			n = item->next;

			if (p == NULL)
				list->head = n;
			else
				p->next = n;

			if (n == NULL)
				list->last = p;
			else
				n->prev = p;

			swap = item->next;
			/*if (item->prev != NULL && item->next != NULL)
			{
				item->prev->next = item->next;
				item->next->prev = item->prev;
			}*/
			item->next = NULL;
			item->prev = NULL;
			dir_list_entry_destroy(item);
			item = swap;
		}
		else
		{
			//item->is_directory = 0;
			num_dirs++;
			item = item->next;
		}
	}
	return num_dirs;
}

int
my_nftw(const char *dirpath,
		int (*fn) (const char *fpath, const struct stat *sb,
				int typeflag, struct FTW *ftwbuf),
		int nopenfd, int flags)
{
	int depth = (flags & FTW_DEPTH) != 0 ? 1 : 0;
	struct dir_list *list, *concat;
	struct dir_list_entry *item, *swap;

	list = scan_directory(dirpath);
	scan_directory__add(list, dirpath, NULL);

	if (depth == 0) {
		(fn)(list->last->full_path, NULL, 0, NULL);
		list->last->prev->next = NULL;
		dir_list_entry_destroy(list->last);
		item = list->head;
		while (item != NULL)
		{
			(fn)(item->full_path, NULL, 0, NULL);
			if (item->is_directory == 1)
			{
				concat = scan_directory(item->full_path);
				swap = item->next;
				item->next = concat->head;
				concat->last->next = swap;
				swap->prev = concat->last;
				free(concat);
			}
			swap = item;
			item = item->next;
			dir_list_entry_destroy(swap);
		}
	} else {
		yank_files(list,fn);
		list->last->is_directory = 0;
		item = list->head;
		while (item != NULL)
		{
			if (item->is_directory == 1)
			{
				concat = scan_directory(item->full_path);
				item->is_directory = 0;
				if(yank_files(concat, fn) > 0)
				{

					list->head->prev = concat->last;
					concat->last->next = list->head;
					item = concat->head;
					free(concat);
				}
				else
				{
					free(concat);
					item = item->next;
				}
			}
			else
			{
				(fn)(item->full_path,NULL,0,NULL);
				swap = item;
				item = item->next;
				free(swap);
			}
		}
	}

	/*while(1){

	}*/
}
