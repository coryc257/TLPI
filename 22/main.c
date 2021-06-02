/*
 * main.c
 *
 *  Created on: Jun 1, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"
#include "V/lib/vsignal.h"

static volatile sig_atomic_t __22_1_switch = 0;

void
__22_1_hand(int sig)
{
	pid_t zoinks;
	int status;
	if (sig == SIGCONT)	{
		switch(zoinks = fork())	{
		case -1:
			errExit("fork\n");
		case 0:
			printf("GOT:%d\n", zoinks);
			_exit(EXIT_SUCCESS);
			break;
		default:
			waitpid(zoinks, &status, 0);
			break;
		}
	} else if (sig == SIGINT){
		__22_1_switch = 1;
	}
}

void __22_1__main (int argc, char *argv[])
{
	sigset_t blocker, empty;
	struct sigaction act;
	sigemptyset(&blocker);
	sigaddset(&blocker, SIGCONT);
	sigemptyset(&empty);
	act.sa_handler = __22_1_hand;
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;
	sigprocmask(SIG_SETMASK, &blocker, NULL);
	sigaction(SIGCONT, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	for (;;) {
		pause();
		if (__22_1_switch)
			sigprocmask(SIG_SETMASK, &empty, NULL);
	}
}

void
__22_2__hand (int sig)
{
	pid_t zoinks;
	int status;
	if (sig != SIGCHLD)
		switch (zoinks=fork()) {
		case -1:
			errExit("fork\n");
			break;
		case 0:
			printf("SIGNAL:%d\n",sig);
			_exit(EXIT_SUCCESS);
			break;
		default:
			waitpid(zoinks,&status,0);
			if (status != 0)
				errExit("zoinks\n");
			break;
		}
}

void
__22_2__main (int argc, char *argv[])
{
	sigset_t blocker, empty;
	struct sigaction act;
	int status;

	sigemptyset(&empty);
	sigfillset(&blocker);
	sigprocmask(SIG_SETMASK, &blocker, NULL);

	act.sa_handler = __22_2__hand;
	sigfillset(&(act.sa_mask));
	act.sa_flags = 0;

	for (int j = 0; j < NSIG; j++)
	{
		sigaction(j,&act,NULL);
	}

	setbuf(stdout, NULL);
	printf("SLEEP NAO(%d)", getpid());

	sleep(60);
	sigprocmask(SIG_SETMASK, &empty, NULL);
	for (int j = 0; j < 10; j++) {
		pause();
		//printf("SLEEP NAO(%d)", getpid());
		//sleep(1);
	}

	exit(EXIT_SUCCESS);
}

static volatile sig_atomic_t v_ATOMIC = 0;

void
v_hand(int sig)
{
	pid_t child;
	int cstat;
	if (sig != SIGCHLD)
		switch(child = fork()) {
		case -1:
			errExit("v_hand::fork\n");
		case 0:
			printf("SIG:%d\n", sig);
			_exit(EXIT_SUCCESS);
		default:
			waitpid(child,&cstat,0);
			if (sig == SIGTSTP)
				v_ATOMIC = 1;
			break;
		}
}


int
main(int argc, char *argv[])
{
	//__22_1__main(argc,argv);
	//__22_2__main(argc, argv);
	sigset(SIGINT,v_hand);
	sigset(SIGCHLD,v_hand);
	sigset(SIGTSTP,v_hand);


	for(int j = 0; j < 10; j++) {
		pause();
		if (v_ATOMIC == 1) {

		}
	}
}
