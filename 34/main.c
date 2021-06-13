/*
 * main.c
 *
 *  Created on: Jun 11, 2021
 *      Author: cory
 */
#define _POSIX_C_SOURCE 199309
#include "tlpi_hdr.h"
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

static void x34_3();

typedef struct sig_params {
	pid_t caller;
	int action;
} sig_params;

sig_atomic_t signal_sent = 0;

static void
orphan_handler(int sig, siginfo_t *info, void *ucontext)
{
	printf("OH:%d\n", sig);
	if (sig == SIGUSR1) {
		signal_sent = 1;
	}
}

static void
orphan_process(pid_t parent)
{
	struct sigaction act;
	struct timespec sleeper;
	sig_params param;
	union sigval sgv;
	int fd;

	act.sa_flags = 0 | SA_SIGINFO;
	sigfillset(&(act.sa_mask));
	act.sa_sigaction = orphan_handler;
	sigaction(SIGUSR1,&act,NULL);

	fd = open("./34-7_output.log", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	sleeper.tv_nsec = 250000000;
	sleeper.tv_sec = 0;


	setpgrp(0,0);
	sgv.sival_int = getpid();
	sigqueue(parent,SIGUSR1,sgv);
	for (;;) {
		printf("dink\n");
		nanosleep(&sleeper,NULL);
		if (signal_sent) {
			write(fd, "got signal don't care\n", strlen("got signal don't care\n"));
			break;
		}
	}
	printf("dong\n");

	sgv.sival_ptr = getpid();
	sigqueue(parent,SIGUSR1,sgv);
}

static sig_atomic_t actions = 0;
void
parent_handler(int sig, siginfo_t *info, void *ucontext)
{
	printf("PH:%d\n", sig);
	union sigval v;
	sig_params *params;
	int passer;
	//params = (sig_params*)info->si_value.sival_ptr;
	passer = info->si_value.sival_int;
	printf("GOT PASSER:%d\n",passer);
	//printf("PARAMS:%ld,%d\n", params->caller, params->action);
	if (sig == SIGUSR1)
	{
		if (actions == 0) {
			printf("boink\n");
			v.sival_int = 0;
			sigqueue(passer,SIGTSTP,v);
			sleep(1);
			sigqueue(passer,SIGUSR1,v);
			actions++;
		} else if (actions == 1) {
			_exit(EXIT_SUCCESS);
		}
	}
}

void x34_7()
{
	pid_t parent, orphan;
	struct sigaction act;
	act.sa_flags = 0 | SA_SIGINFO;
	sigfillset(&(act.sa_mask));
	act.sa_sigaction = parent_handler;
	parent = getpid();
	sigaction(SIGUSR1,&act,NULL);
	if (fork() == 0) {
		switch (orphan = fork()) {
		case 0:
			orphan_process(parent);
			break;
		default:
			_exit(EXIT_SUCCESS);
		}
	}
	for (;;) pause();

}

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
	} else if (strcmp(argv[1], "34-6") == 0) {
		int fd, fd2;
		char *x;
		x = malloc(sizeof(char));
		fd = open("/dev/tty", O_RDWR);
		fd2 = open("./output.log", O_CREAT | O_TRUNC | O_RDWR, NULL);

		if (fork() != 0) {
			_exit(EXIT_SUCCESS);
		}

		setpgrp(0,0);
		if (read(fd, x, 1) == -1 && errno ==  EIO)
			write(fd2,"EI EI O\n", strlen("EI EI O\n"));
		_exit(EXIT_SUCCESS);

	} else if (strcmp(argv[1], "34-7") == 0) {
		x34_7();
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
