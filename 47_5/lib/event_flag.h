/*
 * event_flag.h
 *
 *  Created on: Jul 3, 2021
 *      Author: cory
 */

#ifndef LIB_EVENT_FLAG_H_
#define LIB_EVENT_FLAG_H_

#include <sys/sem.h>

typedef struct event_flag {
	key_t key;
	int handle;
} EVENT_FLAG;

typedef enum {eb_false,eb_true} EBoolean;

EVENT_FLAG *initEventFlag(const char *flag_name);
EVENT_FLAG *getEventFlag(const char *flag_name);
void terminateEventFlag(EVENT_FLAG **flag);
EBoolean clearEventFlag(EVENT_FLAG *flag);
EBoolean setEventFlag(EVENT_FLAG *flag);
EBoolean waitForEventFlag(EVENT_FLAG *flag);

#endif /* LIB_EVENT_FLAG_H_ */
