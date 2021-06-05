/*
 * main.c
 *
 *  Created on: Jun 3, 2021
 *      Author: cory
 */

#include <signal.h>
#include <unistd.h>
#include "lib/alarm.h"

void
h23_1(int sig)
{
	printf("GOT:%d\n",sig);
}

void
f23_1()
{
	sigset_t block, empty;
	struct sigaction act;
	sigfillset(&block);
	sigemptyset(&empty);
	sigprocmask(SIG_SETMASK, &block, NULL);
	sigemptyset(&(act.sa_mask));
	act.sa_handler = h23_1;
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, NULL);
	alarm(10);
	sigprocmask(SIG_SETMASK, &empty, NULL);
	for (;;) {
		pause();
	}
}

int
main(int argc, char *argv[])
{
	//f23_1();
	//t_nanosleep(argc,argv);
	x23_3(argc,argv);
}
