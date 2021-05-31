/*
 * sig_sender.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */


#include <signal.h>
#include "tlpi_hdr.h"

int
__sig_sender__main(int argc, char *argv[])
{
	int numSigs, sig, j;
	pid_t pid;

	if (argc < 4 || strcmp(argv[1],"--help") == 0)
		usageErr("%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);

	pid = getLong(argv[1],0,"PID");
	numSigs = getInt(argv[2],0,"num-sigs");
	sig = getInt(argv[3],0,"sig-num");

	/* Send signals to receiver */
	printf("%s: sending signal %d to process %ld %d times\n",
			argv[0], sig, (long)pid, numSigs);

	for (j = 0;  j < numSigs; j++)
		if (kill(pid,sig) == -1)
			errExit("kill\n");

	/* If a fourth command-line argument was specified, send that signal */

	if (argc > 4)
		if (kill(pid, getInt(argv[4],0,"sig-num-2")) == -1)
			errExit("kill2\n");

	printf("%s: exiting\n", argv[0]);
	exit(EXIT_SUCCESS);
}
