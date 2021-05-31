/*
 * main.c
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#include "lib/signal_functions.h"
#include "lib/mysig.h"

int
main(int argc, char *argv[])
{
	//__ouch__main(argc,argv);
	//__intquit__main(argc,argv);
	//__t_kill__main(argc,argv);
	//printSigMask(stdout, "Test?");
	__sig_sender__main(argc,argv);
	//__sig_receiver__main(argc, argv);
}
