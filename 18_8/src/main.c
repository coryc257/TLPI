/*
 * main.c
 *
 *  Created on: May 29, 2021
 *      Author: cory
 */

#include "../lib/ftw.h"
//#define _XOPEN_SOURCE 500
//#include <ftw.h>
//#define __USE_XOPEN_EXTENDED
#include <sys/stat.h>
#include <fcntl.h>


static int
handler(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	printf("Received:%s\n",fpath);
	return 0;
}

int
main(int argc, char *argv[])
{
	//nftw("/home/cory/spot",handler,0, FTW_DEPTH);
	my_nftw("/home/cory/spot",handler,10,FTW_DEPTH);
	//nftw("/home/cory/spot",handler,10,FTW_DEPTH);
}
