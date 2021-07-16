/*
 * exec_container.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#include <unistd.h>
#include "lib/exec_container.h"
void
construct_args(int argc, char**args, EXEC_CONTAINER *ec)
{
	char **xargs, **cargs;
	ec->exe = strdup(args[0]);
	if (argc > 0) {
		ec->args = malloc(sizeof(char*)*(argc+1));
		xargs = ec->args;
		cargs = args;
		for (int j = 0; j < argc; j++) {
			*xargs = strdup(*cargs);
			xargs++;
			cargs++;
		}
		*cargs = NULL;
	} else {
		ec->args = NULL;
	}
}
