/*
 * 42_lib_1.c
 *
 *  Created on: Jun 23, 2021
 *      Author: cory
 */

#include "42_lib_2.h"
#include <stdio.h>
void
interfact_make_it(const char *value)
{
	make_it(value);
}

void
interface_show_it()
{
	char *x;
	x = get_it();
	printf("GOT:%s\n",x);
}
