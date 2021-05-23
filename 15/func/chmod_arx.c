/*
 * chmod_arx.c
 *
 *  Created on: May 23, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include <sys/stat.h>



void
do_it(char *file_name)
{
	struct stat stat_buf;
	stat(file_name,&stat_buf);


	chmod(file_name, stat_buf.st_mode | S_IRUSR | S_IRGRP | S_IROTH |S_IXUSR | S_IXGRP | S_IXOTH);

}
