/*
 * cli.c
 *
 *  Created on: May 26, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include "8_hdr.h"
#include <getopt.h>
#include <pwd.h>
#include <acl/libacl.h>
#include <sys/acl.h>
#include "../../lib/18_1.h"
extern char *optarg;
extern int opterr, optind, optopt;

#define NEXTOPT (opt = getopt(argc,argv,"u:g:f:")) != -1

void die_usage(char *program) {
	usageErr("%s -{u,g}<{user,group}> -f<file>\n", program);
}

static void
output_acl(RET_ACL *acl)
{
	printf("Real:%c%c%c\n", acl->permission & ACL_READ ? 'r' : '-', acl->permission & ACL_WRITE ? 'w' : '-', acl->permission & ACL_EXECUTE ? 'x' : '-');
	printf("Mask:%c%c%c\n", acl->mask & ACL_READ ? 'r' : '-', acl->mask & ACL_WRITE ? 'w' : '-', acl->mask & ACL_EXECUTE ? 'x' : '-');
}

void
__cli_main(int argc, char *argv[])
{
	int opt;
	char *file = NULL;
	int type = 0;
	uid_t user = 0; //Type 1
	gid_t group = 0;// Type 2
	RET_ACL *acl;
	opterr = 0;
	if (argc < 3)
		die_usage(argv[0]);

	while (NEXTOPT)
	{
		switch(opt) {
		case 'u':
			if (optarg == NULL)
				errExit("No Username Specified\n");
			user = userIdFromName(optarg);
			type = 1;
			break;
		case 'g':
			if (optarg == NULL)
				errExit("No Group Specified\n");
			group = groupIdFromName(optarg);
			type = 2;
			break;
		case 'f':
			if (optarg == NULL)
				errExit("No File Specified\n");
			file = optarg;
			break;
		}
	}

	if (type == 0 || file == NULL)
		die_usage(argv[0]);

	if((acl = get_user_acl(type == 1 ? user : group, file)) == NULL)
	{
		fatal("Cannot Get ACL\n");
	}
	else if (acl->OK != 0)
	{
		fprintf(stderr,"ACL Error:%d\n", acl->OK);
	}
	output_acl(acl);
}
