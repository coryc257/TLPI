/*
 * 26_2.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void
x26_2__main(int argc, char *argv[])
{
	pid_t parent, child, grand;
	grand = getpid();
	switch(parent=fork()) {
	case -1:
		printf("Dead\n");
		_exit(EXIT_FAILURE);
	case 0:
		switch(child=fork()) {
		case 0:
			sleep(1);
			printf("Parent:%ld\n",getppid());
			sleep(7);
			printf("Parent:%ld\n",getppid());
			_exit(EXIT_SUCCESS);
			break;
		default:
			_exit(EXIT_SUCCESS);
			break;
		}
		break;
	default:
		sleep(5);
		printf("Wait:\n");
		waitpid(parent,NULL,0);
		_exit(EXIT_SUCCESS);
	}
}
