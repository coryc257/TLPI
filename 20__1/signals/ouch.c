/*
 * ouch.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */


#include <signal.h>
#include "tlpi_hdr.h"

static void
sigHandler(int sig)
{
	printf("Ouch!\n");
}

int
__ouch__main(int argc, char *argv[])
{
	int j;

	if (signal(SIGINT, sigHandler) == SIG_ERR)
		errExit("signal\n");

	for (j = 0; ; j++) {
		printf("%d\n",j);
		sleep(3);
	}
}
