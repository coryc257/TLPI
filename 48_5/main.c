/*
 * main.c
 *
 *  Created on: Jul 4, 2021
 *      Author: cory
 */

#include "lib/directory_service.h"
#include "tlpi_hdr.h"

#define SERVER_ROOT "/home/cory/directory_server"

int
main(int argc, char *argv[])
{
	//init_directory_server(SERVER_ROOT);
	add_entry(SERVER_ROOT,"mal", "good cat");
	add_entry(SERVER_ROOT,"zoe", "good cat");
}
