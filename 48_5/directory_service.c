/*
 * directory_service.c
 *
 *  Created on: Jul 4, 2021
 *      Author: cory
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <glib-2.0/glib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "lib/directory_service.h"

static void
maybe_create(const char *str)
{
	mkdir(str, 	S_IRUSR | S_IWUSR | S_IXUSR |
				S_IRGRP | S_IWGRP | S_IXGRP |
				S_IROTH | 			S_IXOTH);
}

static void
create_directory_server(const char *server_root)
{
	GString *rcn;
	char *dup_server_root, *dir_part, *s_server_root;
	dup_server_root = strdup(server_root);
	rcn = g_string_new("");

	while ((dir_part = strtok_r(dup_server_root, "/", &s_server_root)) != NULL) {
		dup_server_root = NULL;
		g_string_append(rcn,"/");
		g_string_append(rcn,dir_part);
		maybe_create((const char *)rcn->str);
	}

	g_free(rcn);
	free(dup_server_root);
}

void
init_directory_server(const char *server_root)
{
	struct stat sb;
	GString *key_file, *sync_file, *key_list;
	key_t file_key, sync_key, list_key;
	int sem_id, mem_id, msg_id;


	if (stat(server_root, &sb) == -1) {
		if (server_root[0] != '/') {
			errExitEN(EINVAL, "You Must provide an absolute path: %s\n", server_root);
		}
		create_directory_server(server_root);
		if (stat(server_root, &sb) == -1)
			errExitEN(EACCES, "Cannot create directory server: %s\n", server_root);
	}

	key_file = g_string_new(server_root);
	g_string_append(key_file,"/");
	g_string_append(key_file,"key_file");

	sync_file = g_string_new(server_root);
	g_string_append(sync_file,"/");
	g_string_append(sync_file,"sync_file");

	key_list = g_string_new(server_root);
	g_string_append(key_list,"/");
	g_string_append(key_list,"key_list");

	if (stat(key_file->str, &sb) == -1) {
		maybe_create(key_file->str);
		if (stat(key_file->str, &sb) == -1) {
			errExitEN(EACCES, "Cannot create directory server key file: %s\n", key_file->str);
		}
	}

	if (stat(sync_file->str, &sb) == -1) {
		maybe_create(sync_file->str);
		if (stat(sync_file->str, &sb) == -1) {
			errExitEN(EACCES, "Cannot create directory server sync file: %s\n", sync_file->str);
		}
	}

	if (stat(key_list->str, &sb) == -1) {
			maybe_create(key_list->str);
			if (stat(key_list->str, &sb) == -1) {
				errExitEN(EACCES, "Cannot create directory server list file: %s\n", key_list->str);
			}
		}

	file_key = ftok(key_file->str,1);
	sync_key = ftok(sync_file->str,1);
	list_key = ftok(key_list->str,1);

	msg_id = msgget(file_key, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (msg_id == -1)
		errExit("Cannot Create Message Queue\n");

	sem_id = semget(sync_key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (sem_id == -1)
		errExit("Cannot Create Semaphore\n");

	mem_id = shmget(list_key, 2048*10001, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (mem_id == -1)
		errExit("Cannot Create Directory Listing\n");

	semctl(sem_id,0,SETVAL,1);
	g_string_free(key_file, TRUE);
	g_string_free(sync_file, TRUE);
	g_string_free(key_list, TRUE);
}

BASE_INFO *
get_base_info(const char *server_root)
{
	BASE_INFO *p;
	GString *key_file, *sync_file, *key_list;

	p = malloc(sizeof(BASE_INFO));

	key_file = g_string_new(server_root);
	g_string_append(key_file,"/");
	g_string_append(key_file,"key_file");

	sync_file = g_string_new(server_root);
	g_string_append(sync_file,"/");
	g_string_append(sync_file,"sync_file");

	key_list = g_string_new(server_root);
	g_string_append(key_list,"/");
	g_string_append(key_list,"key_list");

	p->file_key = ftok(key_file->str,1);
	p->sync_key = ftok(sync_file->str,1);
	p->list_key = ftok(key_list->str,1);

	p->file_id = msgget(p->file_key, S_IRUSR | S_IWUSR);
	if (p->file_id == -1)
		errExit("Cannot Open Message Queue\n");

	p->sync_id = semget(p->sync_key, 1, S_IRUSR | S_IWUSR);
	if (p->sync_id == -1)
		errExit("Cannot Open Semaphore\n");

	p->list_id = shmget(p->list_key, 2048*10001, S_IRUSR | S_IWUSR);
	if (p->list_id == -1)
		errExit("Cannot Open Directory Listing\n");

	g_string_free(key_file, TRUE);
	g_string_free(sync_file, TRUE);
	g_string_free(key_list, TRUE);

	return p;
}

int
get_lock(BASE_INFO *info)
{
	struct sembuf x[1];
	x[0].sem_flg = 0;
	x[0].sem_op = -1;
	x[0].sem_num = 0;
	return semop(info->sync_id,x,1);
}

int
release_lock(BASE_INFO *info)
{
	struct sembuf x[1];
	x[0].sem_flg = 0;
	x[0].sem_op = 1;
	x[0].sem_num = 0;
	return semop(info->sync_id,x,1);
}

static void
get_item_size(int *key_size, char *item, size_t size)
{
	for (int j = 0; j <= size; j++) {
		if (item[j] == '\0') {
			break;
		}
		(*key_size)++;
	}
}


static void
construct_output(DIRECTORY_MESSAGE *item, DIRECTORY_LISTING *found)
{
	int key_size = 0, value_size = 0;

	item->mtype = getpid();
	get_item_size(&key_size, found->key, sizeof(found->key));
	get_item_size(&value_size, found->value, sizeof(found->value));


	memset(&item->message[0],0,sizeof(item->message));
	strncpy(&item->message[0], found->key, key_size);
	*(item->message + key_size) = '\0';
	strncpy(&item->message[key_size+1], found->value, value_size);
	/*
	 1234567890
	 key0valu0?
	 3+1+4 == 8
	*/
	*(item->message + key_size + 1 + value_size) = '\0';
}

