/*
 * main.c
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */


#include "tlpi_hdr.h"
#include "lib/11.h"

int
main(int argc, char *argv[])
{
	RETURN status;
	status = get_method("t_sysconf")(NULL);
	status = get_method("t_tpathconf")(NULL);
}
