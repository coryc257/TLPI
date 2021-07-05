/*
 * x49_4.c
 *
 *  Created on: Jul 5, 2021
 *      Author: cory
 */

#include <linux/mman.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>

 //#define MAP_UNINITIALIZED 0x4000000
void
x49_4(int argc, char *argv[])
{
	void *root, *x, *y, *z;
	int ps = sysconf(_SC_PAGESIZE);
	root = mmap(NULL,4096*3,PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	memset(root,'x',4096*1);

	x = mmap(root+(ps*2), 4096, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED | MAP_UNINITIALIZED, -1, 0);
	y = mmap(root+ps, 4096, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED | MAP_UNINITIALIZED, -1, 0);
	z = mmap(root, 4096, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED | MAP_UNINITIALIZED, -1, 0);

	memset(x,'x',4096);
	memset(y,'y',4096);
	memset(z,'z',4096);

	// outputs z...y...x...\n
	for(int j = 0; j < 4096*3; j++) {
		putchar(((char*)root)[j]);
	}
	printf("\n---------------------\n");
}
