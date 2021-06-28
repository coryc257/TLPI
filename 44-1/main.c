/*
 * main.c
 *
 *  Created on: Jun 26, 2021
 *      Author: cory
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"


struct object_container {
	int reader;
	int writer;
};

static struct object_container this;

static void
run_child()
{
	//printf("%d,%d::%d,%d\n",(int)'a', (int)'z', (int)'A', (int)'Z');
	char line[1002];
	for (;;) {
		read(this.reader,line,1002);
		for (int j = 0; j < 1002; j++) {
			if (line[j] >= 'a' && line[j] <= 'z')
				line[j] = line[j]-('a'-'A');
		}
		write(this.writer,line,1002);
	}
}

static void
run_parent()
{
	int numRead;
	char line[1002];
	for (;;) {
		numRead = read(STDIN_FILENO, line, 1001);
		line[numRead] = '\0';
		if (line[numRead-1] != '\n')
			errExit("Bad Input\n");
		write(this.writer,line,1002);
		read(this.reader,line,1002);
		printf("%s", line);
	}
}


int
main(int argc, char *argv[])
{
	int pp[2], pp2[2];
	pid_t child;

	pipe(pp);
	pipe(pp2);

	switch (child = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		this.reader = pp[0];
		this.writer = pp2[1];
		close(pp[1]);
		close(pp2[0]);
		run_child();
		break;
	default:
		this.reader = pp2[0];
		this.writer = pp[1];
		close(pp2[1]);
		close(pp[0]);
		run_parent();
		break;
	}


}
