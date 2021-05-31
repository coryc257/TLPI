/*
 * 20_4.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#include <signal.h>
#include <unistd.h>

int my_siginterrupt(int sig, int flag)
{
	struct sigaction act;
	sigaction(sig, NULL, &act);
	act.sa_flags = flag ? act.sa_flags&~SA_RESTART : act.sa_flags|SA_RESTART;
	sigaction(sig,&act,NULL);
}
