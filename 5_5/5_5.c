/*
 * 5_5.c
 *
 *  Created on: May 11, 2021
 *      Author: cory
 */


#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char **argv)
{
	int fd0, fd1;
	int flags0, flags1;
	int offset0, offset1;

	if(argc != 2)
		usageErr("%s <file>",argv[0]);

	fd0 = open(argv[1],
			O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	if(fd0 == -1)
		errExit("Cannot open file: '%s'\n", argv[1]);

	fd1 = dup(fd0);

	if(fd1 == -1)
		errExit("Cannot Dup: %d{%s}\n", fd0, argv[1]);

	flags0 = fcntl(fd0, F_GETFL);
	flags1 = fcntl(fd1, F_GETFL);

	if(flags0 == -1)
		errExit("Cannot Get Flags 0\n");

	if(flags1 == -1)
		errExit("Cannot Get Flags 1\n");

	if(flags0 == flags1)
		printf("FLAGS:%d==%d\n", flags0, flags1);

	write(fd0, "x", 1);

	offset0 = lseek(fd0, 0, SEEK_CUR);
	offset1 = lseek(fd1, 0, SEEK_CUR);

	if(offset0 == -1)
		errExit("offset 0\n");

	if(offset1 == -1)
		errExit("offset 1\n");

	if(offset0 == offset1)
		printf("OFFSET:%d==%d", offset0, offset1);

}
