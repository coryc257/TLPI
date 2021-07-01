/*
 * daemonize.h
 *
 *  Created on: Jul 1, 2021
 *      Author: cory
 */

#ifndef LIB_DAEMONIZE_H_
#define LIB_DAEMONIZE_H_

#define BD_NO_CHDIR 01
#define BD_NO_CLOSE_FILES 02
#define BD_NO_REOPEN_STD_FDS 04
#define BD_NO_UMASK0 010
#define BD_MAX_CLOSE 819

int DAEMONIZE(int flags);


#endif /* LIB_DAEMONIZE_H_ */
