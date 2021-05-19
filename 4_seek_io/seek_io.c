/*
 * seek_io.c
 *
 *  Created on: May 8, 2021
 *      Author: cory
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#include <string.h>

int
main(int argc, char **argv)
{
	size_t len;
	off_t offset;
	int fd, ap, j;
	unsigned char *buf;
	ssize_t numRead, numWritten;

	if (argc < 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file {r<length>|R<length|w<string>|s<offset}...\n", argv[0]);

	// fd will be -1 if bad, else it's good
	fd = open(argv[1], O_RDWR | O_CREAT, // Read/Write Create
			S_IRUSR | S_IWUSR | //rw
			S_IRGRP | S_IWGRP | //rw
			S_IROTH | S_IWOTH); //rw

	// cannot open file
	if (fd == -1)
		errExit("open:%s\n", argv[1]);

	for (ap = 2; ap < argc; ap++) {
		switch (argv[ap][0]) {
			case 'r': /* Display bytes at current offset, as text */
			case 'R': /* Display bytes at current offset, as hex  */
				len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
				buf = malloc(len);
				if (buf == NULL)
					errExit("malloc:%d\n", (long)len);

				// read len bytes into buf from current offset
				numRead = read(fd, buf, len);
				if (numRead == 0) {
					printf("%s: end-of-file\n", argv[ap]);
				}
				else {
					// print # bytes
					printf("%s: ", &argv[ap][1]);
					if(argv[ap][0] == 'r')
					{
						for (j = 0; j < numRead; j++)
						{
							printf("%c", isprint(buf[j]) ? buf[j] : '?');
						}
					}
					else
					{
						for (j = 0; j < numRead; j++)
						{
							printf("%02x ", buf[j]);
						}
					}
					printf("\n");
				}

				// safe to free buf because errExit above
				free(buf);
				break;
			case 'w': /* Write string to file given current offset */
				numWritten = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
				if(numWritten == -1)
					errExit("write");
				printf("%s: Wrote %ld bytes\n", &argv[ap][1], (long)numWritten);
				break;
			case 's': /* Seek to current position */
				offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
				if (lseek(fd, offset, SEEK_SET) == -1)
					errExit("lseek");
				printf("%s: seek succeeded\n", &argv[ap][1]);
				break;
			default:
				cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
		}
	}

	close(fd);
	exit(EXIT_SUCCESS);
}
