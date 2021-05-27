/*
 * acl.c
 *
 *  Created on: May 26, 2021
 *      Author: cory
 */

#include <sys/types.h>
#include <sys/acl.h>
#include <acl/libacl.h>
#include <pwd.h>

#include <unistd.h>
#include <stdlib.h>
#include "../../lib/18_1.h"

#define ORPERM(perm_set,permission,perm,ret) {switch(acl_get_perm(perm_set, permission)){ \
	case 0: \
		break; \
	case 1: \
		perm |= permission; \
		break; \
	case -1: \
		ret->OK = -10;\
		return; \
	}}

static void
process_entry(RET_ACL *ret, acl_t acl, acl_entry_t entry)
{
	acl_tag_t tag_type;
	acl_permset_t perm_set;
	acl_perm_t perm = 0;
	void *quail;

	if(acl_get_tag_type(entry,&tag_type) == -1)
	{
		ret->OK = -3;
		return;
	}

	if (tag_type == ACL_MASK)
			goto is_mask;

	if ((quail = acl_get_qualifier(entry)) == NULL && tag_type != ACL_MASK)
	{
		if (tag_type & (ACL_MASK|ACL_USER_OBJ|ACL_GROUP_OBJ|ACL_OTHER))
			return;
		ret->OK = -4;
		return;
	}

	if((*((uid_t*)quail)) != ret->user_or_group)
		return;

	is_mask:
	if(acl_get_permset(entry,&perm_set) == -1)
	{
		ret->OK = -5;
		return;
	}

	ORPERM(perm_set,ACL_READ,perm,ret)
	ORPERM(perm_set,ACL_WRITE,perm,ret)
	ORPERM(perm_set,ACL_EXECUTE,perm,ret)
	if (tag_type == ACL_MASK)
		ret->mask = perm;
	else
		ret->permission = perm;
}

RET_ACL
*get_user_acl(uid_t uid, char *file)
{
	RET_ACL *ret;
	acl_t acl;
	acl_entry_t entry;
	int entry_id = ACL_FIRST_ENTRY;

	if((ret = GRET_ACL(malloc(sizeof(RET_ACL)))) == NULL)
		return NULL;

	ret->user_or_group = uid;

	if ((acl = acl_get_file(file, ACL_TYPE_ACCESS)) == NULL)
	{
		ret->OK = 0;
		return ret;
	}

	get_user_acl_next:
	switch (acl_get_entry(acl,entry_id,&entry)) {
	case 0:
		return ret;
		break;
	case 1:
		entry_id = ACL_NEXT_ENTRY;
		process_entry(ret,acl,entry);
		//if (ret->OK != 0)
			//return ret;
		goto get_user_acl_next;
		break;
	case -1:
		ret->OK = -1;
		return ret;
	default:
		ret->OK = -2;
		return ret;
		break;
	}
	return ret; // Should not get here
}
