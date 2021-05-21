/*
 * main.c
 *
 *  Created on: May 17, 2021
 *      Author: cory
 */

#include "tlpi_hdr.h"
#include "lib/12.h"

int
main(int argc, char *argv[])
{

	if(argc < 2)
		usageErr("%s <program>", argv[0]);

	for(int j = 0; j < argc-1; j++)
	{
		argv[j]=argv[j+1];
	}
	argv[argc-1] = NULL;

	if(strcmp(argv[0],"procfs_pidmax") == 0)
	{
		__procfs_pidmax__main(argc-1,argv);
	}
	else if(strcmp(argv[0], "t_uname") == 0)
	{
		__t_uname__main();
	}
	else if(strcmp(argv[0], "12-1") == 0)
		__12_1__main(argc-1, argv);
	else if(strcmp(argv[0], "12-2") == 0)
		__12_2__main();
	else if(strcmp(argv[0], "12-3") == 0)
		__12_3__main(argc-1, argv);
	/*char **path_items;
	int path_item_count;
	split_path("/var/run/log.txt",&path_items,&path_item_count);*/

}
