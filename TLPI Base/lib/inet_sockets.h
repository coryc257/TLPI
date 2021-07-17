/*
 * inet_sockets.h
 *
 *  Created on: Jul 17, 2021
 *      Author: cory
 */

#ifndef LIB_INET_SOCKETS_H_
#define LIB_INET_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>

int inetConnect(const char *host, const char *service, int type);
int inetListen(const char *service, int backlog, socklen_t *addrlen);
int inetBind(const char *service, int type, socklen_t *addrlen);
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
		char *addrStr, int addrStrLen);

#define IS_ADDR_STR_LEN 4096
	/* Suggest length for string buffer that caller
	 * should pass to inetAddressStr(). Must be greater
	 * than (NI_MAXHOST + NI_MAXSERV + 4)
	 */

#endif /* LIB_INET_SOCKETS_H_ */
