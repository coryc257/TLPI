/*
 * main.c
 *
 *  Created on: May 27, 2021
 *      Author: cory
 */
#include <unistd.h>

int
main(int argc, char *argv[])
{
	chdir("/home/cory/tlpi_workspace");
	char *rp = realpath("../z/file.txt", NULL);
	puts(rp);
	puts('\n');
}
