/*
 * make_zombie.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */


#include <signal.h>
#include <libgen.h>
#include "tlpi_hdr.h"
#include <unistd.h>

#define CMD_SIZE 200

static sig_atomic_t ss = 0;

void
handler(int sig)
{
	if (sig == SIGUSR1)
		ss = 1;
}

int
make_zombie(int argc, char *argv[])
{

	sigset_t block, open;
	struct sigaction act;
	char cmd[CMD_SIZE];
	pid_t childPid;


	sigemptyset(&open);
	sigfillset(&block);
	setbuf(stdout,NULL);
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigfillset(&(act.sa_mask));
	sigprocmask(SIG_SETMASK,&block,NULL);
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGUSR2, &act, NULL);

	printf("Parent PID=%ld\n", (long) getpid());
	sigprocmask(SIG_SETMASK,&open,NULL);
	switch (childPid = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		for (;;) {
			pause();
			if (ss)
				break;
		}
		sleep(5);
		printf("Child (PID=%ld) exiting\n", (long) getpid());
		kill(getppid(),SIGUSR1);
		_exit(EXIT_SUCCESS);
	default:


		snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
		cmd[CMD_SIZE-1] = '\0';
		system(cmd);
		kill(childPid,SIGUSR1);
		for (;;) {
			pause();
			if (ss)
				break;
		}
		system(cmd);



		if (kill(childPid,SIGKILL) == -1)
			errMsg("kill\n");
		sleep(3);
		printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
		system(cmd);
		exit(EXIT_SUCCESS);
	}
}
