/*
 * main.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include "lib/6_functions.h"
extern char **environ;
int
main(int argc, char **argv)
{
	char **forward;
	if(argc < 2)
		usageErr("%s <function>\n", argv[0]);

	forward = malloc(sizeof(char **)*(argc-1));

	forward[0] = argv[0];
	for(int i = 0; i < (argc-2); i++)
	{
		forward[i+1]=argv[i+2];
	}

	if(strcmp(argv[1],"mem_segments") == 0)
		return __mem_segments__main(argc-1, forward);
	else if(strcmp(argv[1], "necho") == 0)
		return __necho__main(argc-1, forward);
	else if(strcmp(argv[1], "display_env") == 0)
		return __display_env__main(argc-1, forward);
	else if(strcmp(argv[1], "modify_env") == 0)
		return __modify_env__main(argc-1, forward);
	else if(strcmp(argv[1], "longjmp") == 0)
		return __longjmp__main(argc-1, forward);
	else if(strcmp(argv[1], "setjmp_vars") == 0)
		return __setjmp_vars__main(argc-1, forward);
	else if(strcmp(argv[1], "i_tripped") == 0)
		return __i_tripped__main(argc-1, forward);
	else if(strcmp(argv[1], "6-3") == 0)
	{
		clearenv();

		char **x;
		x = malloc(sizeof(char**)*4);
		x[0] = malloc(10);
		x[1] = malloc(10);
		x[2] = malloc(10);
		x[3] = NULL;

		strcpy(x[0], "PATH=1111");
		strcpy(x[1], "PATH=2222");
		strcpy(x[2], "XXXXXXX=2");
		environ = x;
		my_unsetenv("PATH");
		__display_env__main(argc-1, forward);

		clearenv();
		my_setenv("PATH","X",0);
		__display_env__main(argc-1, forward);
		my_setenv("PATH","Y",0);
		__display_env__main(argc-1, forward);
		my_setenv("PATH","Y",1);
		__display_env__main(argc-1, forward);
		exit(EXIT_SUCCESS);
	}

	errExitEN(-5324, "Error Command: <%s> not found\n", argv[1]);
}
