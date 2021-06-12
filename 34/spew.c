/*
 * spew.c
 *
 *  Created on: Jun 11, 2021
 *      Author: cory
 */

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; //SYNC
static sig_atomic_t ready = 0;

void
fork_action (int sig)
{
	//printf("DING\n");
	ready++;
}
void calculate_sleep(struct timespec *xsleep)
{
	#define SLEEP_LEN 250000000
	#define NANO_MAX 1000000000
	/*clock_gettime(CLOCK_REALTIME, &*xsleep);
	if (xsleep->tv_nsec + SLEEP_LEN >= 1000000000) {
		xsleep->tv_sec++;
		xsleep->tv_nsec = xsleep->tv_nsec + SLEEP_LEN - NANO_MAX;
	} else {
		xsleep->tv_nsec += SLEEP_LEN;
	}*/
	xsleep->tv_nsec = SLEEP_LEN;
	xsleep->tv_sec = 0;
}

static pid_t
__spew(char *execme, int sync)
{
	pid_t child = 0;
	sigset_t mask, og;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK,&mask,&og);

	if (execme != NULL)
		child = fork();
	if (child == 0)
	{

			if (execme != NULL && sync)
			{
				execl(execme, execme, "34-2-spew", (char*)NULL);
			}
			struct sigaction act;
			pthread_cond_t *condition;
			struct timespec xsleep, remain;
			pid_t me;
			volatile int step;
			act.sa_flags = 0;
			sigfillset(&(act.sa_mask));
			act.sa_handler = fork_action;
			sigaction(SIGUSR1,&act,NULL);
			sigdelset(&mask,SIGUSR1);
			sigprocmask(SIG_SETMASK,&mask,NULL);

			for (;;) {
				me = getpgrp();
				printf("SPEW PROCESS GROUP:%lu at step %d\n",(long unsigned int)me, ready);
				calculate_sleep(&xsleep);
				nanosleep(&xsleep,NULL);
				if (ready >= 2)
					break;
				/*if (step == 0 && ready)
					spew_fork(condition);*/
			}

		printf("Thread Exiting\n");
	}
	sigprocmask(SIG_SETMASK,&og,NULL);
	return child;
}

pid_t
spew(char *arg, int sync)
{
	return __spew(arg,sync);
}
