/*
 * interop.c
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */

#include <string.h>
#include <stdio.h>
#include "lib/11.h"
#include "tlpi_hdr.h"


static void
sysconfPrint(const char *msg, int name)
{
	long lim;
	errno = 0;
	lim = sysconf(name);
	if(lim != -1) {
		printf("%s %ld\n", msg, lim);
	} else {
		if(errno == 0)
			printf("%s (indeterminate)\n");
		else
			errExit("sysconf %s\n", msg);
	}
}

static RETURN
t_sysconf(PARAM *obj)
{
	RETURN ret;
	ret.obj = NULL;
	ret.status = 0;

	sysconfPrint("_SC_ARG_MAX:            ", _SC_ARG_MAX);
	sysconfPrint("_SC_LOGIN_NAME_MAX:     ", _SC_LOGIN_NAME_MAX);
	sysconfPrint("_SC_OPEN_MAX:           ", _SC_OPEN_MAX);
	sysconfPrint("_SC_NGROUPS_MAX:        ", _SC_NGROUPS_MAX);
	sysconfPrint("_SC_PAGESIZE:           ", _SC_PAGESIZE);
	sysconfPrint("_SC_RTSIG_MAX:          ", _SC_RTSIG_MAX);

	return ret;
}

static void
fpathconfPrint(const char *msg, int fd, int name)
{
	long lim;

	errno = 0;

	lim = fpathconf(fd, name);
	if(lim != -1) {
		printf("%s %ld\n", msg, lim);
	} else {
		if(errno == 0)
			printf("%s (indeterminate)\n", msg);
		else
			errExit("fpathconf %s", msg);
	}
}

static RETURN
t_tpathconf(PARAM *param)
{
	RETURN ret;
	ret.status = 0;
	ret.obj = NULL;
	fpathconfPrint("_PC_NAME_MAX:           ", STDIN_FILENO, _PC_NAME_MAX);
	fpathconfPrint("_PC_PATH_MAX:           ", STDIN_FILENO, _PC_NAME_MAX);
	fpathconfPrint("_PC_PIPE_BUF:           ", STDIN_FILENO, _PC_NAME_MAX);
	return ret;
}



method
get_method(char *method)
{
	if(strcmp(method, "t_sysconf") == 0)
		return t_sysconf;
	else if(strcmp(method, "t_tpathconf") == 0)
		return t_tpathconf;
}

