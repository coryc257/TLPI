/*
 * utmpx_login.c
 *
 *  Created on: Jun 21, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <time.h>
#include <lastlog.h>
#include <utmpx.h>
#include <paths.h>
#include <fcntl.h>
#include <string.h>
#include "tlpi_pwd.h"
#include "tlpi_hdr.h"

int
utmpx_login(int argc, char *argv[])
{
	struct lastlog llog;
	struct utmpx ut;
	char *devName;
	uid_t user;
	int llfd;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s username [sleep-time]\n", argv[0]);

	/* Initialize login record for utmp and wtmp files */

	memset(&ut, 0, sizeof(struct utmpx));
	memset(&llog, 0, sizeof(struct lastlog));
	ut.ut_type = USER_PROCESS;
	strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));

	user = userIdFromName(argv[1]);
	if (time((time_t *) &ut.ut_tv.tv_sec) == -1)
		errExit("time\n");
	if (time((time_t *) &llog.ll_time))
	ut.ut_pid = getpid();

	devName = ttyname(STDIN_FILENO);
	if (devName == NULL)
		errExit("ttyname");
	if (strlen(devName) <= 8)
		fatal("Terminal name is too short: %s", devName);

	strncpy(ut.ut_line, devName + 5, sizeof(ut.ut_line));
	strncpy(llog.ll_line, devName + 5, sizeof(llog.ll_line));
	strncpy(ut.ut_id, devName + 8, sizeof(ut.ut_id));

	printf("Appending Last Login\n");
	if ((llfd = open(_PATH_LASTLOG, O_RDWR, NULL)) == -1)
		errExit("open\n");
	if ((lseek(llfd, user * sizeof(struct lastlog), SEEK_SET)) == -1)
		errExit("lseek\n");
	if (write(llfd, &llog, sizeof(struct lastlog)) != sizeof(struct lastlog))
		errExit("write\n");
	close(llfd);

	if (pututxline(&ut) == NULL)
			errExit("pututxline\n");
		updwtmpx(_PATH_WTMP, &ut);

	printf("Creating login entries in utmp and wtmp\n");
	printf("        using pid %ld, line %.*s, id %.*s\n",
			(long) ut.ut_pid, (int) sizeof(ut.ut_line), ut.ut_line,
			(int) sizeof(ut.ut_id), ut.ut_id);

	sleep(2);

	ut.ut_type = DEAD_PROCESS;
	time((time_t *) &ut.ut_tv.tv_sec);
	memset(&ut.ut_user, 0, sizeof(ut.ut_user));
	printf("Creating logout entries in utmp and wtmp\n");
	setutxent();
	if (pututxline(&ut) == NULL)
		errExit("pututxline\n");
	updwtmpx(_PATH_WTMP, &ut);
	endutxent();

	printf("HOST:%s\n", ut.ut_host);
	exit(EXIT_SUCCESS);

}
