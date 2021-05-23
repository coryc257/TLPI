/*
 * effective_access.c
 *
 *  Created on: May 23, 2021
 *      Author: cory
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define CHECK_MODE(aflags,mode) (aflags & mode)
#define NO_MATCH(u,g,fu,fg,fm,pu,pg,po) (!(u==fu && pu & fm) && \
										!(g == fg && pg & fm) && \
										!(fm & po))

int
effective_access (char *file, int aflags)
{
	struct stat stat_buf;
	uid_t r_uid, e_uid, s_uid;
	gid_t r_gid, e_gid, s_gid;
	int file_mode;
	uid_t file_userid;
	gid_t file_gropid;

	if (aflags & R_OK)

	if(getresuid(&r_uid,&e_uid,&s_uid) == -1)
		return -1;

	if(getresgid(&r_gid,&e_gid,&s_gid) == -1)
		return -1;

	if(stat(file,&stat_buf) == -1)
	{
		if (CHECK_MODE(aflags, F_OK))
		{
			errno = ENOENT;
		}
		return -1;
	}

	file_mode = stat_buf.st_mode;
	errno = 0;

	file_userid = stat_buf.st_uid;
	file_gropid = stat_buf.st_gid;



	if (CHECK_MODE(aflags,R_OK) && (NO_MATCH(
									e_uid, e_gid,
									file_userid, file_gropid,
									file_mode, S_IRUSR, S_IRGRP, S_IROTH)))
	{
		errno = EACCES;
		return -1;
	}
	else if (CHECK_MODE(aflags,W_OK) && (NO_MATCH(
									e_uid, e_gid,
									file_userid, file_gropid,
									file_mode, S_IWUSR, S_IWGRP, S_IWOTH)))
	{
		errno = EACCES;
		return -1;
	}
	else if (CHECK_MODE(aflags,X_OK) && (NO_MATCH(
										e_uid, e_gid,
										file_userid, file_gropid,
										file_mode, S_IXUSR, S_IXGRP, S_IXOTH)))
	{
		errno = EACCES;
		return -1;
	}





	return 0;
}
