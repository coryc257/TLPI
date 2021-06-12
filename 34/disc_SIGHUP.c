/*
 * disc_SIGHUP.c
 *
 *  Created on: Jun 12, 2021
 *      Author: cory
 */


#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void
disc_SIGHUP_handler (int sig)
{
	printf("PID %ld: caught signal %2d (%s)\n", (long)getpid(),
			sig, strsignal(sig));
}

int
disc_SIGHUP (int argc, char *argv[])
{
	pid_t parentPid, childPid;
	int j;
	struct sigaction sa;

	if (argc < 2 || strcmp(argv[1],"--help") == 0)
		usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);

	signal(SIGHUP, SIG_IGN);

	setbuf(stdout,NULL);

	parentPid = getpid();
	printf("PID of parent process is:      %ld\n", (long) parentPid);
	printf("Foreground process group ID is: %ld\n", (long) tcgetpgrp(STDIN_FILENO));

	for (j = 1; j < argc; j++) {
		childPid = fork();
		if (childPid == -1)
			errExit("fork\n");

		if (childPid == 0) {
			if (argv[j][0] == 'd')
				if (setpgid(0,0) == -1)
					errExit("setpgid");

			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = disc_SIGHUP_handler;
			if (sigaction(SIGHUP, &sa, NULL) == -1)
				errExit("sigaction\n");
			break;
		}
	}

	alarm(60);

	printf("PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());
	for (;;)
		pause();
}
