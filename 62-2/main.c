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
#include "make_pipe.h"


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
