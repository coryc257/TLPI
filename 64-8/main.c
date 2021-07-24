/*
 * main.c
 *
 *  Created on: Jul 23, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "vim_auto.h"
#include "read_line.h"
#include "pty_fork.h"
#include "tty_functions.h"

#define MAX_SNAME 1000

struct termios ttyOrig;

int
main(int argc, char *argv[])
{
	int fd, master, terminal, status;
	char *s1, *s2;
	pid_t slave;
	BUFFERED_STREAM_READER stream;
	VIM_COMMAND *cmd;
	unsigned char *x;
	struct winsize ws;
	char esc[1];
	esc[0] = 27;

	if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
		errExit("tcgetattr");
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
		errExit("ioctl-TIOCGWINSZ");

	master =  getpt();
	grantpt(master);
	unlockpt(master);
	s1 = ptsname(master);
	s2 = strdup(s1);

	switch(slave = fork()) {
	case -1:
		errExit("fork\n");
	case 0:
		close(master);
		fd = open(s2, O_RDWR);

		if (fd == -1) {
			write(STDERR_FILENO, "z\n", 2);
		}

		dup2(fd,STDIN_FILENO);
		dup2(fd,STDOUT_FILENO);
		dup2(fd,STDERR_FILENO);

		execl("/bin/vim", "/bin/vim", (char*)NULL);

		break;
	default:
		write(master,":w file.txt\n",strlen(":w file.txt\n"));
		write(master,"idog", strlen("idog"));
		write(master,esc,1);
		write(master,":wq\n",strlen(":wq\n"));

		waitpid(slave, &status, 0);
		break;
	}


	return 0;

	fd = open("/home/cory/script.vs", O_RDONLY);

	stream = buffered_stream_reader_new(fd, 4096);

	ttySetRaw(STDOUT_FILENO, NULL);

	dup2(STDOUT_FILENO, terminal);

	while((cmd = fetch_command(stream)) != NULL) {
		switch (cmd->bytes[0]) {
		case '*':
			if (write(terminal, ":", 1) != 1) {
				errExit("write\n");
			}
			for (x = (cmd->bytes + 1); *x != '\n'; x++) {
				if(write(terminal, x, 1) != 1) {
					errExit("write\n");
				}
			}
			if(write(terminal, "\r", 1) != 1) {
				errExit("write\n");
			}
			if(write(terminal, "\n", 1) != 1) {
				errExit("write\n");
			}
			//tcflush(terminal, TCIOFLUSH);
			break;
		case '-':
			break;
		case '+':
			break;
		default:
			continue;
		}
		//printf("%s", cmd->bytes);
		while (read(terminal,x,1)) {
			printf("%c",x);
		}
	}

}

