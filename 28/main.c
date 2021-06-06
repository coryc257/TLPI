/*
 * main.c
 *
 *  Created on: Jun 6, 2021
 *      Author: cory
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

static void
show_metrics(const char *type, struct timespec *start, struct timespec *stop)
{
	__time_t seconds;
	__syscall_slong_t nanos;
	seconds = stop->tv_sec - start->tv_sec - (stop->tv_nsec < start->tv_nsec ? 1 : 0);
	nanos = (stop->tv_nsec < start->tv_nsec ? 1000000000 : 0) + stop->tv_nsec - start->tv_nsec;

	printf("%s:%10ld.%09ld\n", type, seconds, nanos);
}

void type_fork(const char *type, pid_t (*f)(void), int iterations)
{
	struct timespec t_start, t_stop;
	pid_t child;
	clock_gettime(CLOCK_REALTIME, &t_start);
	for (int i = 0; i < iterations; i++) {
		switch (child = (f)()) {
		case -1:
			exit(EXIT_FAILURE);
		case 0:
			_exit(EXIT_SUCCESS);
		default:
			waitpid(child, NULL, 0);
		}
	}
	clock_gettime(CLOCK_REALTIME, &t_stop);
	show_metrics(type, &t_start, &t_stop);
}

int
main(int argc, char *argv[])
{
#define iters 1000000
	type_fork("fork ", fork, iters);
	type_fork("vfork", vfork, iters);
}
