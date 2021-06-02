/*
 * system.c
 *
 *  Created on: Jun 1, 2021
 *      Author: cory
 */


#include <sys/types.h>
#include <signal.h>
#include "lib/vsignal.h"
#include <unistd.h>
#include <string.h>
void (*sigset(int sig, void (*handler)(int)))(int)
{
	struct sigaction prev,new;

	memset(&prev,0,sizeof(prev));
	if (sigaction(sig, NULL, &prev) == -1)
		return (void(*)(int))-1;

	new.sa_handler = handler;
	new.sa_flags = prev.sa_flags;
	new.sa_mask = prev.sa_mask;

	if(sigaction(sig,&new,NULL) == -1)
		return (void(*)(int))-1;

	return prev.sa_handler;
}

int
sighold(int sig)
{
	sigset_t mask;
	if (sigprocmask(SIG_SETMASK, NULL, &mask) == -1)
		return -1;
	sigaddset(&mask, sig);
	return sigprocmask(SIG_SETMASK, &mask, NULL);
}

int
sigrelse(int sig)
{
	sigset_t mask;
	if (sigprocmask(SIG_SETMASK, NULL, &mask) == -1)
		return -1;
	sigdelset(&mask,sig);
	return sigprocmask(SIG_SETMASK, &mask, NULL);
}

int
sigignore(int sig)
{
	sigset_t mask;
	sigemptyset(&mask);

	if (sigprocmask(SIG_IGN, NULL, &mask) == -1)
		return -1;
	sigaddset(&mask,sig);
	return sigprocmask(SIG_IGN,&mask,NULL);
}

int
sigpause(int sig)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, sig);
	return sigsuspend(&mask);
}

