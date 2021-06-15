/*
 * nice.c
 *
 *  Created on: Jun 14, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "exec_container.h"
extern char *optarg;
extern int optind, opterr, optopt;


static void
run(EXEC_CONTAINER *ec, int nice_value)
{
	setpriority(PRIO_PROCESS,0, nice_value);
	execv(ec->exe,ec->args);
}

int
nice__main(int argc, char *argv[])
{
	opterr = 0;
	int option_index = 0;
	int nice_value = 0;
	int pass_args = 0;
	EXEC_CONTAINER ec;
	Boolean nice_got = FALSE;
	struct option long_opts[] = {
			{"adjustment", 1, NULL, 'n'},
			{0,0,0,0}
	};

	if ((argc < 2) || (argc >= 2 && strcmp(argv[1],"--help") == 0))
		usageErr("%s [OPTION] [COMMAND [ARG]...]\n");

	while (1) {
		int option = getopt_long(argc,argv,"n:",long_opts,&option_index);
		switch (option) {
		case 'n':
			if (optarg == NULL)
				errExit("No Nice Value supplied\n");
			nice_value = getInt(optarg,GN_ANY_BASE,"-n");
			nice_got = TRUE;
			break;
		}
		if (option == -1 || nice_got == TRUE) break;
	}

	if (nice_got == FALSE)
		errExit("No Nice Value supplied\n");

	pass_args = argc-optind;

	if (pass_args == 0) {
		errno = EINVAL;
		errExit("No Argument\n");
	}

	construct_args(pass_args, &(argv[optind]), &ec);
	run(&ec,nice_value); // This program dies as this point
	return -1;
}
