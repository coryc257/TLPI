/*
 * necho.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"

int
__necho__main(int argc, char **argv)
{
	int j;

	for(j = 0; j < argc; j++)
	{
		printf("argv[%d] = %s\n", j, argv[j]);
	}

	exit(EXIT_SUCCESS);
}
