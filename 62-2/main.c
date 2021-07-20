/*
 * main.c
 *
 *  Created on: Jul 19, 2021
 *      Author: cory
 */


#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "tlpi_hdr.h"

typedef int PD;
typedef struct __pipe {
	PD input;
	PD output;
} PIPE;


int make_pipe(PIPE **p)
{
	PIPE *new;
	int fds[2];

	*p = NULL;
	new = malloc(sizeof(PIPE));;

	if (new == NULL) {
		errno = ENOMEM;
		return -1;
	}

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fds) == -1) {
		free(new);
		*p = NULL;
		return -1;
	}

	new->input = fds[0];
	new->output = fds[1];

	if (shutdown(new->input, SHUT_RD) == -1) {
		free(new);
		return -1;
	}
	if (shutdown(new->output, SHUT_WR) == -1) {
		free(new);
		return -1;
	}

	*p = new;

	return 0;
}

int
main (int argc, char *argv[])
{
	PIPE *p;
	char buf[10];
	if (make_pipe(&p) == 0) {
		write(p->input, "test\n", 6);
		read(p->output, buf, 6);
		printf("%s", buf);
	} else {
		errExit("make_pipe\n");
	}
}
