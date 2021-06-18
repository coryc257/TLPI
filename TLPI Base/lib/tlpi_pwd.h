/*
 * 8_hdr.h
 *
 *  Created on: May 15, 2021
 *      Author: cory
 */

#ifndef X8_HDR_H
#define X8_HDR_H
#include <pwd.h>
#include <unistd.h>

char *userNameFromId(uid_t uid);
uid_t userIdFromName(const char *name);
char *groupNameFromid(gid_t gid);
gid_t groupIdFromName(const char *name);
struct passwd *getpwname(const char *name);

#endif /* 8_HDR_H_ */
