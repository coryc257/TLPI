/*
 * i_tripped.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */


#include <setjmp.h>
#include "tlpi_hdr.h"
#include "../lib/6_functions.h"
#include <stdio.h>


static jmp_buf jump_area;

static
void go_to()
{
	switch(setjmp(jump_area)){
	case 0:
		printf("Start 0\n");
		longjmp(jump_area,1);
		break;
	case 1:
		printf("Jump 1\n");
		break;
	case 2:
		printf("Jump 2\n");
		break;
	default:
		printf("badJump!\n");
		break;
	}
}


int
__i_tripped__main(int argc, char **argv)
{
	go_to();
	longjmp(jump_area,1);
	exit(EXIT_SUCCESS);
}
