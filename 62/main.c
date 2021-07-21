/*
 * main.c
 *
 *  Created on: Jul 20, 2021
 *      Author: cory
 */


#define _XOPEN_SOURCE
#define _POSIX_C_SOURCE 200112
#include <termios.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>


#define PASS_MAX 5001

static int
my_isatty(int fd)
{
	struct termios x;
	return tcgetattr(fd,&x) == -1 ? 0 : 1;
}

char *
my_passwd(const char *prompt)
{
	struct termios swap;
	char *ret;
	ret = malloc(sizeof(char)*(PASS_MAX+1));
	memset(ret,0,PASS_MAX);

	printf("%s", prompt);

	tcgetattr(STDIN_FILENO,&swap);
	swap.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &swap);
	read(STDERR_FILENO,ret,PASS_MAX-1);
	printf("\n");
	swap.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &swap);

	return ret;
}

int
main(int argc, char *argv[])
{
	int fd, ftty;
	fd = open("testfile.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftty = open("/dev/tty", O_RDONLY);
	printf("One:%d\n", my_isatty(STDIN_FILENO));
	printf("Zero:%d\n", my_isatty(fd));
	printf("One:%d\n", my_isatty(ftty));

	printf("%s", my_passwd("HI?"));
}
