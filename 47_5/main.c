/*
 * main.c
 *
 *  Created on: Jul 3, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include "lib/event_flag.h"
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
	EVENT_FLAG *flag;
	pid_t child;
	flag = initEventFlag("murican flag");
	printf("Created\n");

	switch(child = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		waitForEventFlag(flag);
		printf("EVENT\n");
		_exit(EXIT_SUCCESS);
		break;
	default:
		sleep(2);
		setEventFlag(flag);
		waitpid(child,NULL,0);
		break;
	}

	terminateEventFlag(&flag);
	printf("Terminated\n");

}

