/*
 * file.h
 *
 *  Created on: May 23, 2021
 *      Author: cory
 */

#ifndef FILE_H_
#define FILE_H_

#include <unistd.h>

typedef struct fileinfo {
		char *directory;
		char *file_name;
		int ck_a;
		int ck_A;
		int ck_F;
		int ck_c;
		int ck_D;
		int ck_d;
		int ck_e;
		int ck_i;
		int ck_C;
		int ck_s;
		int ck_u;
		int ck_S;
		int ck_t;
		int ck_T;
		int ck_P;
		int ck_j;
		int __fd;
} FILEINFO;

void get_file_info(char *file_name, FILEINFO **fi);
int save_file_attr(FILEINFO *fi);

#endif /* FILE_H_ */
