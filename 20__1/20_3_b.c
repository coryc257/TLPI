/*
 * 20_3_b.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void
__20_3_b_handler(int sig)
{
	printf("GOT:%d\n", sig);
	/*for (;;) {

	}*/
	printf("OUT\n");
}

int
__20_3_b__main(int argc, char *argv[])
{
	sigset_t empty, init, go;
	struct sigaction act;
	sigfillset(&init);
	sigemptyset(&empty);
	sigemptyset(&go);

	act.sa_handler = __20_3_b_handler;
	act.sa_flags = 0;
	act.sa_flags = SA_NODEFER;
	act.sa_mask = init;

	sigprocmask(SIG_SETMASK, &init, NULL);

	for (int j = 0; j < NSIG; j++) {
		sigaction(j, &act, NULL);
	}

	sigprocmask(SIG_SETMASK, &go, NULL);
	for (;;) {
		pause();
	}
}
