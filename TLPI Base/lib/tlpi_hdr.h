/*
 * tlpi_hdr.h
 *
 *  Created on: May 5, 2021
 *      Author: cory
 */

#ifndef TLPI_HDR_H_
#define TLPI_HDR_H_
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "get_num.h"

#include "error_functions.h"

#include "read_line.h"

#ifndef A_BOOL
typedef enum { FALSE, TRUE } Boolean;
#endif

#define min(m,n) ((m) < (n)) ? (m) : (n)
#define max(m,n) ((m) > (n)) ? (m) : (n)

int char_index(char *str, size_t len, char search);

void append_to_hold(BUFFERED_STREAM_READER stream, char *str, size_t len);


#endif /* TLPI_HDR_H_ */
