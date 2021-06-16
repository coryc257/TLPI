/*
 * x36_1.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>

#define tps sysconf(_SC_CLK_TCK)

int
x36_1__main(int argc, char *argv[])
{
	struct rusage r;
	struct tms start, current;
	pid_t child;
	int elapsed_second = 0;
	int computed;

	switch (child = fork()) {
	case 0:
		times(&start);

		while (elapsed_second < 3) {
			times(&current);
			computed = current.tms_stime+current.tms_utime-start.tms_stime-start.tms_utime;
			if (computed/tps > elapsed_second)
			{
				printf("DONG\n");
				elapsed_second++;
			}

		}
		printf("CHILD OUT\n");
		_exit(EXIT_SUCCESS);
	default:
		sleep(5);
		getrusage(RUSAGE_CHILDREN,&r);
		printf("TOTAL USAGE: %lld\n", r.ru_stime.tv_sec + r.ru_stime.tv_usec + r.ru_utime.tv_sec + r.ru_utime.tv_usec);
		waitpid(child,NULL,0);
		getrusage(RUSAGE_CHILDREN,&r);
		printf("TOTAL USAGE AFTER WAIT: %lld\n", r.ru_stime.tv_sec + r.ru_stime.tv_usec + r.ru_utime.tv_sec + r.ru_utime.tv_usec);
		exit(EXIT_SUCCESS);
	}



}
