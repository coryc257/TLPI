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
	strcpy(x.ut_name, "grug");
	strcpy(x.ut_user, "grug");
	my_login(&x);
	if(my_logout(&x) == 1)
		printf("SUCCESS\n");
	my_logwtmp("pts/43", "grugalug", "theinternets");


	//dump_utmpx(argc,argv);
}
