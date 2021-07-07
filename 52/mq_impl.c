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


	snprintf(ref->chat_name__1, MAX_CNAME, "/talkt_%d_%d", them, me);
	snprintf(ref->chat_name__2, MAX_CNAME, "/talkt_%d_%d", me, them);
	snprintf(ref->security_file, PATH_MAX+1, "/tmp/talkt_%d_%d", lower, upper);
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
	struct mq_attr ma, *map;
	unsigned char buf[4096];
	unsigned char buf2[4096];
	map = &ma;

	fd = open(ref->security_file, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1 && errno == EEXIST) {
		fd = open(ref->security_file, O_RDWR, S_IRUSR | S_IWUSR);
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
		RAND_bytes(ref->security->security_token,32);

		f_acl = acl_get_file(ref->security_file, ACL_TYPE_ACCESS);

		create_mask(ref, f_acl, ACL_MASK);
		create_mask(ref, f_acl, ACL_USER);
		if (acl_set_file(ref->security_file,ACL_TYPE_ACCESS,f_acl) == -1) {
			unlink(ref->security_file);
			errExit("acl_set_fd");
		}

		mq_unlink(ref->chat_name__1);
		mq_unlink(ref->chat_name__2);
		prev = umask(0);
		ma.mq_maxmsg = 10;
		ma.mq_msgsize = 4096;
		ref->chat_1 = mq_open(ref->chat_name__1,O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);
		ref->chat_2 = mq_open(ref->chat_name__2,O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP, &ma);
		umask(prev);
	}

	// Set Up Encryption
	if (AES_set_encrypt_key(ref->security->security_token,256,&(ref->ekey)) == -1)
		errExit("AES KEY SET\n");
	if (AES_set_decrypt_key(ref->security->security_token,256,&(ref->dkey)) == -1)
			errExit("AES KEY SET\n");

	/*strcpy(buf,"Hello\n");

	AES_encrypt(buf,buf2,&(ref->ekey));
	memset(buf,0,4096);
	AES_decrypt(buf2,buf,&(ref->dkey));

	printf("%s\n", buf);*/


}

int open_message_queues(TALK_TYPE *ref, const char *chat_user)
{

	acl_t p_them = 0;
	unsigned char *rb;

	if (init_names(ref,chat_user) != 0)
		return -1;

	if (init_sync(ref) != 0)
		return -1;


	return 0;
}
