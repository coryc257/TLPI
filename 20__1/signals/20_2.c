#define _GNU_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "tlpi_hdr.h"

void
handler(int sig)
{
	signal(sig, SIG_IGN);
	printf("GOT:%d\n",sig);
}

int
__20_2__main(int argc, char *argv[])
{
	sigset_t sigst, init, go;
	struct sigaction act;

	if (sigemptyset(&go)+sigfillset(&init)+sigemptyset(&sigst) != 0)
		errExit("siggle wiggle\n");
	sigprocmask(SIG_SETMASK, &init, NULL);

	act.sa_flags = 0;
	act.sa_mask = sigst;
	act.sa_handler = handler;

	for (int j = 0; j < NSIG; j++) {
		sigaction(j, &act, NULL);
	}

	sigprocmask(SIG_SETMASK, &go, NULL);

	for (;;) {
		printf("ITERATION\n");
		sleep(5);
	}

	return 0;
}
