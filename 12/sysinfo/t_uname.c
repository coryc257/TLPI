/*
 * t_uname.c
 *
 *  Created on: May 18, 2021
 *      Author: cory
 */

#define _GNU_SOURCE 1
#include <sys/utsname.h>
#include "tlpi_hdr.h"

int
__t_uname__main()
{
	struct utsname uts;

	if(uname(&uts) == -1)
		errExit("uname");

	printf("Node name:   %s\n", uts.nodename);
	printf("System name: %s\n", uts.sysname);
	printf("Release:     %s\n", uts.release);
	printf("Version:     %s\n", uts.version);
	printf("Machine:     %s\n", uts.machine);
#ifdef _GNU_SOURCE
	printf("Domain Name: %s\n", uts.domainname);
#endif
	return 0;
}
