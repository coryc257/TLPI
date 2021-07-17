/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 59-6 */

#define _DEFAULT_SOURCE             /* To get definitions of NI_MAXHOST and
                                   NI_MAXSERV from <netdb.h> */
#include <netdb.h>
#include "is_seqnum.h"
#include "tlpi_hdr.h"
#include "read_line.h"
#include "inet_sockets.h"
#define BACKLOG 50

int
is_seqnum_sv(int argc, char *argv[])
{
	BUFFERED_STREAM_READER reader;
    uint32_t seqNum;
    char reqLenStr[INT_LEN];            /* Length of requested sequence */
    char seqNumStr[INT_LEN];            /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char *d_line;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [init-seq-num]\n", argv[0]);

    seqNum = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    errExit("signal");

    lfd = inetListen("50000", BACKLOG, NULL);

    if (lfd == -1)
    	errExit("Cannot start server\n");


    for (;;) {                  /* Handle clients iteratively */

        /* Accept a client connection, obtaining client's address */

        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            errMsg("accept");
            continue;
        }

        reader = buffered_stream_reader_new(cfd, INT_LEN);

        if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                    host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        /* Read client request, send sequence number back */

        /*if (readLine(cfd, reqLenStr, INT_LEN) <= 0) {
            close(cfd);
            continue;                   /* Failed read; skip request */
        //}

        d_line = buffered_stream_reader_read_line(reader);

        reqLen = atoi(d_line);
        if (reqLen <= 0) {              /* Watch for misbehaving clients */
            close(cfd);
            continue;                   /* Bad request; skip it */
        }
        free(d_line);

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
        if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
            fprintf(stderr, "Error on write");

        seqNum += reqLen;               /* Update sequence number */

        if (close(cfd) == -1)           /* Close connection */
            errMsg("close");
    }
}
