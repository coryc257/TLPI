/*
 * t_readv.c
 *
 *  Created on: May 10, 2021
 *      Author: cory
 */

#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

struct ting {
	char data[10];
	int thing;
};

void write_thing();

int
main(int argc, char *argv[])
{
	int fd;
	struct iovec iov[3];	/* First Buffer */
	struct stat myStruct;
	int x;					/* Second buffer */
#define STR_SIZE 100
	char str[STR_SIZE]; 	/* Third Buffer */
	ssize_t numRead, totRequired;

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		errExit("open");

	totRequired = 0;

	iov[0].iov_base = &myStruct;
	iov[0].iov_len = sizeof(struct stat);
	totRequired += iov[0].iov_len;


	iov[1].iov_base = &x;
	iov[1].iov_len = sizeof(x);
	totRequired += iov[1].iov_len;

	iov[2].iov_base = str;
	iov[2].iov_len = STR_SIZE;
	totRequired += iov[2].iov_len;

	numRead = readv(fd, iov, 3);

	if(numRead == -1)
		errExit("readv");

	if(numRead < totRequired)
	{
		printf("Read fewer bytes than requested\n");
	}

	printf("total bytes requested:%ld; bytes read: %ld\n", (long)totRequired, (long)numRead);

	// My Testing
	write_thing();
	read_thing();
	exit(EXIT_SUCCESS);
}

void
write_thing()
{
	int fd;
	struct iovec output[1];
	struct ting myTing;
	char buf[] = "XiXiXiOoO";

	for(int i = 0; i < 10; i++)
	{
		myTing.data[i] = buf[i];
	}
	myTing.thing = 458;

	output[0].iov_base = &myTing;
	output[0].iov_len = sizeof(struct ting);

	fd = open("struct_file.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);
	if(fd == -1)
		errExit("open");

	writev(fd, output, 1);

	close(fd);
}

void
read_thing()
{
	struct iovec input[1];
	struct ting myTingAgain;
	int fd;
	int numRead;

	input[0].iov_base = &myTingAgain;
	input[0].iov_len = sizeof(struct ting);

	fd = open("struct_file.txt", O_RDONLY);
	if(fd == -1)
		errExit("open");

	numRead = readv(fd, input, 1);

	printf("Received->[%lu]:{\"data\":\"%s\",thing:\"%d\"}", (long)numRead, myTingAgain.data, myTingAgain.thing);
}
