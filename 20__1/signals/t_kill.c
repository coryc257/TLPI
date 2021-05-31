/*
 * t_kill.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#include <signal.h>
#include "tlpi_hdr.h"

int
__t_kill__main(int argc, char *argv[])
{
	int s, sig;

	if (argc != 3 || strcmp(argv[1],"--help") == 0)
		usageErr("%s pid sig-num\n", argv[0]);

	sig = getInt(argv[2], 0, "sig-num");
	s = kill(getLong(argv[1],0,"pid"), sig);

	if (sig != 0) {
		if (s == -1)
			errExit("kill\n");
	} else {
		if (s == 0)
			printf("Process exists and we can send it a signal\n");
		else {
			if (errno == EPERM)
				printf("Process exists, but we dont have"
						"permission to send it a signal\n");
			else if (errno == ESRCH)
				printf("Process does not exist\n");
			else
				errExit("kill\n");

		}
	}

	exit(EXIT_SUCCESS);
}
