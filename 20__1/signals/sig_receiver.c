#define _GNU_SOURCE
#include <signal.h>
#include "../lib/signal_functions.h"
#include "tlpi_hdr.h"

static int sigCnt[NSIG];
static volatile sig_atomic_t gotSigint = 0;

static void
handler(int sig)
{
	if (sig == SIGINT)
		gotSigint = 1;
	else
		sigCnt[sig]++;
}

int
__sig_receiver__main(int argc, char *argv[])
{
	int n, numSecs;
	sigset_t pendingMask, blockingMask, emptyMask;

	printf("%s: PID is %ld\n", argv[0], (long) getpid());

	for (n = 1; n < NSIG; n++)
		(void) signal(n, handler);

	if (argc > 1) {
		numSecs = getInt(argv[1],0,NULL);

		sigfillset(&blockingMask); // set every one to on
		// Block all current signals in blockingMask
		if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
			errExit("sigprocmask\n");

		printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
		sleep(numSecs);

		if (sigpending(&pendingMask) == -1)
			errExit("sigpending\n");

		printf("%s: pending signals are: \n", argv[0]);
		printSigset(stdout, "\t\t", &pendingMask);

		sigemptyset(&emptyMask);
		if (sigprocmask(SIG_SETMASK,&emptyMask,NULL) == -1)
			errExit("sigprocmask\n");
	}

	while (!gotSigint)
		continue;

	for (n = 0; n < NSIG; n++) {
		if (sigCnt[n] != 0)
			printf("%s: signal %d caught %d time%s\n", argv[0], n,
					sigCnt[n], (sigCnt[0] > 1 ? "s" : ""));
	}

	exit(EXIT_SUCCESS);
}
