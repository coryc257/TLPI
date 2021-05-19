/*
 * ugid_functions.c
 *
 *  Created on: May 15, 2021
 *      Author: cory
 */


#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <unistd.h>
#include "../lib/8_hdr.h"

char *		/* Return name corresponding to 'uid' or NULL on error */
userNameFromId(uid_t uid)
{
	struct passwd *pwd;
	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

uid_t		/* return UID corresponding to 'name' or -1 on error */
userIdFromName(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;

	if(name == NULL || *name == '\0')	/* On null or empty string 	*/
		return -1;						/* return an error			*/

	u = strtol(name, &endptr, 10);
	if(*endptr == '\0')					/* Return the UID if the user passes us "number" */
		return u;

	pwd = getpwnam(name);
	if(pwd == NULL)
		return -1;

	return pwd->pw_uid;
}

char *		/* return group name giving valid GID */
groupNameFromid(gid_t gid)
{
	struct group *grp;
	grp = getgrgid(gid);
	return (grp == NULL) ? NULL : grp->gr_name;
}

gid_t		/* Return GID corresponding to 'name', or -1 on error */
groupIdFromName(const char *name)
{
	struct group *grp;
	gid_t gid;
	char *end;

	gid = strtol(name,&end,10);
	if(*end == '\0')	/* Return gid if given a gid */
		return gid;

	grp = getgrnam(name);
	return (grp == NULL) ? -1 : grp->gr_gid;
}
