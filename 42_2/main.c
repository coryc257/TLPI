/*
 * main.c
 *
 *  Created on: Jun 24, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include "tlpi_hdr.h"

void
output_stats(void *function)
{
#define _X_X__OUTPUT_STRING "{\n\t\"dli_fname\":\"%s\",\n\t\"dli_fbase\":\"%p\",\n\
\t\"dli_sname\":\"%s\",\n\t\"dli_saddr\":\"%p\"\n}\n"
	Dl_info info;
	dladdr(function,&info);
	printf(_X_X__OUTPUT_STRING,info.dli_fname,info.dli_fbase,info.dli_sname,info.dli_saddr);
}

int
main(int argc, char *argv[])
{
	void *libHandle;
	void (*funcp)(void);
	const char *err;

	printf("HELLO?\n");

	if (argc < 3 || strcmp(argv[1],"--help") == 0)\
		usageErr("%s lib-path func-name\n", argv[0]);

	/* Load the shared library and get a handle for later use */

	libHandle = dlopen(argv[1], RTLD_LAZY);
	if (libHandle == NULL)
		fatal("dlopen: %s", dlerror());

	/* Search library for symbol name in argv[2] */
	(void)dlerror(); // Clear dlerror();
	*(void**)&funcp = dlsym(libHandle, argv[2]);
	err = dlerror();
	if (err != NULL)
		fatal("dlsym(%s::%s):%s\n", argv[1], argv[2], err);

	/* Try calling the address returned by dlsym() as a function
	 * that takes no arguments
	 */

	(*funcp)();

	output_stats(funcp);

	dlclose(libHandle);

	exit(EXIT_SUCCESS);


}
