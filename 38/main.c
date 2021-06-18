/*
 * main.c
 *
 *  Created on: Jun 17, 2021
 *      Author: cory
 */
#define _BSD_SOURCE
#define _XOPEN_SOURCE
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include "tlpi_pwd.h"
#include "tlpi_hdr.h"
#include "exec_container.h"
#include <stdio.h>
#include <getopt.h>

extern char *optarg;
extern int optopt, optind, opterr;
static char *root = "root";

int
main (int argc, char *argv[])
{
#define BIG_BUF_LEN 4000
	opterr = 0;
	EXEC_CONTAINER ec;
	char *username, *encrypted, *big_buf, *big_buf2;
	struct spwd spwd, *spwd_result;
	struct passwd pwd, *pwd_result;
	int opt, execargs, got_user = 0, arg_index;

	if ((big_buf = malloc(sizeof(char)*BIG_BUF_LEN)) == NULL)
		errExit("malloc\n");
	if ((big_buf2 = malloc(sizeof(char)*BIG_BUF_LEN)) == NULL)
			errExit("malloc\n");
	username = root;

	if (strcmp(argv[1],"-u") == 0) {
		if (argc < 4)
			usageErr("%s [-n USER] command [[ARG] ...]\n");
		username = argv[2];
		execargs = argc-3;
		arg_index = 3;
	} else {
		execargs = argc-1;
		arg_index = 1;
	}

	if (execargs == 0)
		errExit("No Command\n");

	construct_args(execargs,&(argv[arg_index]),&ec);
	getspnam_r(username,&spwd,big_buf,BIG_BUF_LEN,&spwd_result);
	if (spwd_result == NULL)
			errExit("getspnam_r\n");
	getpwnam_r(username,&pwd,big_buf2,BIG_BUF_LEN,&pwd_result);
	if (pwd_result == NULL)
		errExit("getpwnam_r\n");

	{
		char *password;
		char *p;
		password = getpass("Password:");
		encrypted = crypt(password,spwd_result->sp_pwdp);
		for (p = password; *p != '\0';)
			*p++ = '\0';
	}

	if (strcmp(encrypted,spwd_result->sp_pwdp) != 0)
		errExit("INVALID PASSWORD\n");

	execv(ec.exe,ec.args);

}
