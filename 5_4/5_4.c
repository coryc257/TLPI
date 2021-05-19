/*
 * 5_4.c
 *
 *  Created on: May 10, 2021
 *      Author: cory
 */

#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include <errno.h>

extern errno;

int my_dup(int oldfd);
int my_dup2(int oldfd, int newfd);

int
main(void)
{
	int fd, f2, fb, fx;
	fd = open("test.file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if(fd == -1)
		errExit("bad juju\n");

	f2 = my_dup(fd);
	fb = my_dup2(-5,3);

	printf("%d, %d, %d\n", fd, f2, fb);

	fx = open("test2.file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	fb = my_dup2(fx,3);
	printf("%d, %d\n", fx, fb);

	write(fb,"test",4);
}


int
my_dup(int oldfd)
{
	int ret;
	ret = fcntl(oldfd, F_DUPFD, 0);
	return ret;
}

int
my_dup2(int oldfd, int newfd)
{
	int cur_flags = fcntl(oldfd, F_GETFL);

	if(cur_flags == -1)
	{
		errno = EBADF;
		return -1;
	}

	if(oldfd == newfd)
	{
		return newfd;
	}

	close(newfd);
		errno = 0;

	return fcntl(oldfd, F_DUPFD, newfd);
}
