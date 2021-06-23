/*
 * my_who.c
 *
 *  Created on: Jun 23, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <utmpx.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
static void
output_record(const struct utmpx *x)
{
	char time_str[2000];
	struct tm *z;
	time_t r = x->ut_tv.tv_sec;
	z = localtime(&r);
	strftime(time_str,2000,"%F %I:%M:%S %p",z);

	printf("%-33s%10s %s\n", x->ut_user, x->ut_line, time_str);
}

void
who()
{
	struct utmpx *record;
	setutxent();

	while ((record = getutxent()) != NULL) {
		if (record->ut_type == USER_PROCESS) {
			output_record(record);
		}
	}
	endutxent();
}
