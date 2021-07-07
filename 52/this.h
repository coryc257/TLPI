/*
 * this.h
 *
 *  Created on: Jul 6, 2021
 *      Author: cory
 */

#ifndef THIS_H_
#define THIS_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <openssl/rand.h>
#include <openssl/aes.h>

#define TOKEN_LEN 32

typedef struct security_file {
	int sync;
	pid_t lower_user;
	int lower_ready;
	unsigned char security_token[TOKEN_LEN];
	int upper_ready;
	pid_t upper_user;
} SECURITY_FILE;

typedef struct __talk__type__this {
	char *chat_user;
	char *chat_name__1;
	char *chat_name__2;
	char *security_file;

	mqd_t chat_1;
	mqd_t chat_2;

	uid_t user1;
	uid_t user2;

	SECURITY_FILE *security;
	AES_KEY ekey;
	AES_KEY dkey;
} THIS;



typedef struct __talk__type__this TALK_TYPE;

int open_message_queues(TALK_TYPE *ref, const char *chat_user);

#endif /* THIS_H_ */
