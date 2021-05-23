/*
 * main.c
 *
 *  Created on: May 23, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include "../lib/effective_access.h"
#include "../lib/chmod_arx.h"
#include <sys/stat.h>

int
main (char argc, char *argv[])
{
	/*if(effective_access("/home/cory/p_check.txt", R_OK | W_OK | X_OK) == -1)
	{
		errExit("effective_access\n");
	}*/

	do_it("/home/cory/arx.txt");
}
