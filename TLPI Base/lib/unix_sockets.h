/*
 * unix_sockets.h
 *
 *  Created on: Jul 18, 2021
 *      Author: cory
 */

#ifndef LIB_UNIX_SOCKETS_H_
#define LIB_UNIX_SOCKETS_H_

int unixConnect(const char *name);
int unixListen(const char *name, int max_pending, int *umask_value);
int unixBind(const char *name, int max_pending, int *umask_value);

#endif /* LIB_UNIX_SOCKETS_H_ */
