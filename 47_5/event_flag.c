/*
 * event_flag.c
 *
 *  Created on: Jul 3, 2021
 *      Author: cory
 */

#include <sys/stat.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "lib/event_flag.h"

static key_t
key_flag(const char* key, size_t length)
{
  size_t i = 0;
  int hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return (key_t)hash;
}

EVENT_FLAG *
initEventFlag(const char *flag_name)
{
	EVENT_FLAG *ret;
	key_t flag_key;
	int handle;

	ret = calloc(1,sizeof(EVENT_FLAG));
	flag_key = key_flag(flag_name, strlen(flag_name));
	handle = semget(flag_key,1, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL);

	if (handle == -1) {
		free(ret);
		return NULL;
	}

	ret->key = flag_key;
	ret->handle = handle;
	clearEventFlag(ret);

	return ret;
}

EVENT_FLAG *
getEventFlag(const char *flag_name)
{
	EVENT_FLAG *ret;
	key_t flag_key;
	int handle;

	ret = calloc(1,sizeof(EVENT_FLAG));
	flag_key = key_flag(flag_name, strlen(flag_name));
	handle = semget(flag_key,1, S_IRUSR | S_IWUSR);

	if (handle == -1) {
		free(ret);
		return NULL;
	}

	ret->key = flag_key;
	ret->handle = handle;

	return ret;
}

void
terminateEventFlag(EVENT_FLAG **flag)
{
	semctl((*flag)->handle, 1, IPC_RMID);
	free(*flag);
	*flag = NULL;
}

EBoolean
clearEventFlag(EVENT_FLAG *flag)
{
	if (semctl(flag->handle,0,SETVAL,1) == -1)
		return eb_false;
	return eb_true;
}

EBoolean
setEventFlag(EVENT_FLAG *flag)
{
	if (semctl(flag->handle,0,SETVAL,0) == -1)
		return eb_false;
	return eb_true;
}

EBoolean
waitForEventFlag(EVENT_FLAG *flag)
{
	struct sembuf x[1];
	x[0].sem_flg = 0;
	x[0].sem_num = 0;
	x[0].sem_op = 0;
	if (semop(flag->handle,x,1) == -1)
		return eb_false;
	return eb_true;
}

EBoolean
getFlagState(EVENT_FLAG *flag)
{
	int ret;
	ret = semctl(flag->handle,0,GETVAL,NULL);
	if (ret == 0)
		return eb_true;
	return eb_false;
}
