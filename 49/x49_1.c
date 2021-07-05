/*
 * x49_1.c
 *
 *  Created on: Jul 5, 2021
 *      Author: cory
 */


#include "tlpi_hdr.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

// Lower 12 bits
#define PMASK 0xFFF

typedef struct exec_info {
	char *source_file;
	char *target_file;

	int source_fd;
	int target_fd;

	struct stat source_stat;

	void *source_byte_stream;
	void *target_byte_stream;
} EXEC_INFO;

EXEC_INFO this;

void
my_cp(int argc, char *argv[])
{
	if (argc != 3)
		usageErr("%s <source> <target>", argv[0]);

	this.source_file = argv[1];
	this.target_file = argv[2];

	this.source_fd = open(this.source_file, O_RDONLY, NULL);
	flock(this.source_fd, LOCK_EX);
	fstat(this.source_fd, &this.source_stat);

	remove(this.target_file);
	this.target_fd = open(this.target_file, O_CREAT | O_TRUNC | O_EXCL | O_RDWR, this.source_stat.st_mode & PMASK);
	flock(this.target_fd, LOCK_EX);
	ftruncate(this.target_fd, this.source_stat.st_size);

	this.source_byte_stream = mmap(NULL,this.source_stat.st_size, PROT_READ, MAP_PRIVATE, this.source_fd, 0);
	this.target_byte_stream = mmap(NULL,this.source_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, this.target_fd, 0);

	memcpy(this.target_byte_stream,this.source_byte_stream,this.source_stat.st_size);

	flock(this.source_fd,LOCK_UN);
	flock(this.target_fd,LOCK_UN);
	close(this.source_fd);
	close(this.target_fd);

	exit(EXIT_SUCCESS);
}
