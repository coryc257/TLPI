#define _GNU_SOURCE
#include <sys/types.h>
#include <signal.h>
#include  "tlpi_hdr.h"

void
mal_and_zoe(int sig)
{
	printf("GOT:%d\n", sig);
}

__20_3_a__main(int argc, char *argv[])
{
	sigset_t empty, init, go;
	struct sigaction act;

	sigfillset(&init);
	sigprocmask(SIG_SETMASK, &init,NULL);
	sigemptyset(&empty);
	sigemptyset(&go);

	act.sa_handler = mal_and_zoe;
	act.sa_mask = empty;
	act.sa_flags = SA_RESETHAND;

	for (int j = 0; j < NSIG; j++) {
		sigaction(j,&act,NULL);
	}

	sigprocmask(SIG_SETMASK, &go, NULL);

	for (;;) {
		printf("ITERATION\n");
		sleep(5);
	}
}
