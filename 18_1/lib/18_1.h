/*
 * 18_1.h
 *
 *  Created on: May 26, 2021
 *      Author: cory
 */

#ifndef X18_1_H_
#define X18_1_H_
#include <sys/acl.h>
#include <acl/libacl.h>

typedef struct ret_acl {
	int OK;
	uid_t user_or_group;
	acl_perm_t permission;
	acl_perm_t mask;
} RET_ACL;

#define GRET_ACL(item) (RET_ACL*)item
RET_ACL *get_user_acl(uid_t uid, char *file);

#endif /* 18_1_H_ */
