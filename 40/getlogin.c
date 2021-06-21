/*
 * getlogin.c
 *
 *  Created on: Jun 21, 2021
 *      Author: cory
 */
#define _GNU_SOURCE
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utmpx.h>
#include <string.h>

char *
getlogin()
{
	struct utmpx uact, *ract;
	char *t, ttyn[L_ctermid+1];

	t = ttyname(STDIN_FILENO);

	memset(&uact,0,sizeof(struct utmpx));
	strcpy(uact.ut_line,&t[5]);

	setutxent();
	ract = getutxline(&uact);
	endutxent();

	if (ract == NULL)
		return NULL;

	return ract->ut_user;
}
