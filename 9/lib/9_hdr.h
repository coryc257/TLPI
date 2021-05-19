/*
 * 9_hdr.h
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */

#ifndef LIB_9_HDR_H_
#define LIB_9_HDR_H_

#define _GNU_SOURCE
#define _BSD_SOURCE  /* Get getPass() declaration from <unistd.h> */
#define _XOPEN_SOURCE /* Get crypt() declaration from <unistd.h> */

#include <sys/types.h>
#include <grp.h>
#include <unistd.h>

void __id_show();

int my_initgroups(const char *user, gid_t group);

#endif /* LIB_9_HDR_H_ */
