/*
 * main.c
 *
 *  Created on: Jun 11, 2021
 *      Author: cory
 */
#define _POSIX_C_SOURCE 199309
#include "tlpi_hdr.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void x34_3();

int
main (int argc, char *argv[])
{
	pid_t child;
	int x34_3_flag = 0;
	if (argc < 2)
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
	} else if (strcmp(argv[1], "34-3") == 0) {
		if (argc == 3)
		{
			x34_3_flag = 1;
		}
		x34_3(x34_3_flag);
	} else if (strcmp(argv[1], "disc_SIGHUP") == 0) {
		char **argv_new = malloc(sizeof(char*)*argc-1);
		char **pos = argv_new;
		*pos = strdup("disc_SIGHUP");
		for (int j = 2; j < argc; j++)
		{
			pos++;
			*pos = strdup(argv[j]);
		}
		disc_SIGHUP(argc-1,argv_new);
	}

}


static pid_t session_child = 0;
static sig_atomic_t sigrec = 0;
static void
x34_3__handler(int sig, siginfo_t *info, void *ucontext)
{
	session_child = (long)info->si_value.sival_ptr;
	sigrec = 1;
}

static void
x34_3(int flag)
{
	pid_t child;
	pid_t parent;
	struct sigaction act;
	act.sa_sigaction = x34_3__handler;
	sigfillset(&(act.sa_mask));
	act.sa_flags = 0 | SA_SIGINFO;

	sigaction(SIGUSR1,&act,NULL);

	parent = getpid();
	switch (child = fork()) {
	case -1:
		errExit("fork\n");
		break;
	case 0:
	{
		pid_t new_session;
		union sigval x;
		if (flag)
			setpgrp(getpid(),0);
		new_session = setsid();
		x.sival_ptr = (void*)new_session;
		sigqueue(parent,SIGUSR1,x);
	}
		break;
	default:
		while (sigrec == 0)
			pause();
		if (session_child == -1)
			printf("New Session Failed!\n");
		else
			printf("New Session: %ld\n", session_child);
		break;
	}
}
