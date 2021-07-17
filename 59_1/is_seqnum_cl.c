/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 59-7 */

#include <netdb.h>
#include "is_seqnum.h"
#include "tlpi_hdr.h"
#include "read_line.h"
#include "inet_sockets.h"

int
is_seqnum_cl(int argc, char *argv[])
{
	BUFFERED_STREAM_READER reader;
    char *reqLenStr;                    /* Requested length of sequence */
    int cfd;
    char *line;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s hostname port [sequence-len]\n", argv[0]);


    // 59-2 Exercise, get rid of boilerplate
    cfd = inetConnect(argv[1], argv[2], SOCK_STREAM);

    if (cfd == -1)
    	errExit("Cannot Connect To Server\n");

    /* Send requested sequence length, with terminating newline */

    reqLenStr = (argc > 3) ? argv[3] : "1";
    if (write(cfd, reqLenStr, strlen(reqLenStr)) !=  strlen(reqLenStr))
        fatal("Partial/failed write (reqLenStr)");
    if (write(cfd, "\n", 1) != 1)
        fatal("Partial/failed write (newline)");

    /* Read and display sequence number returned by server */
    reader = buffered_stream_reader_new(cfd, INT_LEN);

    line = buffered_stream_reader_read_line(reader);

    printf("Sequence number: %s", line);   /* Includes '\n' */

    exit(EXIT_SUCCESS);                         /* Closes 'cfd' */
}
