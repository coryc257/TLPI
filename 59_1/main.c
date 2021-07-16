/*
 * main.c
 *
 *  Created on: Jul 15, 2021
 *      Author: cory
 */


#include "tlpi_hdr.h"
#include "read_line.h"
#include <sys/stat.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	/*BUFFERED_STREAM_READER stream;
	char *line;
	int fd = open("/home/cory/testfile.txt", O_CREAT | O_RDWR | O_SYNC, S_IRUSR | S_IWUSR);
	stream = buffered_stream_reader_new(fd, 4096);

	for(;;) {
		line = buffered_stream_reader_read_line(stream);
		if (line != NULL)
			printf("%s",line);
		else
			break;
	}*/

	//is_seqnum_sv(argc,argv);
	is_seqnum_cl(argc, argv);

	/*append_to_hold(stream,"Test\n", 5);
	increment_line_count(stream);
	append_to_hold(stream,"BLAHHH\n", strlen("BLAHHH\n"));
	increment_line_count(stream);
	//printf("%s", get_line(stream, 20));
	printf("%s", yank_line(stream));
	printf("%s", yank_line(stream));*/

	//char *x = "hello\nworld\nblah\n";
	//printf("%d\n", char_index(x, strlen(x)-1, '\n'));
}
