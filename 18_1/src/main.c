/*
 * main.c
 *
 *  Created on: May 26, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"

#ifdef X18_1__ENTRY_POINT

int
main(int argc, char *argv[])
{
#ifdef X18_1__CLI
	__cli_main(argc, argv);
#else
	__gui_main(argc, argv);
#endif
}
#endif
