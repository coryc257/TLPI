/*
 * main.c
 *
 *  Created on: Jun 4, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

extern int errno;

static sig_atomic_t exit_switch = 0;

void
handler(int sig)
{

}

int
main(int argc, char *argv[])
{
	int fd, status;
	pid_t child;
	struct sigaction act;
	sigemptyset(&(act.sa_mask));
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, NULL);

	fd = open("test.txt", O_CREAT | O_RDWR, NULL);

	switch(child = vfork()){
	case -1:
		exit(EXIT_FAILURE);
	case 0:
		printf("Close\n");
		close(fd);
		_exit(EXIT_SUCCESS);
	default:
		waitpid(child,&status,0);
		if (write(fd,"x",1) == -1 && errno == EBADF)
		{
			printf("It did indeed close\n");
		}
		else
		{
			printf("No good man\n");
		}
	}
}
