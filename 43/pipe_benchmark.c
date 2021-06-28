/*
 * pipe_benchmark.c
 *
 *  Created on: Jun 24, 2021
 *      Author: cory
 */
#define _POSIX_C_SOURCE 199309
#define _XOPEN_SOURCE
#define __USE_XOPEN_EXTENDED
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

struct pipe_def {
	int fd[2];
	int *input;
	int *output;
	pid_t parent;
	pid_t child;
	int block_size;
	int block_count;
};

static struct pipe_def pd;
static sig_atomic_t is_eof = 0;

static void
__sig_h(int sig, siginfo_t *info, void *ucontext)
{
	int pass_value = info->si_int;
	if (sig == SIGUSR1 && pass_value == SIGKILL)
	{
		is_eof = 1;
	}
}

static void
send_data()
{
	char buf[pd.block_size];
	close(*pd.output);
	for (int j = 0; j < pd.block_count; j++) {
		write(*pd.input, buf, pd.block_size);
	}
	close(*pd.input);
}

static void
receive_data()
{
	close(*pd.input);
	int numRead;
	char buf[pd.block_size];

	for (;;) {
		numRead = read(*pd.output,buf,pd.block_size);
		if (numRead == 0)
			break;
		if (numRead == -1)
			break;
		if (numRead != pd.block_size)
			errExit("Invalid Block Length\n");

	}
	close(*pd.output);



	if (is_eof)
		_exit(EXIT_SUCCESS);
	else {
		sigset_t wait_set;
		siginfo_t sinfo;
		struct timespec wait_time;
		wait_time.tv_sec = 2;
		wait_time.tv_nsec = 0;
		sigemptyset(&wait_set);
		sigaddset(&wait_set,SIGUSR1);
		sigtimedwait(&wait_set, &sinfo, &wait_time);
		if (sinfo.si_int || is_eof)
			_exit(EXIT_SUCCESS);
		else
			_exit(EXIT_FAILURE);
	}
}

static void
kill_child()
{
	int status;
	union sigval x;
	x.sival_int = SIGKILL;
	sigqueue(pd.child, SIGUSR1, x);
	waitpid(pd.child,&status,0);
	printf("child exit(%d)\n",status);
}


void
__init(struct pipe_def *pd)
{
	struct timespec start,finish;
	sigset_t block, child_ready;
	struct sigaction act;
	int swap, total_seconds, total_nanoseconds;
	sigfillset(&block);
	sigemptyset(&child_ready);
	sigprocmask(SIG_SETMASK, &block, NULL);
	setbuf(stdout,NULL);
	act.sa_sigaction = __sig_h;
	act.sa_flags = SA_SIGINFO | SA_RESTART;
	sigfillset(&act.sa_mask);
	pd->input = &pd->fd[1];
	pd->output = &pd->fd[0];
	if (pipe(pd->fd) == -1)
		errExit("pipe\n");

	pd->parent = getpid();
	switch (pd->child = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		sigaction(SIGUSR1,&act,NULL);
		sigprocmask(SIG_SETMASK,&child_ready,NULL);
		receive_data();
		break;
	default:
		clock_gettime(CLOCK_REALTIME, &start);
		send_data();
		kill_child();
		clock_gettime(CLOCK_REALTIME, &finish);
		total_seconds = finish.tv_sec-start.tv_sec;
		swap = finish.tv_nsec-start.tv_nsec;
		if (swap < 0)
		{
			total_seconds--;
			total_nanoseconds = 1000000000+swap;
		}
		else
			total_nanoseconds = swap;
		printf("%d.%d => %d.%d\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
		printf("%d.%d\n", total_seconds, total_nanoseconds);
		break;
	}
}

void
pipe_benchmark(int argc, char *argv[])
{
	if (argc != 3)
		usageErr("%s block_size block_count\n", argv[0]);

	pd.block_size = getInt(argv[1], GN_GT_0, "block_size");
	pd.block_count = getInt(argv[2], GN_GT_0, "block_count");
	__init(&pd);
}
