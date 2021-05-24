/*
 * file.c
 *
 *  Created on: May 23, 2021
 *      Author: cory
 */

#include "../lib/file.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <stdio.h>

#define SET_FLAG(field, flag, attr) field = ((flag & attr) ? 1 : 0)
#define SAVE_FLAG(field, flag, attr) attr = ((field == 1) ? (attr | flag) : (attr & (~flag)))

void
get_file_info(char *file_name, FILEINFO **fi)
{
	int attr;
	FILEINFO *f;
	if(*fi == NULL)
	{
		*fi = malloc(sizeof(FILEINFO));

	}

	if(*fi == NULL)
		return;


	f = *fi;
	memset(f,0,sizeof(FILEINFO));
	f->__fd = open(file_name, O_RDONLY);

	if (f->__fd == -1)
		return;

	if (ioctl(f->__fd, FS_IOC_GETFLAGS, &attr) == -1)
		return;

	f->file_name = file_name;

	SET_FLAG(f->ck_a, FS_APPEND_FL, attr);
	SET_FLAG(f->ck_A, FS_NOATIME_FL, attr);
	f->ck_F = -1;
	//SET_FLAG(f->ck_F, 0, attr);
	SET_FLAG(f->ck_c, FS_COMPR_FL, attr);
	SET_FLAG(f->ck_D, FS_DIRSYNC_FL, attr);
	SET_FLAG(f->ck_d, FS_NODUMP_FL, attr);
	SET_FLAG(f->ck_e, FS_EXTENT_FL, attr);
	SET_FLAG(f->ck_i, FS_IMMUTABLE_FL, attr);
	SET_FLAG(f->ck_C, FS_NOCOW_FL, attr);
	SET_FLAG(f->ck_s, FS_SECRM_FL, attr);
	SET_FLAG(f->ck_u, FS_UNRM_FL, attr);
	SET_FLAG(f->ck_S, FS_SYNC_FL, attr);
	SET_FLAG(f->ck_t, FS_NOTAIL_FL, attr);
	SET_FLAG(f->ck_T, FS_TOPDIR_FL, attr);
	SET_FLAG(f->ck_P, FS_PROJINHERIT_FL, attr);
	SET_FLAG(f->ck_j, FS_JOURNAL_DATA_FL, attr);
}

int
save_file_attr(FILEINFO *fi)
{
	FILEINFO *f = fi;
	int attr;

	if (f->__fd == -1)
		return -1;

	if (ioctl(f->__fd, FS_IOC_GETFLAGS, &attr) == -1)
		return -1;

	SAVE_FLAG(f->ck_a, FS_APPEND_FL, attr);
	SAVE_FLAG(f->ck_A, FS_NOATIME_FL, attr);
	SAVE_FLAG(f->ck_c, FS_COMPR_FL, attr);
	SAVE_FLAG(f->ck_D, FS_DIRSYNC_FL, attr);
	SAVE_FLAG(f->ck_d, FS_NODUMP_FL, attr);
	SAVE_FLAG(f->ck_e, FS_EXTENT_FL, attr);
	SAVE_FLAG(f->ck_i, FS_IMMUTABLE_FL, attr);
	SAVE_FLAG(f->ck_C, FS_NOCOW_FL, attr);
	SAVE_FLAG(f->ck_s, FS_SECRM_FL, attr);
	SAVE_FLAG(f->ck_u, FS_UNRM_FL, attr);
	SAVE_FLAG(f->ck_S, FS_SYNC_FL, attr);
	SAVE_FLAG(f->ck_t, FS_NOTAIL_FL, attr);
	SAVE_FLAG(f->ck_T, FS_TOPDIR_FL, attr);
	SAVE_FLAG(f->ck_P, FS_PROJINHERIT_FL, attr);
	SAVE_FLAG(f->ck_j, FS_JOURNAL_DATA_FL, attr);

	if (ioctl(f->__fd, FS_IOC_SETFLAGS, &attr) == -1)
	{
		errExit("FS_IOC_SETFLAGS\n");
		return -1;
	}

	close(f->__fd);
	return 0;
}
