/*
 * main.c
 *
 *  Created on: May 6, 2021
 *      Author: cory
 */

#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
extern int optind, opterr, optopt;
//extern char *optarg;


#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')
static void
/* Print "usage" message and exit */
usageError(char *progName, char *msg, int opt)
{
if (msg != NULL && opt != 0)
//fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
//fprintf(stderr, "Usage: %s [-p arg] [-x]\n", progName);
exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
#define BUF_SIZE 10
	opterr = 0;
	int opt, output_fd;
	Boolean append = FALSE;
	char *file_name = NULL;
	char c[BUF_SIZE];
	int numRead;

	while ((opt = getopt(argc, argv, "a")) != -1) {
		/*printf("opt =%3d (%c); optind = %d", opt, printable(opt), optind);
		if (opt == '?' || opt == ':')
			printf("; optopt =%3d (%c)", optopt, printable(optopt));
		printf("\n");*/
		switch (opt) {
			case 'a':
				append = TRUE;
				break;
			case ':':
				usageError(argv[0], "Missing argument", optopt);
				break;
			case '?':
				usageError(argv[0], "Unrecognized option", optopt);
				break;
			default:
				fatal("Unexpected case in switch()");
		}
	}

	if(append == TRUE)
	{
		printf("The file will be appended\n");
	}

	if(optind < argc)
	{
		file_name = argv[optind];
	}

	if (file_name != NULL)
	{
		printf("FileName:%s\n",file_name);
		output_fd = open(file_name, O_RDWR | O_CREAT | (append == TRUE ? O_APPEND : O_TRUNC),
				S_IRUSR | S_IWUSR | //rw
				S_IWGRP | S_IRGRP | //rw
				S_IROTH | S_IWOTH); //rw
		if(output_fd == -1)
			errExit("unable to open file:%s\n", file_name);
	}
	else
	{
		errExit("No File");
	}

	while((numRead = read(STDIN_FILENO, c, BUF_SIZE)) != -1)
	{
		if(numRead == EOF || numRead == 0)
			break;
		write(output_fd,c,numRead);
		//fflush(output_fd);
	}
	close(output_fd);

	/*if (xfnd != 0)
		printf("-x was specified (count=%d)\n", xfnd);
	if (pstr != NULL)
		printf("-p was specified with the value \"%s\"\n", pstr);
	if (optind < argc)
		printf("First nonoption argument is \"%s\" at argv[%d]\n", argv[optind],
				optind);*/
	exit(EXIT_SUCCESS);
}
