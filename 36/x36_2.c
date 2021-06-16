/*
 * x36_2.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include "exec_container.h"
#include "tlpi_hdr.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

int
x36_2__main(int argc, char *argv[])
{
	EXEC_CONTAINER ec;
	pid_t child;
	struct rusage ru;

	if (argc < 3)
		usageErr("%s command [[arg] ...]\n");

	construct_args(argc-1, &(argv[1]),&ec);

	switch (child = fork()) {
	case 0:
		execv(ec.exe, ec.args);
		printf("DEAD\n");
		break;
	default:
		waitpid(child,NULL,0);
		getrusage(RUSAGE_CHILDREN, &ru);
		fflush(stdout);
		printf("\n--------------\n\nTIME:sys(%ld.%ld);usr(%ld.%ld)\n", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec, ru.ru_utime.tv_sec, ru.ru_utime.tv_usec);
		break;
	}
}
