/*
 * 6.c
 *
 *  Created on: Jun 6, 2021
 *      Author: cory
 */

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>


void handler(int sig)
{
	printf("GOT SIG:%d\n",sig);
}
void
x27_6(int argc, char *argv[])
{
	sigset_t closed, open;
	struct sigaction act;
	pid_t child;

	sigfillset(&closed);
	sigemptyset(&open);
	sigfillset(&(act.sa_mask));

	act.sa_handler = handler;
	act.sa_flags = 0;

	sigprocmask(SIG_SETMASK,&closed,NULL);

	sigaction(SIGCHLD,&act,NULL);

	switch (child = fork()){
	case -1:
		printf("Error:fork\n");
		exit(EXIT_FAILURE);
	case 0:
		_exit(EXIT_SUCCESS);
	default:
		sleep(2);
		waitpid(child,NULL,0);
		printf("Child Exit\n");
		break;
	}
	sigprocmask(SIG_SETMASK,&open,NULL);

	pause();

	exit(EXIT_SUCCESS);

}
