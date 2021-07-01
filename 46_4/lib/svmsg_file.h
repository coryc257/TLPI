/*
 * svmsg_file.h
 *
 *  Created on: Jun 30, 2021
 *      Author: cory
 */

#ifndef LIB_SVMSG_FILE_H_
#define LIB_SVMSG_FILE_H_

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h> // For definition of offsetof()
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

#define WELL_KNOWN_SERVER "/home/cory/ipc_server.key"
#define SERVER_KEY 0x1aaaaaa1

struct requestMsg {
	long mtype;
	int clientId;
	char pathname[PATH_MAX];
};

#define REQ_MSG_SIZE ((offsetof(struct requestMsg, pathname) - offsetof(struct requestMsg, clientId)) + (sizeof(char)*PATH_MAX))
#define RESP_MSG_SIZE 8192

struct responseMsg {
	long mtype;
	char data[RESP_MSG_SIZE];
};

#define RESP_MT_FAILURE 	1
#define RESP_MT_DATA		2
#define RESP_MT_END			3

#endif /* LIB_SVMSG_FILE_H_ */
