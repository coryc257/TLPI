/*
 * 42_lib_2.c
 *
 *  Created on: Jun 23, 2021
 *      Author: cory
 */

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static char *grobal = NULL;

void make_it(const char *value)
{
	pthread_mutex_lock(&lock);
	if (grobal != NULL) {
		free(grobal);
		grobal = NULL;
	}
	grobal = strdup(value);
	pthread_mutex_unlock(&lock);
}

char *
get_it()
{
	char *ret = NULL;
	pthread_mutex_lock(&lock);
	if (grobal == NULL)
		goto get_it_out;
	ret = strdup(grobal);
	get_it_out:
	pthread_mutex_unlock(&lock);
	return ret;
}
