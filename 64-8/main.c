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
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "vim_auto.h"
#include "read_line.h"
#include "pty_fork.h"
#include "tty_functions.h"

#define MAX_SNAME 1000

struct termios ttyOrig;

void write_bytes(int terminal, unsigned char *x, VIM_COMMAND *cmd) {
	for (x = (cmd->bytes + 1); *x != '\n'; x++) {
		if (write(terminal, x, 1) != 1) {
			errExit("write\n");
		}
	}
}

void send_commands(int terminal)
{
	int fd;
	BUFFERED_STREAM_READER stream;
	VIM_COMMAND *cmd;
	unsigned char *x;
	char esc[1];

	esc[0] = 27;
	fd = open("/home/cory/script.vs", O_RDONLY);
	stream = buffered_stream_reader_new(fd, 4096);

	while ((cmd = fetch_command(stream)) != NULL) {
		switch (cmd->bytes[0]) {
		case '*':
			if (write(terminal, ":", 1) != 1) {
				errExit("write\n");
			}
			write_bytes(terminal, x, cmd);
			if (write(terminal, "\n", 1) != 1) {
				errExit("write\n");
			}
			//tcflush(terminal, TCIOFLUSH);
			break;
		case '-':
			write(terminal, "i", 1);
			write_bytes(terminal, x, cmd);
			write(terminal, esc, 1);
			break;
		case '+':
			write(terminal, "o", 1);
			write(terminal, esc, 1);
			break;
		default:
			continue;
		}
	}
}

int
main(int argc, char *argv[])
{
	int fd, master, status;
	char *s1, *s2;
	pid_t slave;
	struct winsize ws;


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
		send_commands(master);
		/*write(master,":w file.txt\n",strlen(":w file.txt\n"));
		write(master,"idog", strlen("idog"));
		write(master,esc,1);
		write(master,":wq\n",strlen(":wq\n"));*/

		waitpid(slave, &status, 0);
		break;
	}


	return 0;


}

