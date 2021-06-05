/*
 * main.c
 *
 *  Created on: Jun 5, 2021
 *      Author: cory
 */

#include <stdarg.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	my_execlp("/bin/ls", "-lZ", "/home/cory", "/home/cory/Pictures", (char*)NULL);//, "/home/cory/Pictures");
	//my_execlp("ls");

}
