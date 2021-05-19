/*
 * idshow.c
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */

#include "../lib/9_hdr.h"
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include "8_hdr.h"
#include "tlpi_hdr.h"

#define SG_SIZE (NGROUPS_MAX + 1)

#define DISPLAY_USERNAME(user) user == NULL ? "???" : user
#define DISPLAY_GROUPNAME(group) DISPLAY_USERNAME(group)

void
__id_show()
{
	uid_t ruid, euid, suid, fsuid;
	gid_t rgid, egid, sgid, fsgid;
	gid_t suppGroups[SG_SIZE];
	int numGroups, j;
	char *p;

	if(getresuid(&ruid, &euid, &suid) == -1)
		errExit("getresuid");
	if(getresgid(&rgid, &egid, &sgid) == -1)
		errExit("getresgid");

	/* Attemps to thcnage the file-system IDs are always ignored
	 * for unpriviledged processes, but even so, the following
	 * calls return the current file-system IDs
	 */

	fsuid = setfsuid(0);
	fsgid = setfsgid(0);

	printf("UID: ");
	p = userNameFromId(ruid);
	printf("real=%s (%ld); ", DISPLAY_USERNAME(p), (long)ruid);
	p = userNameFromId(euid);
	printf("effective=%s (%ld); ", DISPLAY_USERNAME(p), (long)ruid);
	p = userNameFromId(suid);
	printf("stored=%s (%ld); ", DISPLAY_USERNAME(p), (long)suid);
	p = userNameFromId(fsuid);
	printf("fs=%s (%ld); ", DISPLAY_USERNAME(p), (long)suid);
	printf("\n");

	printf("GID: ");
	p = groupNameFromid(rgid);
	printf("real=%s (%ld); ", DISPLAY_GROUPNAME(p), (long)rgid);
	p = groupNameFromid(egid);
	printf("effective=%s (%ld); ", DISPLAY_GROUPNAME(p), (long)egid);
	p = groupNameFromid(sgid);
	printf("stored=%s (%ld)", DISPLAY_GROUPNAME(p), (long)sgid);
	p = groupNameFromid(fsgid);
	printf("fs=%s (%ld); ", DISPLAY_GROUPNAME(p), (long)fsgid);
	printf("\n");

	numGroups = getgroups(SG_SIZE, suppGroups);
	if(numGroups == -1)
		errExit("getgroups");

	printf("Supplementary Groups(%d): ", numGroups);
	for(j = 0; j < numGroups; j++)
	{
		p = groupNameFromid(suppGroups[j]);
		printf("%s (%ld) ", DISPLAY_GROUPNAME(p), (long)suppGroups[j]);
	}
	printf("\n");

	//exit(EXIT_SUCCESS);

}
