/*
 * svmsg_file_client.c
 *
 *  Created on: Jun 30, 2021
 *      Author: cory
 */

#include "../lib/svmsg_file.h"
#include <syslog.h>
#include <stdarg.h>

static int clientId;

static void
removeQueue(void)
{
	if (msgctl(clientId, IPC_RMID,NULL) == -1)
		errExit("msgctl\n");
}

struct x_container {
	int value;
};
static key_t
get_server_key()
{
	key_t ret;
	struct x_container x;
	int id;
	int fd;
	fd = open(WELL_KNOWN_SERVER, O_RDONLY, NULL);
	if (fd == -1)
		errExit("open\n");

	if (read(fd,&x,sizeof(struct x_container)) == -1)
		errExit("write\n");
	close(fd);
	return x.value;
}

timeout_handler(int sig)
{
	printf("timout expired\n");
	exit(EXIT_FAILURE);
}

int
svmsg_file_client(int argc, char *argv[])
{
	struct requestMsg req;
	struct responseMsg resp;
	struct sigaction act;
	int serverId, numMsgs;
	ssize_t msgLen, totBytes;
	struct msqid_ds id;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = timeout_handler;
	sigaction(SIGALRM,&act, NULL);

	if (argc != 2 || strcmp(argv[1],"--help") == 0)
		usageErr("%s pathname\n", argv[0]);

	if (strlen(argv[1]) > sizeof(req.pathname)-1)
		cmdLineErr("pathname too long (max: %ld bytes)\n", (long) sizeof(req.pathname)-1);

	//msgctl(24,IPC_STAT,&id);
	//serverId = msgget(IPC_PRIVATE, S_IWUSR);
	serverId = get_server_key();

	if (serverId == -1)
		errExit("msgget - server message queue\n");

	//clientId = 234234;
	clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
	if (clientId == -1)
		errExit("msgget - client message queue\n");

	if (atexit(removeQueue) != 0)
		errExit("atexit\n");

	req.mtype = 1;
	req.clientId = clientId;
	strncpy(req.pathname,argv[1],sizeof(req.pathname)-1);
	req.pathname[sizeof(req.pathname)-1] = '\0';

	alarm(10);
	if (msgsnd(serverId, &req, REQ_MSG_SIZE,0) == -1) {
		alarm(0);
		errExit("msgsnd");
	}
	alarm(0);


	alarm(10);
	msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
	alarm(0);
	if (msgLen == -1)
		errExit("msgrcv\n");

	if (resp.mtype == RESP_MT_FAILURE) {
		printf("%s\n", resp.data);
		if (msgctl(clientId, IPC_RMID, NULL) == -1)
			errExit("msgctl\n");
		exit(EXIT_FAILURE);
	}

	totBytes = msgLen;
	for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
		alarm(10);
		msgLen = msgrcv(clientId,&resp,RESP_MSG_SIZE,0,0);
		alarm(0);
		if (msgLen == -1)
			errExit("msgrcv\n");

		totBytes += msgLen;
	}

	printf("Received %ld bytes (%d messages)\n", (long)totBytes, numMsgs);
	exit(EXIT_SUCCESS);
}
