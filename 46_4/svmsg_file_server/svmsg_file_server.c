/*
 * svmsg_file_server.c
 *
 *  Created on: Jun 30, 2021
 *      Author: cory
 */


#include "../lib/svmsg_file.h"
#include "daemonize.h"
#include <stdarg.h>
#include <syslog.h>

static int serverId;
static void
catch_fire(const char *message_format, ...)
{
	va_list list;
	openlog("svmsg_file_server", LOG_PID | LOG_PERROR, LOG_USER);
	va_start(list,message_format);
	vsyslog(LOG_USER | LOG_ERR, message_format, list);
	va_end(list);
	closelog();
}

static void
grimReaper(int sig)
{
	int savedErrno;

	savedErrno = errno;
	while (waitpid(-1, NULL, WNOHANG) == -1)
		continue;
	errno = savedErrno;
}

static int __timeout_handler__clientId;
static void
timeout_handler(int sig)
{
	catch_fire("[serveRequest->timeout_handler][timeout for clientId(%d)", __timeout_handler__clientId);
	msgctl(__timeout_handler__clientId, IPC_RMID, NULL);
	signal(sig,SIG_DFL);
	kill(getpid(),sig);
}


static void
serveRequest(const struct requestMsg *req)
{
	int fd;
	ssize_t numRead;
	struct responseMsg resp;
	struct sigaction act;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = timeout_handler;
	__timeout_handler__clientId = req->clientId;
	sigaction(SIGALRM,&act,NULL);

	fd = open(req->pathname, O_RDONLY);
	if (fd == -1) {
		catch_fire("[serveRequest][open('%s',O_RDONLY)]", req->pathname);
		resp.mtype = RESP_MT_FAILURE;
		snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
		// TAG:msgsnd0
		alarm(10);
		if (msgsnd(req->clientId, &resp, strlen(resp.data)+1,0) == -1) {
			catch_fire("[serveRequest::msgsnd0][msgsnd(%d, '%s', %d, %d)]", req->clientId, "irrelevant", strlen(resp.data)+1, 0);
		}
		alarm(0);
		close(fd);
		//exit(EXIT_FAILURE);
		return;
	}

	resp.mtype = RESP_MT_DATA;
	while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
		// TAG:msgsnd1
		alarm(10);
		if (msgsnd(req->clientId,&resp,numRead,0) == -1) {
			alarm(0);
			catch_fire("[serveRequest::msgsnd1][msgsnd(%d, '%s', %d, %d)]", req->clientId, "irrelevant", numRead, 0);
			break;
		}
		alarm(0);
	}

	if (numRead == -1) {
		catch_fire("[serveRequest][read(%d, '%s', %d)]", fd, "irrelevant", RESP_MSG_SIZE);
	}

	resp.mtype = RESP_MT_END;
	// TAG:msgssnd2
	alarm(10);
	if (msgsnd(req->clientId, &resp, 0, 0) == -1) {
		alarm(0);
		catch_fire("[serveRequest::msgsnd2][msgsnd(%d, '%s', %d, %d)]", req->clientId, "irrelevant", 0, 0);
	}
	alarm(0);
	close(fd);
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
	if (sig == SIGTERM || sig == SIGINT) {
		msgctl(serverId, IPC_RMID, NULL);
		exit_handler();
		signal(sig,SIG_DFL);
		kill(getpid(),sig);
	}
	//exit(EXIT_SUCCESS);
}

int
svmsg_file_server(int argc, char *argv[])
{
	struct requestMsg req;
	struct msqid_ds id;
	pid_t pid;
	ssize_t msgLen;

	struct sigaction sa;
	// DAEMONIZE
	DAEMONIZE(0);

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
