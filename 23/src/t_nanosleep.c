/*
 * t_nanosleep.c
 *
 *  Created on: Jun 3, 2021
 *      Author: cory
 */


#define _POSIX_C_SOURCE 199309
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void
sigintHandler(int sig)
{
	return;
}

int
t_nanosleep(int argc, char *argv[])
{
	struct timeval start, finish;
	struct timespec request, remain, real;
	//struct timespec xstart, xcurrent;
	struct sigaction sa;
	clock_t xclock;
	int s;
	long nano_dif;

	if (argc!=3 || strcmp(argv[1],"--help") == 0)
		usageErr("%s <secs> <nanoseconds>\n", argv[0]);

	request.tv_sec = getLong(argv[1], 0, "secs");
	request.tv_nsec = getLong(argv[2], 0, "nano");



	/* Allow SIGINT handler to interrupt nanosleep() */

	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = 0;
	sa.sa_handler = sigintHandler;
	if (sigaction(SIGINT,&sa,NULL) == -1)
		errExit("sigaction\n");

	if (gettimeofday(&start,NULL) == -1)
		errExit("gettimeofday\n");
	//clock_gettime(xclock, &xstart);
	//xclock = clock();

	clock_gettime(CLOCK_REALTIME, &real);
	request.tv_sec += real.tv_sec;
	request.tv_nsec += real.tv_nsec;

	for (;;) {
		//s = nanosleep(&request,&remain);
		s = clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&request,&remain);
		if (s != 0 && s != EINTR)
			errExit("nanosleep\n");

		if (gettimeofday(&finish, NULL) == -1)
			errExit("gettimeofday\n");
		printf("Slept for :%9.6f secs\n",
				finish.tv_sec - start.tv_sec +
				(finish.tv_usec-start.tv_usec)/1000000.0);

		if (s == 0)
			break;
		clock_gettime(CLOCK_REALTIME, &real);
		nano_dif = (long)(request.tv_nsec-real.tv_nsec);
		printf("Remaining: %2ld.%09ld\n", (long) (request.tv_sec-real.tv_sec-(nano_dif < 0 ? 1 : 0)),
				nano_dif < 0 ? 1000000000+nano_dif : nano_dif);

		//request = remain;
	}

	printf("Sleep complete\n");
	exit(EXIT_SUCCESS);
}
