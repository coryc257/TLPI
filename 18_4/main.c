/*
 * main.c
 *
 *  Created on: May 29, 2021
 *      Author: cory
 */


#include <dirent.h>
#include "tlpi_hdr.h"

static void
listFiles(const char *dirpath)
{
	DIR *dirp;
	struct dirent *dp, *result;

	Boolean isCurrent;

	isCurrent = strcmp(dirpath, ".") == 0;

	dirp = opendir(dirpath);
	if (dirp == NULL) {
		errMsg("opendir failed on '%s'\n", dirpath);
		return;
	}

	dp = malloc(sizeof(struct dirent));
	if (dp == NULL)
		errExit("malloc:dp\n");

	/* For each entry in this directory, print directory + filename */
	for (;;) {
		errno = 0;
		if (readdir_r(dirp, dp, &result) != 0)
			errExit("readdir_r\n");

		if (result == NULL) // At End
			return;

		if (strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
			continue;

		if (!isCurrent)
			printf("%s/", dirpath);
		printf("%s\n",dp->d_name);
	}

	if (errno != 0)
		errExit("readdir\n");

	if (closedir(dirp) == -1)
		errMsg("closedir\n");
}

int
main(int argc, char *argv[])
{
	if (argc > 1 && strcmp(argv[1],"--help") == 0)
		usageErr("%s [dir...]\n", argv[0]);

	if (argc == 1)
		listFiles(".");
	else
		for(argv++; *argv; argv++)
			listFiles(*argv);

	exit(EXIT_SUCCESS);
}
