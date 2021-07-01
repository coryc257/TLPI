/*
 * svmsg_file_server.c
 *
 *  Created on: Jun 30, 2021
 *      Author: cory
 */


#include "../lib/svmsg_file.h"

static void
grimReaper(int sig)
{
	int savedErrno;

	savedErrno = errno;
	while (waitpid(-1, NULL, WNOHANG) == -1)
		continue;
	errno = savedErrno;
}

static void
serveRequest(const struct requestMsg *req)
{
	int fd;
	ssize_t numRead;
	struct responseMsg resp;

	fd = open(req->pathname, O_RDONLY);
	if (fd == -1) {
		resp.mtype = RESP_MT_FAILURE;
		snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
		msgsnd(req->clientId, &resp, strlen(resp.data)+1,0);
		exit(EXIT_FAILURE);
	}

	resp.mtype = RESP_MT_DATA;
	while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0)
		if (msgsnd(req->clientId,&resp,numRead,0) == -1)
			break;

	resp.mtype = RESP_MT_END;
	msgsnd(req->clientId, &resp, 0, 0);
}

static void
exit_handler()
{
	remove(WELL_KNOWN_SERVER);
}

struct x_container {
	int value;
};

static void
write_well_known_file(int serverId)
{
	struct x_container x;
	x.value = serverId;
	int fd;
	remove(WELL_KNOWN_SERVER);
	fd = open(WELL_KNOWN_SERVER, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		errExit("open\n");

	if (write(fd,&x,sizeof(struct x_container)) == -1)
		errExit("write\n");
	close(fd);
}

static void
default_handler(int sig)
{
	exit(EXIT_SUCCESS);
}

int
svmsg_file_server(int argc, char *argv[])
{
	struct requestMsg req;
	struct msqid_ds id;
	pid_t pid;
	ssize_t msgLen;
	int serverId;
	struct sigaction sa;
	// DAEMONIZE
	if (fork() != 0 ) {setsid(); if (fork() != 0) { _exit(EXIT_SUCCESS);}} else {_exit(EXIT_SUCCESS);}

	atexit(exit_handler);
	serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
			S_IRUSR | S_IWUSR | S_IWGRP);

	msgctl(serverId,IPC_STAT,&id);
	write_well_known_file(serverId);

	if (serverId == -1)
		errExit("msgget\n");

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	sa.sa_handler = default_handler;
	for (int j = 0; j < NSIG; j++) {
		sigaction(j,&sa,NULL);
	}
	sa.sa_handler = grimReaper;
	if (sigaction(SIGCHLD,&sa,NULL) == -1)
		errExit("sigaction\n");



	for (;;) {
		msgLen = msgrcv(serverId,&req,REQ_MSG_SIZE,0,0);
		if (msgLen == -1) {
			if (errno == EINTR)
				continue;
			errMsg("msgrcv\n");
			break;
		}

		pid = fork();
		if (pid == -1) {
			errMsg("fork\n");
			break;
		}

		if (pid == 0) {
			serveRequest(&req);
			_exit(EXIT_SUCCESS);
		}
	}

	if (msgctl(serverId, IPC_RMID, NULL) == -1)
		errExit("msgctl");

	exit(EXIT_SUCCESS);

}
