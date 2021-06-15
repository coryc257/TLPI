/*
 * rtsched.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include "exec_container.h"
#include <sched.h>
#include <unistd.h>

static void
run(EXEC_CONTAINER *ec, int policy, int priority)
{
	uid_t real_user;
	gid_t real_group;
	struct sched_param sp;
	sp.sched_priority = priority;
	sched_setscheduler(getpid(),policy,&sp);

	real_user = getuid();
	real_group = getgid();

	// ABANDON SHIP!
	setfsuid(real_user);
	setresuid(real_user,real_user,real_user);
	setfsgid(real_group);
	setresgid(real_group,real_group,real_group);


	execv(ec->exe, ec->args);
}

int
rtsched__main(int argc, char *argv[])
{
	EXEC_CONTAINER ec;
	char *arg_policy, *arg_priority;
	int policy, priority;

	if (argc < 4 || (argc == 2 && strcmp(argv[1],"--help") == 0))
		usageErr("%s POLICY PRIORITY COMMAND [[ARG] ...]\n", argv[0]);

	arg_policy = argv[1];
	arg_priority = argv[2];

	switch (arg_policy[0]) {
	case 'r':
		policy = SCHED_RR;
		break;
	case 'f':
		policy = SCHED_FIFO;
		break;
	default:
		errExit("Policy Error, allowed values { 'r':SCHED_RR, 'f':SCHED_FIFI } \n");
	}

	priority = getInt(arg_priority,GN_ANY_BASE,"arg_priority");


	construct_args(argc-3, &(argv[3]), &ec);
	run(&ec,policy,priority);
}
