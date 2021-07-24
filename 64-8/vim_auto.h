/*
 * vim_auto.h
 *
 *  Created on: Jul 23, 2021
 *      Author: cory
 */

#ifndef VIM_AUTO_H_
#define VIM_AUTO_H_

#include "read_line.h"

typedef struct __vim_command {
	unsigned char *bytes;
} VIM_COMMAND;

VIM_COMMAND * fetch_command(BUFFERED_STREAM_READER streamn);

#endif /* VIM_AUTO_H_ */
