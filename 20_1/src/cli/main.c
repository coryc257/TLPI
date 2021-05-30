/*
 * main.c
 *
 *  Created on: May 30, 2021
 *      Author: cory
 */
#include "tlpi_hdr.h"
#include "../../lib/logging.h"
#include "../../lib/file.h"

int
bang_handler(void)
{
	if(errno == ENOMEM)
		errExit("bang_handler::malloc\n");

	return 0;
}
int
main(int argc, char *argv[])
{
	char *root_folder, *log_file;
	if (argc != 3)
		usageErr("%s <root folder> <log file>\n", argv[0]);

	root_folder = argv[1];
	log_file = argv[2];

	if (init_log(log_file) != 0)
		errExit("Error starting logging\n");

	if (build_file_list(root_folder) != 0)
		errExit("Error building directory structure\n");

	if (init_watch(bang_handler) == 0)
		watch_main();
	else
		errExit("init_watch\n");

}
