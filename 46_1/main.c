/*
 * main.c
 *
 *  Created on: Jun 29, 2021
 *      Author: cory
 */

#include <stdio.h>


int
main (int argc, char *argv[])
{

#ifdef X_svmsg_create
	svmsg_create(argc,argv);
#endif
#ifdef X_svmsg_send
	svmsg_send(argc,argv);
#endif
#ifdef X_svmsg_receive
	svmsg_receive(argc,argv);
#endif
}
