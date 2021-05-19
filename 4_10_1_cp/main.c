/*
 * main.c
 *
 *  Created on: May 6, 2021
 *      Author: cory
 */
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include "4_10_2_cp.h"

// GETOPT OUTPUT
extern char *optarg;
extern int optind, opterr, optopt;


// The, uh... data?
// { input_file, output_file, fd_input_file, fd_output_file }
static FILE_OPTIONS file_options;

// <program> -i <input file> -o <output file>
int // Copy a file somewhere
main (int argc, char *argv[])
{
	init();
	parse_input(argc, argv); // And Open Files
	show_options();
	copy_file();
	close_files();
}

void // Handle arguments, open files
parse_input (int argc, char *argv[])
{
	int current_opt;

	while ((current_opt = getopt(argc, argv, "i:o:")) != -1)
	{
		switch (current_opt)
		{
			case 'i':
				check_for_file("input");
				file_options.input_file = optarg;
				load_file(CPFT_input);
				break;
			case 'o':
				check_for_file("output");
				file_options.output_file = optarg;
				load_file(CPFT_output);
				break;
		}
	}
}

void // Make sure we have a file to try and open
check_for_file (const char *file_type)
{
	if (optarg == NULL || optarg[0]=='\0' || optarg[0]=='-')
			errExit("No %s file provided", file_type);
}

void // Open the file given the settings
open_file(int *fd, int flags, int mode, const char* file, const char* type)
{
	int fopen_ret;

	if(mode == -1)
		fopen_ret = open(file, flags);
	else
		fopen_ret = open(file, flags, mode);

	if (fopen_ret == -1) {
		errExit("Error Opening %s file: %s",
				type,
				file);
	}
	*fd = fopen_ret;
}

void // Load the file given the type
load_file(CP_FILE_TYPE type)
{

	switch (type)
	{
		case CPFT_input:
			open_file(&(file_options.fd_input_file),
					O_RDONLY,
					-1,
					(const char*)(file_options.input_file),
					"input");
			break;
		case CPFT_output:
			open_file(&(file_options.fd_output_file),
					O_RDWR | O_CREAT | O_TRUNC,
					S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP,
					(const char*)(file_options.output_file),
					"output");
			break;
		default:
			errExit("Invalid File Type: %d", (int)type);
	}
}

void // Show what will be done
show_options (void)
{
	printf("Input File: '%s'\nOutput File: '%s'\n",
			file_options.input_file, file_options.output_file);
}

void // Ensure we have data we expect by default
init(void)
{
	file_options.fd_input_file = 0;
	file_options.fd_output_file = 0;
}

void // Copy the file to the specified location
copy_file()
{
#define BUF_SIZE 255
	char buf[BUF_SIZE];
	int currentRead, currentWrite;
	while((currentRead = read(file_options.fd_input_file, buf, BUF_SIZE)) != 0)
	{
		if(currentRead == -1)
		{
			errExit("Error Copying File: read\n");
		}

		if((currentWrite = write(file_options.fd_output_file,
				buf,
				currentRead)) == -1
				|| currentWrite != currentRead)
		{
			errExit("Error Copying File: write\n");
		}
	}
}

void // Close any open files
close_files()
{
	if(file_options.fd_input_file != 0)
		close(file_options.fd_input_file);
	if(file_options.fd_output_file != 0)
		close(file_options.fd_output_file);
}
