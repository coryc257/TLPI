/*
 * main.c
 *
 *  Created on: Jun 23, 2021
 *      Author: cory
 */


#ifndef LIB_1
#define LIB_1 "blah"
#endif
#ifndef LIB_2
#define LIB_2 "blah"
#endif

#include <stdio.h>
#include <dlfcn.h>
int
main(int argc, char *argv[])
{
	void *x_LIB_1, *x_LIB_2;
	char* (*x_get_it)(void);
	void (*x_set_it)(char *);
	char *result;

	x_LIB_2 = dlopen(LIB_2, RTLD_NOW | RTLD_GLOBAL);
	if (x_LIB_2 == NULL)
		printf("ERROR IN LIB 2:%s\n", dlerror());
	// PROGRAM CRASHES IF THE FOLLOWING THREE LINES ARE COMMENTED OUT
	x_LIB_1 = dlopen(LIB_1, RTLD_NOW | RTLD_GLOBAL);
	if (x_LIB_1 == NULL)
		printf("ERROR IN LIB 1:%s\n", dlerror());


	*((void**)&x_get_it) = dlsym(x_LIB_2, "get_it");
	dlclose(x_LIB_2);
	*((void**)&x_set_it) = dlsym(x_LIB_2, "make_it");
	(x_set_it)("mal and zoe are the best cats");
	result = (x_get_it)();
	if (result == NULL)
		printf("Error\n");
	else
		printf("TRUTH:%s\n",result);
}
