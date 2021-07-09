/*
 * mq_impl.c
 *
 *  Created on: Jul 6, 2021
 *      Author: cory
 */

#include "this.h"
#include "tlpi_hdr.h"
#include "tlpi_pwd.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/acl.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <sys/mman.h>

#define MAX_CNAME (255*2)+9
#define ALLOC_CNAME malloc(sizeof(char)*(MAX_CNAME))

static int
init_names(TALK_TYPE *ref, const char *chat_user)
{
	uid_t them, me;
	uid_t upper, lower;
	them = userIdFromName(chat_user);
	me = getuid();


	if (them == me) {
		errno = EINVAL;
		return -1;
	}

	ref->user1 = lower = min(them,me);
	ref->user2 = upper = max(them,me);

	if ((ref->chat_name__1 = ALLOC_CNAME) == NULL) {
		errno = ENOMEM;
		return -1;
	}
	if ((ref->chat_name__2 = ALLOC_CNAME) == NULL) {
		free(ref->chat_name__1);
		ref->chat_name__1 = NULL;
		errno = ENOMEM;
		return -1;
	}
	if ((ref->security_file = malloc(sizeof(char)*(PATH_MAX+1))) == NULL) {
		errno = ENOMEM;
		return -1;
	}


	snprintf(ref->chat_name__1, MAX_CNAME, "/talkt_%d_%d", lower, upper);
	snprintf(ref->chat_name__2, MAX_CNAME, "/talkt_%d_%d", upper, lower);
	snprintf(ref->security_file, PATH_MAX+1, "/talkt_%d_%d", lower, upper);
	return 0;
}

static void
create_mask(TALK_TYPE *ref, acl_t f_acl, int type_of_mask)
{
	uid_t *f_uid;
	acl_entry_t f_entry;
	acl_permset_t f_perm;
	if (acl_create_entry(&f_acl, &f_entry) == -1) {
		unlink(ref->security_file);
		errExit("acl_create_entry\n");
	}
	if (acl_get_permset(f_entry, &f_perm) == -1) {
		unlink(ref->security_file);
		errExit("acl_get_permset\n");
	}
	if (acl_set_tag_type(f_entry, type_of_mask) == -1) {
		unlink(ref->security_file);
		errExit("acl_set_tag_type\n");
	}
	if (type_of_mask == ACL_USER) {
		f_uid = acl_get_qualifier(f_entry);
		*f_uid = getuid()==ref->user1 ? (ref->user2) : (ref->user1);
		if (acl_set_qualifier(f_entry,f_uid) == -1) {
			unlink(ref->security_file);
			errExit("acl_set_qualifier\n");
		}
	}
	if (acl_clear_perms(f_perm) == -1) {
		unlink(ref->security_file);
		errExit("acl_clear_perms\n");
	}
	if (acl_add_perm(f_perm, ACL_READ) == -1) {
		unlink(ref->security_file);
		errExit("acl_add_perm\n");
	}
	if (acl_add_perm(f_perm, ACL_WRITE) == -1) {
		unlink(ref->security_file);
		errExit("acl_add_perm\n");
	}
	if (acl_set_permset(f_entry, f_perm) == -1) {
		unlink(ref->security_file);
		errExit("acl_set_permset\n");
	}
}

