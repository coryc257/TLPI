/*
 * modify_env.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */


#define _GNU_SOURC
#include <stdlib.h>
#include "tlpi_hdr.h"

extern char **environ;

int
__modify_env__main(int argc, char **argv)
{
	int j;
	char **ep;

	clearenv();				/* Erase the environment */

	for (j = 1; j < argc; j++)
		if(putenv(argv[j]) != 0)
			errExit("putenv: %s", argv[j]);

	if (setenv("GREET", "Hello world",0) == -1)
		errExit("setenv");

	unsetenv("BYE");

	for(ep = environ; *ep != NULL; ep++)
		puts(*ep);

	exit(EXIT_SUCCESS);
}
