/*
 * logging.h
 *
 *  Created on: May 30, 2021
 *      Author: cory
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <sys/inotify.h>

#define __20_1_FILE_FLAGS (O_WRONLY | O_CREAT | O_APPEND | O_DSYNC)
#define __20_1_FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

typedef enum {
	PERM
}LOGERROR;

int init_log(char *log_file);
void log_event(int (*bang)(void), struct inotify_event *event, char *directory);

#endif /* LOGGING_H_ */
