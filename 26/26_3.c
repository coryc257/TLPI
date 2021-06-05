/*
 * 26_3.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
extern int errno;

x26_3__main(int argc, char *argv[])
{
	pid_t child;
	siginfo_t info;

	memset(&info,0,sizeof(siginfo_t));
	switch(child = fork()) {
	case -1:
		printf("BOOM FORK\n");
		exit(EXIT_FAILURE);
		break;
	case 0:
		sleep(5);
		_exit(EXIT_SUCCESS);
		break;
	default:
		for (;;) {
			if(waitid(P_PID, child, &info, WEXITED | WNOHANG)==-1)
			{

				printf("Fail: waitid(%d)\n",errno);
				exit(EXIT_FAILURE);
			}
			if (info.si_pid != 0)
			{
				printf("Child(%d) Stopped with Status(%d)\n", info.si_pid, info.si_status);
				break;
			}
		}
		break;
	}
}
