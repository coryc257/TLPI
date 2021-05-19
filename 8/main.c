/*
 * main.c
 *
 *  Created on: May 15, 2021
 *      Author: cory
 */


#include "lib/8_hdr.h"
#include "tlpi_hdr.h"
#include <pwd.h>
#define OUTPUT_FORMAT "PWD(cory):{'name':'%s','passwd':'%s','uid':'%ld','gid':'%ld','dir':'%s','shell':'%s','gecos':'%s'}\n"
int
main(int argc, char **argv)
{
	struct passwd *pwd;
	//__check_password__name(argc, argv);
	if((pwd = my_getpwname("cory")) == NULL)
		errExit("my_getpwname");
	else
	{
		printf(OUTPUT_FORMAT, pwd->pw_name, pwd->pw_passwd,
				pwd->pw_uid, pwd->pw_gid,
				pwd->pw_dir, pwd->pw_shell,
				pwd->pw_gecos);
	}
}