static void
construct_return(DIRECTORY_MESSAGE *msg, DIRECTORY_ITEM *out)
{
	int v_index = 0;
	out->key = strdup(&msg->message[0]);
	while(1) {
		if (msg->message[v_index] == '\0') {
			v_index++;
			break;
		}
		v_index++;
	}
	out->value = strdup(&msg->message[v_index]);
}

static int
modify_entry(const char *server_root, const char *key, const char *value, ENTRY_ACTION action, DIRECTORY_ITEM *out)
{
	BASE_INFO *info;
	DIRECTORY_LISTING *listing, *pos, *open_spot;
	DIRECTORY_MESSAGE dmsg;
	struct timespec start, finish;
	pid_t child;
	sigset_t block, ready, prev;
	int status;
	int ret;

	info = get_base_info(server_root);
	sigfillset(&block);
	sigemptyset(&ready);

	if (action == EA_FIX) {
		semctl(info->sync_id,0,SETVAL,1);
		return 0;
	}

	if (action == EA_ADD || action == EA_UPDATE) {
		if (key == NULL || value == NULL)
			return EINVAL;
		if (strlen(key) > 500 || strlen(value) > 1500)
			return EINVAL;
	}

	if (action == EA_DELETE || action == EA_GET) {
		if (key == NULL)
			return EINVAL;
		if (strlen(key) > 500)
			return EINVAL;
	}

	if (action == EA_GET && out == NULL)
		return EINVAL;

	// Server will deadlock if something makes this go boom
	// BLOCK ALL SIGNALS, set them back at the end
	sigprocmask(SIG_SETMASK, &block, &prev);

	if (get_lock(info) == -1)
		errExit("Error Locking\n");

	// Run the critical section in a try/catch process to prevent deadlock
	switch(child = fork()) {
	case -1:
		release_lock(info);
		break;
	case 0:
		{
			sigprocmask(SIG_SETMASK,&ready,NULL);
			open_spot = NULL;
			pos = listing = (DIRECTORY_LISTING *)shmat(info->list_id,NULL,0);
			for (int j = 0; j < 501; j++) {

				if (strncmp(key,pos->key, 500) == 0) {
					if (action == EA_ADD) {
						_exit(EALREADY);
					} else if (action == EA_DELETE) {
						pos->in_use = 0;
						_exit(EXIT_SUCCESS);
					} else if (action == EA_UPDATE) {
						memset(pos->value,0,sizeof(pos->value));
						strncat(pos->value,value,strlen(value));
						_exit(EXIT_SUCCESS);
					} else if (action == EA_GET) {
						construct_output(&dmsg, pos);
						msgsnd(info->file_id,&dmsg,sizeof(dmsg.message),0);
						_exit(EXIT_SUCCESS);
					}
					//release_lock(info);
					_exit(EINVAL);
				}
				if (pos->in_use != 257 && open_spot == NULL)
					open_spot = pos;

				pos++;
			}

			if (open_spot == NULL && action == EA_ADD) {
				_exit(EALREADY);
				//release_lock(info);
			} else if (action == EA_ADD) {
				open_spot->in_use = 257;
				memset(open_spot->key,0,sizeof(open_spot->key));
				memset(open_spot->value,0,sizeof(open_spot->value));
				strncat(open_spot->key,key,strlen(key));
				strncat(open_spot->value,value,strlen(value));
				_exit(EXIT_SUCCESS);
			} else if (action == EA_GET) {
				_exit(ENODATA);
			} else {
				_exit(EINVAL);
			}
		}
		break;
	default:
		clock_gettime(CLOCK_REALTIME, &start);
		for (;;) {
			clock_gettime(CLOCK_REALTIME, &finish);
			if ((ret = waitpid(child,&status,WNOHANG)) == child) {
				release_lock(info);
				sigprocmask(SIG_SETMASK,&prev,NULL);
				// an _exit appears to shift the bits, shift them back
				status = ((unsigned int)status)>>8;

				if (status == 0 && action == EA_GET) {
					if ((status = msgrcv(info->file_id, &dmsg, sizeof(dmsg.message),child,IPC_NOWAIT)) == sizeof(dmsg.message)) {
						construct_return(&dmsg,out);
						return 0;
					} else {
						if (status == -1)
							return errno;
						else
							return EBADMSG;
					}
				}
				return status;
			}
			// wait 6 seconds for the process to complete
			if (finish.tv_sec-start.tv_sec >= 6) {
				kill(child, SIGKILL);
				release_lock(info);
				sigprocmask(SIG_SETMASK,&prev,NULL);
				return ETIMEDOUT;
			}
		}
		break;
	}
	//release_lock(info);
	return 0;

}


int
add_entry(const char *server_root, const char *key, const char *value)
{
	return modify_entry(server_root, key, value, EA_ADD, NULL);
}

int
update_entry(const char *server_root, const char *key, const char *value)
{
	return modify_entry(server_root, key, value, EA_UPDATE, NULL);
}

int
delete_entry(const char *server_root, const char *key)
{
	return modify_entry(server_root, key, NULL, EA_DELETE, NULL);
}

void
fix_server(const char *server_root)
{
	modify_entry(server_root, NULL, NULL, EA_FIX, NULL);
}

DIRECTORY_ITEM *
get_entry(const char *server_root, const char *key)
{
	DIRECTORY_ITEM *x;
	int ret;
	x = malloc(sizeof(DIRECTORY_ITEM));
	if ((ret = modify_entry(server_root,key,NULL,EA_GET,x)) != 0)
	{
		errno = ret;
		free(x);
		return NULL;
	}

	return x;
}
