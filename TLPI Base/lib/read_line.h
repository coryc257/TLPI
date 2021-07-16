/*
 * read_line.h
 *
 *  Created on: Jul 15, 2021
 *      Author: cory
 */

#ifndef READ_LINE_H_
#define READ_LINE_H_

typedef void* BUFFERED_STREAM_READER;

ssize_t readLine(int fd, void *buffer, size_t n);


BUFFERED_STREAM_READER buffered_stream_reader_new(int fd, size_t block_size);
char * buffered_stream_reader_read_line(BUFFERED_STREAM_READER stream);
char * get_line(BUFFERED_STREAM_READER stream, size_t line_stop);
char * yank_line(BUFFERED_STREAM_READER stream);
void increment_line_count(BUFFERED_STREAM_READER stream);


#endif /* READ_LINE_H_ */
