/*
 * main.c
 *
 *  Created on: Jun 26, 2021
 *      Author: cory
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "exec_container.h"

pid_t oe[FOPEN_MAX];

static char *
escape_command(const char *command)
{
	char *ret, *cur, *cmd;
	int len = strlen(command);
	int tot_quote = 0;
	for (int j = 0; j <= len; j++) {
		if (command[j] == '"')
			tot_quote++;
	}
	ret = malloc(sizeof(char)*(tot_quote+1+len));
	cur = ret;
	cmd = (char*)command;
	for (int j = 0; j < tot_quote+1+len; j++) {
		if (*cmd == '"') {
			*cur = '\\';
			cur++;
		}
		*cur = *cmd;
		cur++;
		cmd++;
	}
	*cur = '\0';
	return ret;
}

FILE *
my_popen(const char *command, const char *type)
{
	int mode, pipe_fd[2];
	int close_exec = 0;
	FILE *res;
	EXEC_CONTAINER ec;
	pid_t child;
	char **pass_args;

	pass_args = malloc(sizeof(char*)*3);
	*pass_args = strdup("/bin/sh");
	pass_args++;
	*pass_args = strdup("-c");
	pass_args++;
	*pass_args = escape_command(command);
	pass_args--;
	pass_args--;

	construct_args(3, pass_args, &ec);

	if (strlen(type) == 2)
	{
		if (type[0] == 'e' || type[1] == 'e')
			close_exec = 1;
		if (type[0] == 'r' || type[1] == 'r')
			mode = 1;
		else
			mode = 2;
	} else {
		if (type[0] == 'r')
			mode = 1; //READ
		else
			mode = 2; //WRITE
	}

	pipe(pipe_fd);
	res = fdopen(mode == 1 ? pipe_fd[0] : pipe_fd[1], mode == 1 ? "r" : "a");

	if ((child = fork()) == 0) {

		if (mode == 1) {
			close(pipe_fd[0]);
			close(STDOUT_FILENO);
			dup2(pipe_fd[1], STDOUT_FILENO);
		} else {
			close(pipe_fd[1]);
			close(STDIN_FILENO);
			dup2(pipe_fd[0], STDIN_FILENO);
		}
		execv(ec.exe, ec.args);
		return NULL;
	}

	close(mode == 1 ? pipe_fd[1] : pipe_fd[0]);
	oe[fileno(res)] = child;
	return res;
}


int
my_pclose(FILE *file)
{
	pid_t child;
	int status;
	child = oe[fileno(file)];
	waitpid(child,&status,0);
	return status;
}


int
main(int argc, char *argv[])
{
	FILE *f;
	char data[10000];
	char cdata[3];

	f = my_popen("ls -l","r");
	if (f != NULL) {
		fread(data,sizeof(char),10000,f);
		printf("%s\n",data);
		printf("%d\n", my_pclose(f));
	}

	f = my_popen("cat > file.txt","w");
		if (f != NULL) {
			cdata[0] = 'x';
			cdata[1] = '\n';
			cdata[2] = '\0';
			fwrite(cdata,sizeof(char),3,f);
			fflush(f);
			my_pclose(f);
			printf("Don\n");
			//printf("%s\n",data);
			//printf("%d\n", my_pclose(f));
		}
}
