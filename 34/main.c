/*
 * main.c
 *
 *  Created on: Jun 11, 2021
 *      Author: cory
 */
#include "tlpi_hdr.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	pid_t child;
	if (argc != 2)
		usageErr("%s <option>\n", argv[0]);
	if (strcmp(argv[1],"34-2-spew") == 0) {
		child = spew(NULL,0);
	} else if (strcmp(argv[1], "34-2-exec") == 0) {
		child = spew(argv[0],1);
		sleep(1);
		kill(child,SIGUSR1);
		setpgid(child,0);
		sleep(2);
		kill(child,SIGUSR1);
		setpgid(child,0);
		sleep(2);
	}  else if (strcmp(argv[1], "34-2-sync") == 0) {
		child = spew(argv[0],0);
		sleep(1);
		kill(child,SIGUSR1);
		setpgid(child,0);
		sleep(2);
		kill(child,SIGUSR1);
		setpgid(child,0);
		sleep(2);
	}

}
