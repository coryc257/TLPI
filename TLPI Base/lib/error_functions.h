/*
 * error_functions.h
 *
 *  Created on: May 5, 2021
 *      Author: cory
 */

#ifndef ERROR_FUNCTIONS_H_
#define ERROR_FUNCTIONS_H_

void errMsg(const char *format, ...);

#ifdef __GNUC__
	/* This macro stops gcc -Wall from spitting out an error when nothing returns from a function stuch as:
	 * 	int main(void)
	 * 	{
	 * 		errExit(...);
	 * 	}
	 */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

#include "tlpi_hdr.h"

void errExit(const char *format, ...) NORETURN;
void err_exit(const char *format, ...) NORETURN;
void errExitEN(int errnum, const char *format, ...) NORETURN;
void fatal(const char *format, ...) NORETURN;
void usageErr(const char *format, ...) NORETURN;
void cmdLineErr(const char *format, ...) NORETURN;



#endif /* ERROR_FUNCTIONS_H_ */