static int
init_sync(TALK_TYPE *ref)
{
	int fd, is_maker;
	acl_t f_acl;
	mode_t prev;
	struct mq_attr ma;

	ma.mq_maxmsg = 10;
	ma.mq_msgsize = 4096;

	fd = shm_open(ref->security_file, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1 && errno == EEXIST) {
		fd = shm_open(ref->security_file, O_RDWR, S_IRUSR | S_IWUSR);
		if (fd == -1)
			errExit("Cannot Open Security File\n");
		is_maker = 0;

	} else if (fd == -1) {
		errExit("Cannot Open Security File\n");

	} else {
		is_maker = 1;
	}


	ref->security = mmap(NULL,4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (is_maker) {
		ftruncate(fd,4096);
		memset(ref->security,0,4096);
		ref->security->lower_user = ref->user1;
		ref->security->upper_user = ref->user2;
		ref->security->lower_ready = 0;
		ref->security->upper_ready = 0;
		RAND_bytes(ref->security->security_token,TOKEN_LEN);
		RAND_bytes(ref->security->auth_token,TOKEN_LEN);

		f_acl = acl_get_fd(fd);

		create_mask(ref, f_acl, ACL_MASK);
		create_mask(ref, f_acl, ACL_USER);
		if (acl_set_fd(fd,f_acl) == -1) {
			unlink(ref->security_file);
			errExit("acl_set_fd");
		}

		//mq_unlink(ref->chat_name__1);
		//mq_unlink(ref->chat_name__2);


		//ref->chat_1 = mq_open(ref->chat_name__1,O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);
		//ref->chat_2 = mq_open(ref->chat_name__2,O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);





	} else {
		//ref->chat_1 = mq_open(ref->chat_name__1, O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);
		//ref->chat_2 = mq_open(ref->chat_name__2, O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);
	}

	prev = umask(0);
	if (ref->security->lower_user == getuid()) {
		ref->my_sem = sem_open(ref->chat_name__2, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH, 256);
		ref->input = mq_open(ref->chat_name__2, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IWOTH, &ma);
		if (ref->input == -1) {
			ref->input = mq_open(ref->chat_name__2, O_RDWR);
			if (ref->input == -1)
				errExit("Cannot Open Input Queue\n");
		}
		ref->output = -257;
		if (ref->my_sem != NULL) {
			ref->security->lower_ready = 257;
			sem_post(ref->my_sem);
		}
		else {
			if ((ref->my_sem = sem_open(ref->chat_name__2, O_RDWR)) == NULL)
				errExit("Cannot Open Semaphore\n");
		}
		//ref->output = mq_open(ref->chat_name__1, O_WRONLY);
	} else {
		ref->my_sem = sem_open(ref->chat_name__1, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH, 256);
		ref->input = mq_open(ref->chat_name__1, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IWOTH, &ma);
		if (ref->input == -1) {
			ref->input = mq_open(ref->chat_name__1, O_RDWR);
			if (ref->input == -1)
				errExit("Cannot Open Input Queue\n");
		}
		ref->output = -257;
		if (ref->my_sem != NULL) {
			ref->security->upper_ready = 257;
			sem_post(ref->my_sem);
		}
		else {
			if ((ref->my_sem = sem_open(ref->chat_name__1, O_RDWR)) == NULL)
				errExit("Cannot Open Semaphore\n");
		}
		//ref->output = mq_open(ref->chat_name__2, O_WRONLY);
	}
	umask(prev);

	// Set Up Encryption
	if (AES_set_encrypt_key(ref->security->security_token,256,&(ref->ekey)) == -1)
		errExit("AES KEY SET\n");
	if (AES_set_decrypt_key(ref->security->security_token,256,&(ref->dkey)) == -1)
			errExit("AES KEY SET\n");

	ref->their_sem = NULL;
	return 0;

}

static void *
__t_monitor_input(void *arg)
{
	MESSAGE_TYPE msg, pmsg;
	char print_buffer[sizeof(MESSAGE_TYPE)*2];
	TALK_TYPE *ref = (TALK_TYPE*)arg;
	int semval;

	/*if (AES_set_encrypt_key(ref->security->security_token,256,&(ref->ekey)) == -1)
		errExit("AES KEY SET\n");
	if (AES_set_decrypt_key(ref->security->security_token,256,&(ref->dkey)) == -1)
			errExit("AES KEY SET\n");*/

	for (;;) {
		sleep(1);
		sem_getvalue(ref->my_sem,&semval);
		memset(&msg,0,sizeof(MESSAGE_TYPE));
		memset(&pmsg,0,sizeof(MESSAGE_TYPE));
		if(semval == 0)
			sem_post(ref->my_sem);
		if (mq_receive(ref->input, (char*)&msg, sizeof(MESSAGE_TYPE), NULL) != sizeof(MESSAGE_TYPE)) {
			fprintf(stderr,"%s\n", "Bad Message Received\n");
			//sched_yield();
			continue;
		} else {
			AES_cbc_encrypt((unsigned char *)&msg, (unsigned char *)&pmsg, sizeof(MESSAGE_TYPE), &(ref->dkey), ref->security->auth_token, AES_DECRYPT);
			AES_e
			memset(print_buffer,0,sizeof(MESSAGE_TYPE)*2);
			strncat(print_buffer,pmsg.message,4064);
			if (memcmp((unsigned char*)pmsg.auth_token, (unsigned char*)ref->security->auth_token, TOKEN_LEN) == 0) {
				printf("Authorized Message:%s",print_buffer);
			} else {
				for (int j = 0; j < TOKEN_LEN; j++) {
					printf("%d::%d==%d\n", (int)pmsg.auth_token[j], (int)ref->security->auth_token[j], (int)pmsg.auth_token[j]-ref->security->auth_token[j]);
				}
				printf("\nMalicious Message:%s",print_buffer);
			}
		}
	}
	return (void*)257;
}

static int
monitor_input(TALK_TYPE *ref)
{

	if ((errno = pthread_create(&(ref->monitor), NULL, __t_monitor_input, ref)) != 0) {
		return -1;
	}
	return 0;
}

int
open_message_queues(TALK_TYPE *ref, const char *chat_user)
{

	acl_t p_them = 0;
	unsigned char *rb;

 	if (init_names(ref,chat_user) != 0)
		return -1;

	if (init_sync(ref) != 0)
		return -1;

	if (monitor_input(ref) != 0)
		return -1;


	return 0;
}
//
int
send_message(TALK_TYPE *ref, const char *message)
{
	MESSAGE_TYPE pmsg, emsg;
	volatile int sem_val = 0;
	if (strlen(message) > 4000) {
		errno = E2BIG;
		return -1;
	}

	if (ref->their_sem == NULL) {
		while (ref->their_sem == NULL) {
			if (ref->security->lower_user == getuid()) {
				ref->their_sem = sem_open(ref->chat_name__1, O_RDONLY);
			} else {
				ref->their_sem = sem_open(ref->chat_name__2, O_RDONLY);
			}
		}
	}

	sem_wait(ref->their_sem);

	while (ref->output == -257 || ref->output == -1) {
		if (ref->security->lower_user == getuid()) {
			ref->output = mq_open(ref->chat_name__1, O_WRONLY);
		} else {
			ref->output = mq_open(ref->chat_name__2, O_WRONLY);
		}
	}

	memcpy(pmsg.auth_token,ref->security->auth_token,TOKEN_LEN);
	strncpy(pmsg.message,message,4000);
	pmsg.message[4000] = '\0';

	/*if (AES_set_encrypt_key(ref->security->security_token,256,&(ref->ekey)) == -1)
		errExit("AES KEY SET\n");
	if (AES_set_decrypt_key(ref->security->security_token,256,&(ref->dkey)) == -1)
			errExit("AES KEY SET\n");*/

	AES_cbc_encrypt((unsigned char *)&pmsg, (unsigned char *)&emsg, sizeof(MESSAGE_TYPE), &(ref->ekey), ref->security->auth_token, AES_ENCRYPT);
	if (mq_send(ref->output, (char *)&emsg, sizeof(MESSAGE_TYPE), 1) == -1)
		errExit("mq_send\n");
}
