/*
 * file.c
 *
 *  Created on: May 30, 2021
 *      Author: cory
 */

#include <ftw.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>


struct watch {
	struct watch_item *head;
	struct watch_item *tail;
};

struct watch_item {
	char *watch_path;
	int watch_id;
	struct watch_item *prev;
	struct watch_item *next;
};

struct exec_info {
	char *root_folder;
	int fd;
	struct watch *w;
	int (*bang)(void);
};

struct exec_info this;
#define BUF_LEN (10 * sizeof(struct inotify_event) + NAME_MAX + 1)
#define WATCH_MASK IN_ALL_EVENTS
//IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO

static int add_watch_item(const char *path, const struct stat *sb, struct watch_item **out)
{
	struct watch_item *x;

	if (!S_ISDIR(sb->st_mode))
		return 0;

	if((x = malloc(sizeof(struct watch_item))) == NULL)
	{
		errno = ENOMEM;
		return -1;
	}
	x->next = NULL;
	x->prev = NULL;
	if ((x->watch_path = strdup(path)) == NULL)
	{
		free(x);
		errno = ENOMEM;
		return -1;
	}

	if (this.w->head == NULL)
	{
		this.w->head = x;
		this.w->tail = x;
	}
	else
	{
		this.w->tail->next = x;
		x->prev = this.w->tail;
		this.w->tail = x;
	}

	//printf("%s\n",path);
	if(out != NULL)
		*out = x;
	return 0;
}

static int
__file_found(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	return add_watch_item(fpath, sb, NULL);
}

int
build_file_list(char *root_folder)
{
	if ((this.w = malloc(sizeof(struct watch))) == NULL)
	{
		errno = ENOMEM;
		return -1;
	}
	this.w->head = NULL;
	this.w->tail = NULL;
	this.root_folder = strdup(root_folder);
	return nftw(this.root_folder,__file_found, 10, 0);
}

int
init_watch(int (*bang)(void))
{
	struct watch_item *c;
	if (this.w == NULL || this.w->head == NULL)
		return -1;

	if(bang == NULL)
			return -1;

	if((this.fd = inotify_init()) == -1)
		return -1;

	this.bang = bang;

	c = this.w->head;
	while (c != NULL)
	{
		c->watch_id = inotify_add_watch(this.fd, c->watch_path,
				WATCH_MASK);
		c = c->next;
	}

	return 0;
}

static void
insert_watch(char *name, char *parent)
{
	struct stat *sb;
	struct watch_item *item;
	char *fp;
	int xstrlen;
	xstrlen = strlen(parent)+strlen(name)+2;
	if ((fp = malloc(sizeof(char)*xstrlen)) == NULL)
	{
		errno = ENOMEM;
		(this.bang)();
		return;
	}
	if((sb = malloc(sizeof(struct stat))) == NULL)
	{
		errno = ENOMEM;
		(this.bang)();
		return;
	}
	fp[0] = '\0';
	sprintf(fp,"%s/%s", parent, name);
	if ((stat(fp,sb)) != 0)
	{
		(this.bang)();
		free(fp);
		free(sb);
		return;
	}
	if (add_watch_item((const char*)fp,(const struct stat*)sb, &item) != 0)
	{
		(this.bang)();
		free(fp);
		free(sb);
		return;
	}
	if ((item->watch_id =
			inotify_add_watch(this.fd, item->watch_path,WATCH_MASK)) == -1)
	{
		(this.bang)();
		free(fp);
		free(sb);
		return;
	}
	free(fp);
	free(sb);
}

static void
add_watch(char *name, int watch_id)
{
	struct watch_item *w;

	w = this.w->head;
	while (w != NULL) {
		if (w->watch_id == watch_id) {
			insert_watch(name, w->watch_path);
			break;
		}
		w = w->next;
	}
}

static void
unlink_watch (struct watch_item *w)
{
	/*if (inotify_rm_watch(this.fd,w->watch_id) == -1)
		(this.bang)();*/
	if (w == this.w->head && w == this.w->tail) {
		w->watch_id = -1;
		free(w->watch_path);
		w->next = NULL;
		w->prev = NULL;
	} else if (w == this.w->head) {
		this.w->head = this.w->head->next;
		this.w->head->prev = NULL;
		free(w->watch_path);
		free(w);
	} else if (w == this.w->tail) {
		this.w->tail = this.w->tail->prev;
		this.w->tail->next = NULL;
		free(w->watch_path);
		free(w);
	} else {
		w->prev->next = w->next;
		w->next->prev = w->prev;
		free(w->watch_path);
		free(w);
	}
}


static void
delete_watch(int wd)
{
	struct watch_item *w;
	w = this.w->head;
	while (w != NULL) {
		if (w->watch_id == wd) {
			unlink_watch(w);
			break;
		}
		w = w->next;
	}
}

static char
*lookup(int wd)
{
	struct watch_item *w;
	w = this.w->head;
	while (w != NULL) {
		if (w->watch_id == wd) {
			return strdup(w->watch_path);
		}
		w = w->next;
	}
	return NULL;
}

static void
displayInotifyEvent(struct inotify_event *event)
{
	log_event(this.bang,(const struct inotify_event*)event, lookup(event->wd));
	if (event->cookie > 0)
		printf("cookie = %4d; ", event->cookie);

	printf("mask = ");
	if ((event->mask & IN_CREATE) && (event->mask & IN_ISDIR)) {
		add_watch(event->name, event->wd);
	}

	if ((event->mask) & IN_DELETE_SELF) {
		delete_watch(event->wd);
	}



	/*if (event->mask & IN_ACCESS)				printf("IN_ACCESS");
	if (event->mask & IN_ATTRIB)				printf("IN_ATTRIB");
	if (event->mask & IN_CLOSE_NOWRITE)			printf("IN_CLOSE_NOWRITE");
	if (event->mask & IN_CLOSE_WRITE)			printf("IN_CLOSE_WRITE");
	if (event->mask & IN_CREATE)				printf("IN_CREATE");
	if (event->mask & IN_DELETE)				printf("IN_DELETE");
	if (event->mask & IN_DELETE_SELF)			printf("IN_DELETE_SELF");
	if (event->mask & IN_IGNORED)				printf("IN_IGNORED");
	if (event->mask & IN_ISDIR)					printf("IN_ISDIR");
	if (event->mask & IN_MODIFY)				printf("IN_MODIFY");
	if (event->mask & IN_MOVE_SELF)				printf("IN_MOVE_SELF");
	if (event->mask & IN_MOVED_FROM)			printf("IN_MOVED_FROM");
	if (event->mask & IN_MOVED_TO)				printf("IN_MOVED_TO");
	if (event->mask & IN_OPEN)					printf("IN_OPEN");
	if (event->mask & IN_Q_OVERFLOW)			printf("IN_Q_OVERFLOW");
	if (event->mask & IN_UNMOUNT)				printf("IN_UNMOUNT");
	if (event->mask & IN_ACCESS)				printf("IN_ACCESS");
	printf("; FILE = ");
	printf(event->name);
	printf("\n");*/
}

void
watch_main()
{
	int bread;
	char buf[BUF_LEN];
	char *p;
	struct inotify_event *event;
	for (;;) {
		bread = read(this.fd, buf, BUF_LEN);
		for (p = buf; p < buf + bread;){
			event = (struct inotify_event*)p;
			displayInotifyEvent(event);
			p += sizeof(struct inotify_event) + event->len;
		}
	}
}
