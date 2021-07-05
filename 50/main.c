/*
 * main.c
 *
 *  Created on: Jul 5, 2021
 *      Author: cory
 */

#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#define PAGE_SIZE get_page_size()
#define STR_VALUE "Hello There\n"

static int
get_page_size()
{
	static int *ps = NULL;
	if (ps == NULL) {
		ps = malloc(sizeof(int));
		*ps = sysconf(_SC_PAGESIZE);
	}
	return *ps;
}

static void
x50_1()
{
	void *x;
	struct rlimit rl;
	rl.rlim_cur = PAGE_SIZE;
	rl.rlim_max = PAGE_SIZE;
	setrlimit(RLIMIT_MEMLOCK, &rl);
	x = mmap(NULL,PAGE_SIZE*2,PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (mlock(x, PAGE_SIZE) != 0)
		errExit("mlock(1)\n");
	if (mlock(x+PAGE_SIZE, PAGE_SIZE) != 0)
		errExit("mlock(2)\n");
}

static void
x50_2()
{
	void *x;
	x = mmap(NULL,PAGE_SIZE,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	memcpy(x, STR_VALUE, strlen(STR_VALUE)+1);

	printf("%s", (char*)x);
	madvise(x,PAGE_SIZE,MADV_DONTNEED);
	printf("%s", (char*)x);
}


int
main(int argc, char *argv[])
{
	//x50_1();
	x50_2();
}
