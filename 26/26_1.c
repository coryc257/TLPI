/*
 * 26_1.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void
x26_1__main(int argc, char *argv[])
{
	switch(fork()) {
	case -1:
		printf("Error At Fork\n");
	case 0:
		printf("My Parent:%ld\n",getppid());
		sleep(5);
		printf("My Parent:%ld\n",getppid());
	default:
		sleep(2);
		_exit(EXIT_SUCCESS);
	}
}
