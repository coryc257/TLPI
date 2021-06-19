/*
 * main.c
 *
 *  Created on: Jun 19, 2021
 *      Author: cory
 */

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE
#include <sys/capability.h>
#include <unistd.h>
#include <limits.h>
#include <sched.h>
#include "tlpi_hdr.h"

static void 
__validate_input(int argc, char *argv[]) 
{
	if (argc < 3 || strchr("rfo", argv[1][0]) == NULL)
		usageErr(
				"%s policy priority [pid ...]\n    policy is 'r' (RR), 'f' (FIFO), of 'o' (OTHER)\n",
				argv[0]);
}

static void
__get_sched_type(int *sched_type, char *argv[])
{
	switch (argv[1][0]) {
	case 'r':
		*sched_type = SCHED_RR;
		break;
	case 'f':
		*sched_type = SCHED_FIFO;
		break;
#ifdef SCHED_BATCH
	case 'b':
		*sched_type = SCHED_BATCH;
		break;
#endif
#ifdef SCHED_IDLE
	case 'i':
		*sched_type = SCHED_IDLE;
		break;
#endif
	default :
		*sched_type = SCHED_OTHER;
		break;
	}
}

void __modify_cap(int setting) {
	cap_t caps;
	cap_value_t cap_list[1];
	caps = cap_get_proc();
	cap_list[0] = CAP_SYS_NICE;
	if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, setting) == -1) {
		cap_free(caps);
		errExit("cap_set_flag\n");
	}
	if (cap_set_proc(caps) == -1) {
		cap_free(caps);
		errExit("cap_set_proc\n");
	}
	cap_free(caps);
}

static void
__get_permissions()
{
	__modify_cap(CAP_SET);
}

static void
__rmv_permissions()
{
	__modify_cap(CAP_CLEAR);
}

int
main (int argc, char *argv[])
{
	int j,sched_type;
	struct sched_param sp;

	__validate_input(argc, argv);
	__get_sched_type(&sched_type,argv);
	sp.sched_priority = getInt(argv[2], 0, "priority");


	__get_permissions();
	for (j = 3; j < argc; j++) {
		if (sched_setscheduler(getLong(argv[j], 0, "pid"), sched_type, &sp) == -1)
			errExit("sched_setscheduler");
	}
	__rmv_permissions();

	exit(EXIT_SUCCESS);
}
