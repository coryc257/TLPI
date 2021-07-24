/*
 * fetch_command.c
 *
 *  Created on: Jul 23, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <unistd.h>
#include "vim_auto.h"
#include "read_line.h"
#include <stdlib.h>

VIM_COMMAND * fetch_command(BUFFERED_STREAM_READER stream)
{
	VIM_COMMAND *cmd;
	char *ret;
	cmd = malloc(sizeof(VIM_COMMAND));
	ret = buffered_stream_reader_read_line(stream);
	if (ret == NULL) {
		free(cmd);
		return NULL;
	}
	cmd->bytes = (unsigned char *)ret;
	return cmd;
}
