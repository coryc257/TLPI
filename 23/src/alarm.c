/*
 * alarm.c
 *
 *  Created on: Jun 3, 2021
 *      Author: cory
 */
#include <sys/time.h>
#include "lib/alarm.h"
#include <string.h>

unsigned int
alarm(unsigned int seconds)
{
	static struct itimerval old;
	static struct itimerval new;

	memset(&old,0,sizeof(struct itimerval));
	memset(&new,0,sizeof(struct itimerval));

	new.it_value.tv_sec = seconds;
	if (setitimer(ITIMER_REAL,&new,&old) == -1)
	{
		return 0;
	}

	return old.it_value.tv_sec;
}
