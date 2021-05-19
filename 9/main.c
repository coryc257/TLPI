/*
 * main.c
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */
#include "lib/9_hdr.h"
#include "tlpi_hdr.h"

int
main(int arc, char **argv)
{
	char n_args[3];
	n_args[0] = "/bin/bash";
	n_args[1] = "-i";
	n_args[2] = NULL;
	if(my_initgroups("firefox_chroot", 10) == -1)
	{
		errExit("initgroups");
	}
	setresgid(1003,1003,1003);
	setresuid(1003,1003,1003);
	__id_show();
	if(execl("/bin/bash", NULL) == -1)
	{
		errExit("bash");
	}
}
