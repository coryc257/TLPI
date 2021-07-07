/*
 * main.c
 *
 *  Created on: Jul 6, 2021
 *      Author: cory
 */

#include "this.h"
#include "tlpi_hdr.h"
THIS this;

int
main(int argc, char *argv[])
{

	if (open_message_queues(&this,"root") == -1)
		errExit("open_message_queues\n");

}
