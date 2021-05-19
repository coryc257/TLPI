/*
 * main.c
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "lib/mem.h"

int
main(int argc, char **argv)
{
	char *x, *y, *z, *a;
	x = (char*)my_malloc(sizeof(char)*92);
	strcpy(x,"1234");
	for(int i = 4; i < 90; i++)
	{
		x[i]='0';
	}
	x[90]='9';
	x[91]='\0';
	y = (char*)my_malloc(sizeof(char)*25);
	strcpy(y,"0987654");
	my_malloc(0);

	printf("%s vs %s\n", x, y);
	my_free(x);

	x = (char*)my_malloc(sizeof(char)*4);
	strcpy(x,"poo");
	a = y;
	y = (char*)my_malloc(sizeof(char)*4);
	strcpy(y,"loo");
	z = (char*)my_malloc(sizeof(char*)*2);
	strcpy(z,"I");
	printf("%s %s on the %s... Absolutely %s!\n",z,x,y,a);

	my_free(x);
	my_free(y);
	my_free(z);
	my_free(a);
}
