/*
 * x23_3.c
 *
 *  Created on: Jun 3, 2021
 *      Author: cory
 */


#include <signal.h>
#include <time.h>

void
siggywiggy(int sig)
{
	printf("SIG:%d\n",sig);
}

int
x23_3(int argc, char *argv[])
{
	sigset_t x,y;
	struct sigaction act;
	timer_t tt;
	struct itimerspec ts;

	memset(&ts,0,sizeof(struct itimerspec));
	memset(&act,0,sizeof(struct sigaction));

	ts.it_interval.tv_sec = 5;
	ts.it_value.tv_sec = 1;

	sigfillset(&x);
	sigemptyset(&y);
	sigprocmask(SIG_SETMASK,&x,NULL);

	act.sa_handler = siggywiggy;
	sigfillset(&(act.sa_mask));

	sigaction(SIGALRM,&act,NULL);

	timer_create(CLOCK_REALTIME,NULL,&tt);
	timer_settime(tt,0,&ts,NULL);
	sigprocmask(SIG_SETMASK,&y,NULL);
	for(;;) {
		pause();
	}
}
