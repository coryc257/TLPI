/*
 * longjmp.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#include <setjmp.h>
#include "tlpi_hdr.h"
#include "../lib/6_functions.h"

static jmp_buf __jump_buf_env; //jump_buf_env

static void
f2(void)
{
	longjmp(__jump_buf_env, 2);
}

static void
f1(int argc)
{
	if (argc == 1)
		longjmp(__jump_buf_env,1);
	f2();
}

int
__longjmp__main(int argc, char **argv)
{
	jump_buf_env:
	switch(setjmp(__jump_buf_env)){
	case 0:	/* This is the return after the initial setjmp() */
		printf("Calling f1() after initial setjmp()\n");
		f1(argc);		/* Never returns */
		break;			/* But this is good form */

	case 1:
		printf("Jump from f1()\n");
		break;

	case 2:
		printf("Jump from f2()\n");
	}

	exit(EXIT_SUCCESS);
}
