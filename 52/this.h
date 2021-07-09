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
#include <semaphore.h>

#define MAX_QUEUED 10
#define IV_SIZE 16
#define TOKEN_LEN 32
#define MESSAGE_SIZE 4096
#define MESSAGE_BUFFER MESSAGE_SIZE-TOKEN_LEN-IV_SIZE
#define MAX_TEXT_MESSAGE MESSAGE_SIZE-TOKEN_LEN-IV_SIZE-1
#define SENTINEL_INDEX MAX_TEXT_MESSAGE-1
#define MAX_CNAME (255*2)+9
#define ALLOC_CNAME malloc(sizeof(char)*(MAX_CNAME))


typedef struct security_file {
	int sync;
	pid_t lower_user;
	int lower_ready;
	unsigned char security_token[TOKEN_LEN];
	unsigned char auth_token[TOKEN_LEN];
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

	mqd_t input;
	mqd_t output;

	uid_t user1;
	uid_t user2;

	pthread_t monitor;

	SECURITY_FILE *security;
	AES_KEY ekey;
	AES_KEY dkey;
	sem_t *my_sem;
	sem_t *their_sem;
} THIS;

typedef struct __message_type {
	unsigned char auth_token[TOKEN_LEN];
	char message[MESSAGE_BUFFER];
} MESSAGE_TYPE;



typedef struct __talk__type__this TALK_TYPE;

int open_message_queues(TALK_TYPE *ref, const char *chat_user);

#endif /* THIS_H_ */
