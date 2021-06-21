/*
 * main.c
 *
 *  Created on: Jun 21, 2021
 *      Author: cory
 */

#include <unistd.h>
#include <utmp.h>

int
main (int argc, char *argv[])
{
	/*char *user;
	user = getlogin();
	if (user != NULL)
		printf("Username:%s\n", user);*/
	//utmpx_login(argc, argv);


	struct utmp x;
	strcpy(x.ut_user, "xory");
	my_login(&x);

}
