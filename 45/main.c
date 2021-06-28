/*
 * main.c
 *
 *  Created on: Jun 28, 2021
 *      Author: cory
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>

#ifndef TEST_FILE_451
#define TEST_FILE_451 "/home/cory/tlpi_workspace/TLPI/45_1_file"
#endif

void x45_1() {
	struct stat sb;
	key_t fk;
	fk = ftok(TEST_FILE_451, 2147483647);
	stat(TEST_FILE_451, &sb);
	printf("0x%x\n", fk);
	printf("%x::%x::%x\n", sb.st_ino, sb.st_dev, 2147483647);
}

static key_t
my_ftok(const char *pathname, int proj)
{
	key_t ret;
	struct stat sb;
	int pshift, dshift_out, ishift_out;
	size_t is_m, ds_m, ps_m;
	unsigned long long ishift;
	unsigned long long dshift;
	stat(pathname,&sb);
	pshift = proj;
	ishift = (unsigned long long)sb.st_ino;
	dshift = (unsigned long long)sb.st_dev;
	ps_m = 8*sizeof(int) - 8;
	is_m = 8*sizeof(unsigned long int) - 16;
	ds_m = 8*sizeof(unsigned long int) - 8;
	pshift = ((unsigned long long)pshift << ps_m);
	dshift_out = ((unsigned long long)dshift << ds_m) >> ds_m << 16;
	ishift_out = (int)(((unsigned long long)ishift << is_m) >> is_m);

	ret = pshift | dshift_out | ishift_out;

	printf("%x\n",ret);
	return ret;
}

int
main(int argc, char *argv[])
{
	x45_1();
	my_ftok(TEST_FILE_451,2147483647);
}
