/*
 * main.c
 *
 *  Created on: Jul 6, 2021
 *      Author: cory
 */

#include "this.h"
#include "tlpi_hdr.h"
#include <stdio.h>
THIS this;

int
main(int argc, char *argv[])
{

	char input[4000];
	if (open_message_queues(&this,argv[1]) == -1)
		errExit("open_message_queues\n");
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);
	memset(input,0,4000);
	for (;;) {
		read(STDIN_FILENO,input,4000);
		fflush(stdin);
		send_message(&this,input);
		memset(input,0,4000);
	}
}
