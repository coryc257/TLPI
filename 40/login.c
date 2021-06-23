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

void __put_ut_line(struct utmp *local) {
	setutent();
	if (pututline(local) == NULL)
		errExit("pututline\n");

	endutent();
}

void
my_login(const struct utmp *ut)
{
	struct utmp *local;
	char *line;
	local = (struct utmp*)ut;

	local->ut_type = USER_PROCESS;
	local->ut_pid = getpid();
	local->ut_line[0] = '\0';
	memset(local->ut_id,0,sizeof(local->ut_id));
	time((time_t *)&local->ut_tv);
	strcpy(local->ut_host, "ubongo");

	if (isatty(STDIN_FILENO)) {
		line = yank_line(ttyname(STDIN_FILENO));
	} else if (isatty(STDOUT_FILENO)) {
		line = yank_line(ttyname(STDOUT_FILENO));
	} else if (isatty(STDERR_FILENO)) {
		line = yank_line(ttyname(STDERR_FILENO));
	}

	if (line != NULL && strlen(line) < 32)
		strcpy(local->ut_line, line);
	else
		strcpy(local->ut_line, "???");
	if (utmpname(_PATH_WTMP) == -1)
		errExit("utmpname\n");
	__put_ut_line(local);
	if (utmpname(_PATH_UTMP) == -1)
		errExit("utmpname\n");
	__put_ut_line(local);

}

int
my_logout(const struct utmp *ut)
{
	struct utmp *found;
	utmpname(_PATH_UTMP);
	setutent();
	if ((found = getutline(ut)) == NULL)
		return 0;
	printf("Logout:%s\n", found->ut_line);

	memset(found->ut_host,0,UT_HOSTSIZE);
	memset(found->ut_user,0,UT_NAMESIZE);
	found->ut_type = DEAD_PROCESS;
	time((time_t *)&found->ut_tv);
	setutent();
	if (pututline(found) == NULL)
		return 0;
	return 1;
}

void
my_logwtmp(const char *line, const char *name, const char *host)
{
	struct utmp new;
	if (strlen(line) > sizeof(new.ut_line)-1 || strlen(name) > sizeof(new.ut_name)-1 || strlen(host) > sizeof(new.ut_host)-1)
		errExit("NO\n");
	strcpy(new.ut_line,line);
	strcpy(new.ut_name,name);
	strcpy(new.ut_host,host);
	new.ut_type = USER_PROCESS;
	time((time_t*)&new.ut_tv);
	new.ut_pid = getpid();
	updwtmp(_PATH_WTMP,&new);

}
