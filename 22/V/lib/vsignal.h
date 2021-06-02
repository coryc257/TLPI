/*
 * vsignal.h
 *
 *  Created on: Jun 1, 2021
 *      Author: cory
 */

#ifndef LIB_VSIGNAL_H_
#define LIB_VSIGNAL_H_

#define _XOPEN_SOURCE 500
#include <signal.h>

void (*sigset(int sig, void (*handler)(int)))(int);
int sighold(int sig);
int sigrelse(int sig);
int sigignore(int sig);
int sigpause(int sig);

#endif /* LIB_VSIGNAL_H_ */
