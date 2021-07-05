/*
 * main.c
 *
 *  Created on: Jul 4, 2021
 *      Author: cory
 */

#include "lib/directory_service.h"
#include "tlpi_hdr.h"
#include <sys/wait.h>

#define SERVER_ROOT "/home/cory/directory_server"

int
main(int argc, char *argv[])
{
	DIRECTORY_ITEM *item;
	pid_t child;

	switch(child = fork()) {
	case -1:
		errExit("fork\n");
		break;
	case 0:
		init_directory_server(SERVER_ROOT);
		_exit(EXIT_SUCCESS);
		break;
	default:
		waitpid(child,NULL,0);
		break;
	}


	fix_server(SERVER_ROOT);

	update_entry(SERVER_ROOT, "mal", "good cat");
	update_entry(SERVER_ROOT, "zoe", "good cat");
	add_entry(SERVER_ROOT,"mal", "good cat");
	add_entry(SERVER_ROOT,"zoe", "good cat");

	item = get_entry(SERVER_ROOT, "mal");
	printf("%s | %s\n", item->key, item->value);
	free(item);

	item = get_entry(SERVER_ROOT, "zoe");
	printf("%s | %s\n", item->key, item->value);
	free(item);

	update_entry(SERVER_ROOT, "mal", "best cat");
	update_entry(SERVER_ROOT, "zoe", "best cat");

	item = get_entry(SERVER_ROOT, "mal");
	printf("%s | %s\n", item->key, item->value);
	free(item);

	item = get_entry(SERVER_ROOT, "zoe");
	printf("%s | %s\n", item->key, item->value);
	free(item);
}
