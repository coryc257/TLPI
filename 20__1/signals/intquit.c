/*
 * intquit.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#include <signal.h>
#include "tlpi_hdr.h"

static void
sigHandler(int sig)
{
	static int count = 0;

	/* UNSAFE: This handler uses a non-async-signal-safe functions
	 * (printf(), exit(); see section 21.1.2)
	 */

	if (sig == SIGINT){
		count++;
		printf("Caught SIGINT (%d)\n", count);
		return;
	}

	printf("Caught SIGQUIT - that's all folks\n");
	exit(EXIT_SUCCESS);
}

int
__intquit__main(int argc, char *argv[])
{
	/* esablish same handler for SIGINT and SIGQUIT */
	if (signal(SIGINT, sigHandler) == SIG_ERR)
		errExit("signal\n");
	if (signal(SIGQUIT, sigHandler) == SIG_ERR)
		errExit("signal\n");
	for (;;) pause();
}
