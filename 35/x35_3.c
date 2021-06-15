/*
 * x35_3.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sched.h>

#define tits_per_second sysconf(_SC_CLK_TCK)
#define sync_time (tits_per_second/4)

int
x35_c__main(int argc, char *argv[])
{
	struct sched_param sp;
	struct tms start, current;
	pid_t child;
	pid_t parent;
	pid_t signal_pid;
	int elapsed_seconds;
	int alert_passed;
	long int computed = 0;
	sp.sched_priority = 5;
	cpu_set_t cpu;

	parent = getpid();
	CPU_ZERO(&cpu);
	CPU_SET(1,&cpu);

	setbuf(stdin,NULL);

	switch (child = fork()) {
	case 0:
		signal_pid = parent;
		break;
	default:
		signal_pid = child;
		break;
	}
	elapsed_seconds = 0;
	alert_passed = 0;

	// Make sure I set the scheduler for both pids
	if (sched_setscheduler(getpid(),SCHED_FIFO,&sp) == -1)
			errExit("sched_setscheduler\n");

	// I had to lock it down to one cpu
	sched_setaffinity(getpid(),sizeof(cpu),&cpu);

	times(&start);

	while (elapsed_seconds < 3) {
		times(&current);

		computed = (current.tms_utime - start.tms_utime) + (current.tms_stime - start.tms_stime);

		if ((computed)/sync_time > alert_passed) {
			printf("DONG %ld - %ld\n", (long)getpid(), computed);
			alert_passed++;
		}

		if ((computed)/tits_per_second > elapsed_seconds) {
			elapsed_seconds++;
			/* (Alternatively, the processes could raise each other’s scheduling priority using sched_setparam().)*/
			// I had to do both? But I did not have to set it to a different value.
			/*
			  [root@localhost Debug]# ./35
									DONG 32405 - 25
									DONG 32405 - 50
									DONG 32405 - 75
									DONG 32405 - 100
									DONG 32406 - 25
									DONG 32406 - 50
									DONG 32406 - 75
									DONG 32406 - 100
									DONG 32405 - 125
									DONG 32405 - 150
									DONG 32405 - 175
									DONG 32405 - 200
									DONG 32406 - 125
									DONG 32406 - 150
									DONG 32406 - 175
									DONG 32406 - 200
									DONG 32405 - 225
									DONG 32405 - 250
									DONG 32405 - 275
									DONG 32405 - 300
									DONG 32406 - 225
									DONG 32406 - 250
									DONG 32406 - 275
									DONG 32406 - 300

			 */
			sched_yield();
			sched_getparam(signal_pid,&sp);
			sched_setparam(signal_pid,&sp);

		}
	}

	if (child == 0)
		_exit(EXIT_SUCCESS);
	else
		waitpid(child,NULL,0);
	return 0;

}
