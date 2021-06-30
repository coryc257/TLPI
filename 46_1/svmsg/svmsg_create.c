/*
 * svmsg_create.c
 *
 *  Created on: Jun 29, 2021
 *      Author: cory
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

static void
usageError(const char *progName, const char *msg)
{
	if (msg != NULL)
		fprintf(stderr, "%s", msg);
	fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} "
			"[octal-perms]\n", progName);
	fprintf(stderr, "     -c          Use IPC_CREAT flag\n");
	fprintf(stderr, "     -x          Use IPC_EXCL flag\n");
	fprintf(stderr, "     -f pathname Generate key using ftok()\n");
	fprintf(stderr, "     -k key      Use 'key as key\n");
	fprintf(stderr, "     -p          Use IPC_PRIVATE key\n");
	exit(EXIT_FAILURE);
}

int
svmsg_create(int argc, char *argv[])
{
	int numKeyFlags;
	int flags, msqid, opt;
	unsigned int perms;
	long lkey;
	key_t key;

	numKeyFlags = 0;
	flags = 0;

	while ((opt = getopt(argc, argv, "cf:k:px"))!= -1) {
		switch(opt) {
		case 'c':
			flags |= IPC_CREAT;
			break;
		case 'f':
			key = ftok(optarg,1);
			if (key == -1)
				errExit("ftok\n");
			numKeyFlags++;
			break;
		case 'k':
			if (sscanf(optarg, "%li", &lkey) != 1)
				cmdLineErr("-k option requires a numeric argument \n");
			key = lkey;
			numKeyFlags++;
			break;
		case 'p':
			key = IPC_PRIVATE;
			numKeyFlags++;
			break;
		case 'x':
			flags |= IPC_EXCL;
			break;
		default:
			usageError(argv[0], "Bad Option\n");
		}
	}

	perms = (optind == argc) ? (S_IRUSR | S_IWUSR) :
			getInt(argv[optind], GN_BASE_8, "octal-perms");

	msqid = msgget(key, flags | perms);
	if (msqid == -1)
		errExit("msget\n");

	printf("%d\n", msqid);
	exit(EXIT_SUCCESS);
}
