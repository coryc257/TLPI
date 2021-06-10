/*
 * main.c
 *
 *  Created on: Jun 10, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SIG_THREAD_PARAM(x) (sig_thread_param*)x

typedef struct sig_thread_param {
	int signal;
	int sync;
} sig_thread_param;



static int
logic_lock(int action, int sync)
{
#define LOGIC_LOCKS 100
static pthread_mutex_t logic_lock_mtx = PTHREAD_MUTEX_INITIALIZER;
static int items[LOGIC_LOCKS];
	int ret = 0;
	if(--sync >= LOGIC_LOCKS || sync < 0)
		return -1;
	pthread_mutex_lock(&logic_lock_mtx);
	switch (action) {
	case 0:
		items[sync] = 1;
		break;
	case 1:
		ret = items[sync];
		break;
	case -1:
		items[sync] = -1;
		break;
	}


	pthread_mutex_unlock(&logic_lock_mtx);
	return ret;
}

void *
sig_watcher(void *arg)
{
	static pthread_mutex_t output = PTHREAD_MUTEX_INITIALIZER;
	sig_thread_param *param = SIG_THREAD_PARAM(arg);
	sigset_t block, open;
	struct sigaction act;
	int status = 0;
	sigfillset(&block);
	sigemptyset(&open);
	sigaddset(&open,param->signal);
	sigfillset(&(act.sa_mask));

	pthread_sigmask(SIG_SETMASK,&block,NULL);
	logic_lock(0,param->sync);

	sigwait(&open,&status);
	pthread_mutex_lock(&output);
	if (param->sync == 2)
		sleep(2);
	printf("(%d)GOT SIGNAL:%d\n", param->sync, status);
	pthread_mutex_unlock(&output);

	logic_lock(-1,param->sync);
	return NULL;
}

int
main (int argc, char *argv[])
{
#define READY_COUNT 2
	pthread_t t1, t2;
	sig_thread_param *param;

	param = malloc(sizeof(sig_thread_param));
	param->signal = SIGUSR1;
	param->sync = 1;
	pthread_create(&t1,NULL,sig_watcher,(void*)param);
	pthread_detach(t1);

	param = malloc(sizeof(sig_thread_param));
	param->signal = SIGUSR2;
	param->sync = 2;
	pthread_create(&t2,NULL,sig_watcher,(void*)param);
	pthread_detach(t2);

	for (;;) {
		int total_ready = 0;
		for (int j = 0; j < READY_COUNT; j++) {
			if (logic_lock(1,j+1) == 1)
				total_ready++;
		}
		if (total_ready == READY_COUNT)
			break;
	}
	printf("THREADS READY\n");

	pthread_kill(t1,SIGUSR1);
	pthread_kill(t2,SIGUSR1); // This won't trigger
	pthread_kill(t2,SIGUSR2);

	for (;;) {
		int total_done = 0;
		for (int j = 0; j < READY_COUNT; j++) {
			if (logic_lock(1,j+1) == -1)
			{
				total_done++;
				printf("Thread(%d):Finished\n", j+1);
			}
			else
			{
				printf("Thread(%d):Still Running\n", j+1);
			}
		}
		if (total_done == READY_COUNT)
			break;
		sleep(1);
	}
	printf("ALL DONE\n");
}
