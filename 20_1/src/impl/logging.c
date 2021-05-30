/*
 * logging.c
 *
 *  Created on: May 30, 2021
 *      Author: cory
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../../lib/logging.h"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

struct exec_info {
	int fd;
	char *file_name;
};

static struct exec_info this;
#define OUTBUF 1000
char out_buf[OUTBUF];

static int
log_beginning()
{
	const char *msg = "\n\nFileWatcherStarted::INFO::";
	char time_buff[1000];
	time_t xtime;
	struct tm *stime;
	size_t tsize;
	struct timeval *tv;
	struct timezone *tz;
	time_buff[0] = '\0';

	if ((xtime = time(NULL)) == -1)
	{
		close(this.fd);
		return -1;
	}

	if (write(this.fd, msg, strlen(msg)) != strlen(msg))
	{
		close(this.fd);
		return -1;
	}

	if ((stime = localtime(&xtime)) == NULL)
	{
		close(this.fd);
		return -1;
	}
	tsize = strftime(time_buff, 1000, "%Y-%m-%dT%H:%M:%S", stime);

	if (write(this.fd, time_buff, tsize)!=tsize)
	{
		close(this.fd);
		return -1;
	}

	if ((tv = malloc(sizeof(struct timeval))) == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	if ((tz = malloc(sizeof(struct timeval))) == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	if (gettimeofday(tv,tz) == -1)
	{
		return -1;
	}

	time_buff[0] = '\0';

	tsize = sprintf(time_buff, ".%ld", tv->tv_usec);
	if (write(this.fd, time_buff, tsize)!=tsize)
	{
		close(this.fd);
		return -1;
	}

	if (write(this.fd,"\n",1)!=1)
	{
		close(this.fd);
		return -1;
	}

	return 0;
}


int
init_log(char *file_name)
{
	this.file_name = strdup(file_name);
	if (this.file_name == NULL)
	{
		errno = ENOMEM;
		return -1;
	}

	this.fd = open(file_name,__20_1_FILE_FLAGS, __20_1_FILE_PERMS);
	if (this.fd == -1)
		return -1;

	return log_beginning();
}

static char
*event_type(int mask)
{
	out_buf[0] = '\0';
	if (mask & IN_ACCESS)
		strcat(out_buf,"IN_ACCESS|");
	if (mask & IN_MODIFY)
		strcat(out_buf,"IN_MODIFY|");
	if (mask & IN_ATTRIB)
		strcat(out_buf,"IN_ATTRIB|");
	if (mask & IN_CLOSE_WRITE)
		strcat(out_buf,"IN_CLOSE_WRITE|");
	if (mask & IN_CLOSE_NOWRITE)
		strcat(out_buf,"IN_CLOSE_NOWRITE|");
	if (mask & IN_CLOSE)
		strcat(out_buf,"IN_CLOSE|");
	if (mask & IN_OPEN)
		strcat(out_buf,"IN_OPEN|");
	if (mask & IN_MOVED_FROM)
		strcat(out_buf,"IN_MOVED_FROM|");
	if (mask & IN_MOVED_TO)
		strcat(out_buf,"IN_MOVED_TO|");
	if (mask & IN_MOVE)
		strcat(out_buf,"IN_MOVE|");
	if (mask & IN_CREATE)
		strcat(out_buf,"IN_CREATE|");
	if (mask & IN_DELETE)
		strcat(out_buf,"IN_DELETE|");
	if (mask & IN_DELETE_SELF)
		strcat(out_buf,"IN_DELETE_SELF|");
	if (mask & IN_MOVE_SELF)
		strcat(out_buf,"IN_MOVE_SELF|");
	if (mask & IN_UNMOUNT)
		strcat(out_buf,"IN_UNMOUNT|");
	if (mask & IN_Q_OVERFLOW)
		strcat(out_buf,"IN_Q_OVERFLOW|");
	if (mask & IN_IGNORED)
		strcat(out_buf,"IN_IGNORED|");
	if (mask & IN_CLOSE)
		strcat(out_buf,"IN_CLOSE|");
	if (mask & IN_MOVE)
		strcat(out_buf,"IN_MOVE|");
	if (mask & IN_ONLYDIR)
		strcat(out_buf,"IN_ONLYDIR|");
	if (mask & IN_DONT_FOLLOW)
		strcat(out_buf,"IN_DONT_FOLLOW|");
	if (mask & IN_EXCL_UNLINK)
		strcat(out_buf,"IN_EXCL_UNLINK|");
	if (mask & IN_MASK_ADD)
		strcat(out_buf,"IN_MASK_ADD|");
	if (mask & IN_ISDIR)
		strcat(out_buf,"IN_ISDIR|");
	if (mask & IN_ONESHOT)
		strcat(out_buf,"IN_ONESHOT|");
	out_buf[strlen(out_buf)-1] = '\0';
	return out_buf;
}

void
log_event(int (*bang)(void), struct inotify_event *event, char *directory)
{
#define BUF 4000
	char event_buffer[BUF];
	char time_buff[BUF/4];
	time_t t;
	struct timezone *tz;
	struct timeval *tv;
	struct tm *m;

	if (directory == NULL)
	{
		return;
	}

	if ((tz = malloc(sizeof(struct timezone))) == NULL)
	{
		errno = ENOMEM;
		(bang)();
		return;
	}

	if ((tv = malloc(sizeof(struct timeval))) == NULL)
	{
		free(tz);
		errno = ENOMEM;
		(bang)();
		return;
	}

	if ((t = time(NULL)) == (time_t)-1)
	{
		free(tz);
		free(tv);
		(bang)();
		return;
	}
	if((m = localtime(&t)) == NULL)
	{
		free(tz);
		free(tv);
		(bang)();
		return;
	}

	if (gettimeofday(tv,tz) == -1)
	{
		free(tz);
		free(tv);
		(bang)();
		return;
	}

	strftime(time_buff,BUF/4, "%Y-%m-%dT%H:%M:%S.", m);
	snprintf(event_buffer, BUF, "INOTIFY_EVENT::%s::%ld::`%s%s%s`::%s%ld\n",
			event_type(event->mask), event->cookie,
			directory, (event->name != NULL ? "/" : ""), (event->name != NULL ? event->name : ""),
			time_buff, tv->tv_usec);
	if (write(this.fd,event_buffer,strlen(event_buffer)) == -1)
	{
		free(tz);
		free(tv);
		(bang)();
		return;
	}

	free(tz);
	free(tv);
	return;
}
