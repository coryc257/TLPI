/*
 * 12_1.c
 *
 *  Created on: May 18, 2021
 *      Author: cory
 */
#include "tlpi_hdr.h"
#include "8_hdr.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define PROC_DIR "/proc/"

typedef enum LINE_TYPE {
	PL_NAME,
	PL_PID,
	PL_UID_R,
	PL_IRRELEVANT
} LINE_TYPE;
typedef struct {
	int status;
	char *message;
	void *object;
} RETURN;

typedef struct x_dirent {
	char directory[256];
	struct x_dirent *next;
} X_DIRENT;

typedef struct pid_info {
	char *name;
	pid_t pid;
	uid_t uid;
	struct pid_info *next;
} PID_INFO;

static void
die_with_error(const RETURN *ret) {
	errExit("walk_pid(%d): %s\n", ret->status,
			ret->message == NULL ? "???" : ret->message);
}

static LINE_TYPE
get_line_name(char buffer[1000], int *colon)
{
	*colon = -1;
	for (int j = 0; j < 1000; j++)
	{
		if (buffer[j] == ':')
		{
			buffer[j] = '\0';
			*colon = j;
			break;
		}

	}

	if(strcmp(buffer,"Name") == 0)
		return PL_NAME;
	else if(strcmp(buffer, "Pid") == 0)
		return PL_PID;
	else if(strcmp(buffer, "Uid") == 0)
		return PL_UID_R;
	else
		return PL_IRRELEVANT;
}

static char
*pull_value(char buffer[1000], int colon)
{
	int die, index, len;
	char *name;
	name = NULL;
	die = 0;
	if(colon == -1)
		return "\0";
	for (int j = colon+1; j < 1000; j++)
	{
		switch(buffer[j]) {
		case '\0':
			index = -1;
			die = 1;
			break;
		case ' ':
			break;
		case '\t':
			break;
		default:
			index = j;
			die = 1;
			break;
		}
		if(die == 1)
			break;
	}

	if (index != -1)
	{
		len = strlen(&buffer[index])+1;
		name = malloc(sizeof(char)*len);
		if (name != NULL)
			strcpy(name,&buffer[index]);
	}
	return name;
}

static void
get_name(char *name, PID_INFO *pid_info)
{
	if (name != NULL)
	{
		pid_info->name = name;
	}
}

static void
limit_string(char *p)
{
	int override = 0;
	while(*p != '\0')
	{
		if((*p == ' ' || *p == '\t') || override == 1)
		{
			*p = '\0';
			override = 1;
		}
		p++;
	}
}

static void
get_pid(char *pid_s, PID_INFO *pid_info)
{
	char *p;
	long pid;
	if (pid_s == NULL)
		pid_info->pid = -1;
	p = pid_s;
	limit_string(p);
	pid = strtol(pid_s, &p, 10);
	if(*p != '\0')
	{
		pid_info->pid = -1;
		return;
	}
	pid_info->pid = pid;
	free(pid_s);
}

static void
get_uid_r(char *uid_s, PID_INFO *pid_info)
{
	char *p;
	long uid;
	if (uid_s == NULL)
		pid_info->uid = -1;

	p = uid_s;
	limit_string(p);
	uid = strtol(uid_s, &p, 10);
	if(*p != '\0')
	{
		pid_info->uid = -1;
		return;
	}
	pid_info->uid = uid;
	free(uid_s);
}

static void
check_line(char buffer[1000], PID_INFO *pid_info)
{
	int colon;
	switch(get_line_name(buffer, &colon)){
	case PL_IRRELEVANT:
		break;
	case PL_NAME:
		get_name(pull_value(buffer, colon), pid_info);
		break;
	case PL_PID:
		get_pid(pull_value(buffer, colon), pid_info);
		break;
	case PL_UID_R:
		get_uid_r(pull_value(buffer, colon), pid_info);
		break;
	default:
		break;
	}
}

