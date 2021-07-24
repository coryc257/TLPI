/*
 * main.c
 *
 *  Created on: Jul 22, 2021
 *      Author: cory
 */


#include "daemonize.h"
#include "inet_sockets.h"
#include "tlpi_hdr.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define ECHO_PORT "545432"

typedef struct __server {
	int tcp;
	int udp;
	int pol;

	struct epoll_event tcp_watcher;
	struct epoll_event udp_watcher;
	struct epoll_event current;


} __SERVER;

static __SERVER SERVER;

static void
__main__iteration(void)
{

	switch(epoll_wait(SERVER.pol,&SERVER.current, 1, 5000)) {
	case -1:
		errExit("epoll_wait\n");
		break;
	case 1:
		((void (*)(void))SERVER.current.data.ptr)();
		break;
	default :
		break;
	}
}


static void
__exit__(void)
{
	close(SERVER.tcp);
	close(SERVER.udp);
}


static void
handle_tcp(void)
{
	int client;
	struct sockaddr_storage sender;
	socklen_t size;
	char emsg[4096];
	size_t numRead;

	size = sizeof(struct sockaddr_storage);
	client = accept(SERVER.tcp, (struct sockaddr *)&sender, &size);

	numRead = read(client, emsg, 4096);
	write(client,emsg,numRead);

	close(client);

}

static void
handle_udp(void)
{
	struct sockaddr_storage sender;
	socklen_t size;
	char emsg[4096];
	size_t numRead;
	size = sizeof(struct sockaddr_storage);

	numRead = recvfrom(SERVER.udp, emsg, 4096, 0, (struct sockaddr *)&sender, &size);

	sendto(SERVER.udp, emsg, numRead, 0, (struct sockaddr *)&sender, size);
}

static void
__init__(void)
{
	sigset_t block;

	int flags;

	// BLOCK SIGNALS
	sigfillset(&block);
	sigprocmask(SIG_SETMASK,&block,NULL);

	// EDGE TRIGGERED EVENTS
	SERVER.tcp_watcher.events = EPOLLIN | EPOLLET;
	SERVER.udp_watcher.events = EPOLLIN | EPOLLET;

	// OPEN SERVER SOCKETS
	SERVER.tcp = inetListen(ECHO_PORT, 5, NULL);
	SERVER.udp = inetBind(ECHO_PORT, SOCK_DGRAM, NULL);
	SERVER.tcp_watcher.data.ptr = &handle_tcp;
	SERVER.udp_watcher.data.ptr = &handle_udp;


	// NON BLOCK TCP
	flags = fcntl(SERVER.tcp, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(SERVER.tcp, F_SETFL, flags);

	// NON BLOCK UDP
	flags = fcntl(SERVER.udp, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(SERVER.udp, F_SETFL, flags);

	// SET UP POLLER
	SERVER.pol = epoll_create(2);


	// Function Pointer based polling, if this needed additional information it could actually
	// be a pointer to a struct{fp, data}
	epoll_ctl(SERVER.pol, EPOLL_CTL_ADD, SERVER.tcp, &(SERVER.tcp_watcher));
	epoll_ctl(SERVER.pol, EPOLL_CTL_ADD, SERVER.udp, &(SERVER.udp_watcher));

	for (;;) {
		__main__iteration();
	}
}





int
main(int argc, char *argv[])
{
	atexit(__exit__);
	//DAEMONIZE(0);
	__init__();
}
