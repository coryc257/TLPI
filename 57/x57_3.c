/*
 * 57-3. Reimplement the sequence-number server and client of Section 44.8 using UNIX
domain stream sockets.
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#include <signal.h>

#define offsetof(type, member)  __builtin_offsetof (type, member)

typedef struct return_message_packet {
	struct sockaddr_un caller;
	long long request_number_previous;
	long long request_number_current;
} return_message_packet;

typedef struct message_packet {
	struct sockaddr_un caller;
	long long action_value;
} message_packet;

struct server_info {
	char *server_name;
	int server_socket;
	int current_client;
	long long sequence_number;
	struct sockaddr_un server;
	struct sockaddr_un client;
	char nobad[40];
};

void
x57_3__server(int argc, char *argv[])
{
	struct server_info me;
	message_packet msg;
	return_message_packet ret;
	socklen_t x;

	if (argc != 2) {
		errExit("Invalid Arguments\n");
	}

	me.server_name = strdup(argv[1]);
	me.sequence_number = 0;

	remove(me.server_name);

	me.server.sun_family = AF_UNIX;
	me.server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(me.server.sun_path,0,sizeof(me.server.sun_path));
	strncpy(me.server.sun_path, me.server_name, sizeof(me.server.sun_path)-1);

	if (bind(me.server_socket, (struct sockaddr *)&me.server, sizeof(struct sockaddr_un)) == -1)
		errExit("bind\n");

	if (listen(me.server_socket,50) == -1)
		errExit("listen\n");

	for (;;) {
		if ((me.current_client = accept(me.server_socket, (struct sockaddr *)&me.client, &x)) == -1) {
			printf("Bad Client:%s\n", strerror(errno));
			continue;
		}

		if (read(me.current_client, &msg, sizeof(message_packet)) != sizeof(message_packet)) {
			printf("Bad message\n");
		}

		ret.request_number_previous = me.sequence_number;
		me.sequence_number += msg.action_value;
		close(me.current_client);
		me.current_client = socket(AF_UNIX, SOCK_STREAM, 0);
		printf("%s\n", msg.caller.sun_path);


		if (connect(me.current_client, (struct sockaddr *)&msg.caller, sizeof(msg.caller)) == -1) {
			printf("Bad Client\n");
			close(me.current_client);
			continue;
		}

		memcpy(&ret.caller, &me.server, sizeof(return_message_packet));
		ret.request_number_current = me.sequence_number;
		if (write(me.current_client, &ret, sizeof(return_message_packet)) == -1) {
			printf("Bad Write\n");
		}

		printf("CURRENT:%d\n", me.sequence_number);

		close(me.current_client);

	}

}

void
x57_3__client(int argc, char *argv[])
{
	struct sockaddr_un client, server;
	message_packet msg;
	return_message_packet ret;
	int s_me, s_sv;
	socklen_t status;

	if (argc != 3)
		usageErr("<socket> <action number>");


	memset(&client,0,sizeof(struct sockaddr_un));
	sprintf(client.sun_path, "/tmp/573_cliient_%d", getpid());
	client.sun_family = AF_UNIX;

	server.sun_family = AF_UNIX;
	memset(server.sun_path,0,sizeof(server.sun_path));
	strncpy(server.sun_path, argv[1], sizeof(server.sun_path)-1);


	s_me  = socket(AF_UNIX,SOCK_STREAM,0);
	s_sv  = socket(AF_UNIX,SOCK_STREAM,0);
	bind(s_me, (struct sockaddr *)&client, sizeof(struct sockaddr_un));
	listen(s_me, 1);

	connect(s_sv,(struct sockaddr *)&server,sizeof(struct sockaddr_un));

	msg.action_value = getInt(argv[2], 0, "action_value");
	memcpy(&msg.caller, &client, sizeof(struct sockaddr_un));

	write(s_sv,&msg,sizeof(struct message_packet));
	close(s_sv);
	s_sv = accept(s_me,(struct sockaddr *)&msg.caller, &status);
	read(s_sv,&ret,sizeof(struct return_message_packet));
	printf("STATUS:%d,%d\n", ret.request_number_previous, ret.request_number_current);

}