static void
process_pid(char directory[256], uid_t uid, PID_INFO *user_pids)
{
	int fd, pos, numRead, bad_line;
	long pid;
	char full_path[269];
	char buffer[1000];
	char *endptr;

	pid = strtol(directory,&endptr,10);
	if(*endptr != '\0')
		return;

	full_path[0] = '\0';
	strcat(full_path,PROC_DIR);
	strcat(full_path,directory);
	strcat(full_path,"/status");

	if((fd = open(full_path, O_RDONLY)) == -1)
		printf("Cannot Find PID:%s\n", full_path);
	else
	{
		pos = 0;
		bad_line = 0;

		while((numRead = read(fd,&buffer[pos],1)) == 1)
		{
			if(bad_line == 1 && buffer[pos] != '\n')
				continue;
			else if (bad_line == 1)
			{
				pos = 0;
				bad_line = 0;
				continue;
			}

			if(buffer[pos] == '\n')
			{
				buffer[pos] = '\0';
				check_line(buffer,user_pids);
				pos = 0;
				bad_line = 0;
				continue;
			}

			pos++;
			if(pos == 1000)
				pos = 0, bad_line = 1;
		}
		int x;
		x = 5;
	}
}

static void walk_pid(RETURN *ret, uid_t uid)
{
	struct dirent *dir_entry = NULL;
	DIR *dir = NULL;
	X_DIRENT *head, *list, *last;

	dir = opendir(PROC_DIR);
	if (dir == NULL)
	{
		ret->message = "opendir(PROC_DIR)";
		die_with_error(ret);
	}

	list = malloc(sizeof(X_DIRENT));
	last = head = list;

	while((dir_entry = readdir(dir)) != NULL)
	{
		strcpy(list->directory, dir_entry->d_name);
		last = list;
		list->next = malloc(sizeof(X_DIRENT));
		list = list->next;
		list->next = NULL;
	}
	closedir(dir);

	if(last != head)
	{
		free(last->next);
		last->next = NULL;
		ret->object = head;
	}
	else
	{
		free(head);
		ret->status = -1;
		ret->object = NULL;
		ret->message = "No PIDS?";
	}
}

void zero_pid(PID_INFO *user_pids) {
	user_pids->name = NULL;
	user_pids->pid = -1;
	user_pids->uid = -1;
}

static void
output_user_pids(RETURN *ret, uid_t uid)
{
	X_DIRENT *list;
	PID_INFO *user_pids, *head, *lag;
	if(ret->object == NULL)
		errExit("output_user_pids: Null Object\n");

	user_pids = malloc(sizeof(PID_INFO));
	if (user_pids == NULL)
		errExit("malloc\n");
	head = user_pids;
	zero_pid(user_pids);
	list = (X_DIRENT*)ret->object;
	while(list != NULL)
	{
		process_pid(list->directory, uid, user_pids);
		if(user_pids->uid == uid)
		{
			user_pids->next = malloc(sizeof(PID_INFO));
			if(user_pids->next == NULL)
				errExit("malloc");
			user_pids = user_pids->next;
		}
		else
		{
			if(user_pids->name != NULL)
			{
				free(user_pids->name);
			}
		}
		zero_pid(user_pids);
		list = list->next;
	}

	user_pids = head;
	while(user_pids != NULL)
	{
		if(user_pids->uid == uid)
		{
			printf("UID: %ld, PID: %ld, PROCESS: %s\n",
					(long)user_pids->uid,
					(long)user_pids->pid,
					user_pids->name == NULL ? "???" : user_pids->name);
		}
		if(user_pids->name != NULL)
		{
			free(user_pids->name);
		}
		lag = user_pids;
		user_pids = user_pids->next;
		free(lag);
	}
}



int
__12_1__main(int argc, char *argv[])
{
	uid_t	uid 		= 0;
	char	*user 		= NULL;
	RETURN	ret;

	if(argc != 2)
		usageErr("%s <username>", argv[0]);


	user = argv[1];
	uid = userIdFromName(user);

	if (uid == -1)
		errExit("User:'%s' not found!!\n", user);

	memset(&ret,0,sizeof(RETURN));
	walk_pid(&ret,uid);
	if(ret.status == 0)
		output_user_pids(&ret, uid);
	else
		die_with_error(&ret);

	if(ret.status == 0)
		return 0;
	else
		die_with_error(&ret);


	return 0;
}
