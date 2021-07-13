#include <stdio.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#include <signal.h>


#define SERVER_ADDRESS "/tmp/server_57_1"
#define CLIENT_ADDRESS "/tmp/client_57_1"

static sig_atomic_t __server_ready = 0;

static char buffer[10];

static void
server_ready(int sig)
{
	printf("got\n");
	__server_ready = 1;
}

static void
__client(pid_t parent)
{
	int sock_fd;
	struct sockaddr_un server;
	struct sigaction sa;
	sigset_t ready;

	sa.sa_flags = 0 | SA_RESTART;
	sigfillset(&sa.sa_mask);
	sa.sa_handler = server_ready;

	sigemptyset(&ready);
	sigaction(SIGUSR1, &sa, NULL);
	sigprocmask(SIG_SETMASK, &ready, NULL);

	while (!__server_ready) {}


	if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM | O_NONBLOCK, 0)) == -1) {
		kill(parent,SIGINT);
		errExit("socket\n");
	}


	server.sun_family = AF_UNIX;
	strncpy(server.sun_path, SERVER_ADDRESS, sizeof(server.sun_path)-1);

	while (sendto(sock_fd, buffer, 10, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) != -1) {
		printf("sending\n");
	}

	if (errno == EWOULDBLOCK) {
		printf("Confirmed... Exiting\n");
		kill(parent,SIGINT);
	} else {
		kill(parent,SIGINT);
		errExit("unknown\n");
	}

}

static void
__server(pid_t child)
{
	int sock_fd;
	struct sockaddr_un server;
	sigset_t ready;
	server.sun_family = AF_UNIX;
	remove(SERVER_ADDRESS);
	strncpy(server.sun_path, SERVER_ADDRESS, sizeof(server.sun_path)-1);

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (bind(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) == -1) {
		kill(child, SIGINT);
		errExit("bind\n");
	}

	sigemptyset(&ready);
	sigprocmask(SIG_SETMASK, &ready, NULL);
	kill(child, SIGUSR1);
	for(;;) {

	}
}


static int
__main(int argc, char *argv[])
{
	sigset_t block;
	pid_t child, parent;
	sigfillset(&block);
	sigprocmask(SIG_SETMASK, &block, NULL);

	parent = getpid();

	switch(child = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		__client(parent);
		_exit(EXIT_SUCCESS);
		break;
	default:
		__server(child);
		break;
	}

	return EXIT_SUCCESS;
}

void
x57_1(int argc, char *argv[])
{
	if (fork() == 0) {
		exit(__main(argc, argv));
	}
}
