/*
 * main.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

sigjmp_buf noway;

void
abort(void)
{
	raise(3);
	exit(EXIT_FAILURE);
}


void
handler(int sig)
{
	if(sig == 3)
		siglongjmp(noway, 4);
	return;
}

int
main(int argc, char *argv[])
{

	struct sigaction act;
	sigemptyset(&(act.sa_mask));
	act.sa_flags=0;
	act.sa_handler = handler;

	for (int j = 1; j < NSIG; j++)
	{
		if (j == SIGTSTP)
			continue;
		else
			sigaction(j,&act,NULL);
	}


	keep_alive:
	if(sigsetjmp(noway,1) != 0)
	{
		printf("NO KILL ME BRUH\n");
		goto keep_alive;
	}
	for(;;) {
		sleep(1);
		printf("STILL KIKCKING\n");
		abort();
	}
}
