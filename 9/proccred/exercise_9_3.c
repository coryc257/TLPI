/*
 * exercise_9_3.c
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */


#include "../lib/9_hdr.h"
#include "8_hdr.h"
#include "tlpi_hdr.h"
#include <limits.h>

#define GROUP_CT_MAX (NGROUPS_MAX+1)

static int
__build_group_list(	const char *user, gid_t group_list[GROUP_CT_MAX],
					char *sup_group, gid_t sgid)
{
	struct group *cur_group;
	int group_pos = 0;
	char **cur_group_nam;

	while ((cur_group = getgrent()) != NULL && group_pos + 2 < GROUP_CT_MAX) {
		if (strcmp(cur_group->gr_name, sup_group) == 0)
			continue;
		// Don't duplicate groups
		cur_group_nam = cur_group->gr_mem;
		while (cur_group_nam != NULL && *cur_group_nam != NULL) {
			if (strcmp(*cur_group_nam, user) == 0) {
				group_list[group_pos] = cur_group->gr_gid;
				group_pos++;
				break;
			}
			cur_group_nam++;
		}
	}
	group_list[group_pos] = sgid;
	return group_pos + 1;
}

int
my_initgroups(const char *user, gid_t group)
{
	uid_t user_id = -1;
	gid_t group_list[GROUP_CT_MAX];
	gid_t *groups, *cg;
	int num_groups = 0;
	char *sup_group = NULL;

	user_id = userIdFromName(user);
	if(user_id == -1)
		errExit("Invalid Username\n");

	sup_group = groupNameFromid(group);
	if(sup_group == NULL)
		errExit("Invalid Group\n");

	setgrent();
	num_groups = __build_group_list(user, group_list, sup_group, group);
	endgrent();

	if(num_groups == 0)
		errExit("no Groups\n");

	groups = malloc(sizeof(gid_t)*num_groups);
	cg = groups;
	for(int i = 0; i < num_groups; i++)
	{
		*cg = group_list[i];
		cg++;
	}

	return setgroups(num_groups, groups);
}
