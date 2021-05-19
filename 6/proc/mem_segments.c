/*
 * mem_segments.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */


#include <stdio.h>
#include <stdlib.h>
#include "../lib/6_functions.h"

char globoHomo[65536];			/* Uninitialized data segment */
int primes[] = { 2, 3, 5, 7 };	/* Initialized data segment */

static int
square(int x)					/* Allocated in frame square() */
{
	int result;					/* Allocated in frame square() */

	result = x * x;
	return result;				/* Return value passed by register */
}

static void
doCalc(int val)					/* Allocated in frame for doCalc() */
{
	printf("The square of %d is %d\n", val, square(val));

	if (val < 1000){
		int t;					/* Allocated in frame for doCalc() */

		t = val*val*val;
		printf("The cube of %d is %d\n", val, t);
	}
}

int
__mem_segments__main(int argc, char **argv)	/* allocated in frame for ...main() */
{
	static int key = 9973;					/* Initialized data segment */
	static char mbuf[10240000];				/* Uninitialized data segment */
	char *p;								/* Allocated in frame for ...main() */

	p = malloc(1024);						/* Points to memory in heap segment */

	doCalc(key);

	exit(EXIT_SUCCESS);
}
