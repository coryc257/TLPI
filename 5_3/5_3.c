/*
 * 5_3.c
 *
 *  Created on: May 10, 2021
 *      Author: cory
 */


#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include <unistd.h>

typedef struct {
	char *file_name;
	char *num_bytes_str;
	int USE_APPEND;
	long num_bytes;
	int fd;
} options;

void get_flags(options *o, int argc, char *argv[]);
void open_file(options *o);
void write_file(options *o);
void close_file(options *o);

int
main(int argc, char *argv[])
{
	options o;
	o.USE_APPEND = 0;
	get_flags(&o, argc, argv);
	open_file(&o);
	write_file(&o);
	close_file(&o);
}

void
get_flags(options *o, int argc, char *argv[])
{
	if(argc < 3)
			usageErr("%s <filename> <num-bytes> [x]", argv[0]);

	if(argc == 4 && argv[3][0] == 'x')
		o->USE_APPEND = O_APPEND;

	o->file_name = argv[1];
	o->num_bytes_str = argv[2];

	o->num_bytes = getLong(o->num_bytes_str, GN_NONNEG, "5_3(main)");
}

void
open_file(options *o)
{
	int fd;

	fd = open(o->file_name, O_RDWR | O_CREAT | o->USE_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if(fd == -1)
		errExit("Cannot open file\n");
	o->fd = fd;
}

void
write_file(options *o)
{
	for(int i = 0; i < o->num_bytes; i++)
	{
		lseek(o->fd,0,SEEK_END);
		write(o->fd,"x",1);
	}
}

void
close_file(options *o)
{
	close(o->fd);
}
