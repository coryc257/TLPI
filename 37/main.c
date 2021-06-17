/*
 * main.c
 *
 *  Created on: Jun 16, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <getopt.h>
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include "tlpi_hdr.h"

extern int optopt, optind, opterr;
extern char *optarg;

#define DEFAULT_OPTIONS 0 | LOG_PERROR
#define NUM_OPTIONS 5
static struct option __ops[] = {
			{"identity",1,NULL,'i'},
			{"priority",1,NULL,'p'},
			{"facility",1,NULL,'f'},
			{"message",1,NULL,'m'},
			{0,0,0,0}
	};
static char *__sops = "i:p:f:m:";

struct __long_ops_ops {

	int ops_index;
	char *sops;
	struct option ops[NUM_OPTIONS];
};

typedef struct X37_STATE {
	char *identity;
	int priority;
	int facility;
	char *message;
	int options;
	struct __long_ops_ops ops;

} X37_STATE;

static void
init_default_state(X37_STATE *state)
{

	for (int j = 0; j < NUM_OPTIONS; j++)
	{
		state->ops.ops[j] = __ops[j];
	}

	state->ops.sops = __sops;
	state->options = DEFAULT_OPTIONS;
	state->identity = "DEFAULT";
	state->message = "<eom>";
	state->facility = LOG_USER;
	state->priority = LOG_INFO;
}

int
main(int argc, char *argv[])
{
	X37_STATE state;
	int option;
	int intval;
	init_default_state(&state);


	while((option = getopt_long(argc, argv, state.ops.sops , state.ops.ops, &(state.ops.ops_index))) != -1) {
		switch (option) {
		case 'i':
			if (optarg != NULL)
				state.identity = optarg;
			else
				errExit("Identity value not supplied\n");
			break;
		case 'p':
			if (optarg == NULL)
				errExit("No Priority Supplied\n");
			state.priority = getInt(optarg, GN_NONNEG, "priority");
			break;
		case 'f':
			if (optarg == NULL)
				errExit("No Facility Supplied\n");
			state.facility = getInt(optarg, GN_NONNEG, "facility");
			break;
		case 'm':
			if (optarg == NULL)
				errExit("No Message Supplied\n");
			state.message = optarg;
		}

	}

	openlog(state.identity,state.options, state.facility);
	syslog(state.facility|state.priority,"%s",state.message);
	closelog();
}
