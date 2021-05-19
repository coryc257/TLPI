/*
 * 5_1.c
 *
 *  Created on: May 10, 2021
 *      Author: cory
 */


#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
main(void)
{
	int fd;
	fd = open("file.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
	if(fd != -1)
	{
		lseek(fd,0,SEEK_SET);
		write(fd,"where?", 6);
		close(fd);
	}
	else
		errExit("unable to open file\n");

	return 0;
}
