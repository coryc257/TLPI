/*
 * login.c
 *
 *  Created on: Jun 21, 2021
 *      Author: cory
 */


#define _GNU_SOURCE
#include <time.h>
#include <lastlog.h>
#include <utmpx.h>
#include <utmp.h>
#include <paths.h>
#include <fcntl.h>
#include <string.h>
#include "tlpi_pwd.h"
#include "tlpi_hdr.h"

char *
yank_line(char *line)
{
	char x, *ret;
	x = *(line + 4);
	*(line + 4) = '\0';
	if (strcmp(line, "/dev") == 0)
		ret = strdup(line + 5);
	else
		ret = strdup(line);

	*(line + 4) = x;
	return ret;
}

void
my_login(const struct utmp *ut)
{
	struct utmp *local;
	char *line;
	local = malloc(sizeof(struct utmp));
	memcpy(local, ut, sizeof(struct utmp));

	local->ut_type = USER_PROCESS;
	local->ut_pid = getpid();
	local->ut_line[0] = '\0';

	if (isatty(STDIN_FILENO)) {
		line = yank_line(ttyname(STDERR_FILENO));
	} else if (isatty(STDOUT_FILENO)) {
		line = yank_line(ttyname(STDERR_FILENO));
	} else if (isatty(STDERR_FILENO)) {
		line = yank_line(ttyname(STDERR_FILENO));
	}

	if (line != NULL && strlen(line) < 32)
		strcpy(local->ut_line, line);
	else
		strcpy(local->ut_line, "???");

	if (pututline(local) == NULL)
		errExit("pututline\n");


}
