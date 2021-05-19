/*
 * error_functions.c
 *
 *  Created on: May 5, 2021
 *      Author: cory
 */

#include <stdarg.h>
#include "lib/error_functions.h"
#include "lib/tlpi_hdr.h"
#include "lib/ename.c.inc"

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif


/*
 * Quit Program and...
	 * Dump core if EF_DUMPCORE environment variable is defined and
	 * is a nonempty string; otherwise call exit(3) or _exit(2),
	 * depending on the value of useExit3.
	 */

static void
terminate(Boolean useExit3)
{
	char *s;

	s = getenv("EF_DUMPCORE");

	if(s != NULL && *s != '\0')
		abort();
	else if (useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
}

static void
outputError(Boolean useErr, int err, Boolean flushStdout,
		const char *format, va_list ap)
{
#define BUF_SIZE 500
#define NUM_BUFF 2
#define STUPID_BYTE 1
#define BIG_BUFF ((BUF_SIZE+(STUPID_BYTE*2))*NUM_BUFF+STUPID_BYTE)
	char buf[BIG_BUFF], userMsg[BUF_SIZE], errText[BUF_SIZE];
	vsnprintf(userMsg, BUF_SIZE, format, ap);

	if(useErr)
		snprintf(errText, BUF_SIZE, " [%s %s]",
				(err > 0 && err <= MAX_ENAME) ?
				ename[err] : "?UNKNOWN?", strerror(err));
	else
		snprintf(errText, BUF_SIZE, ":");

	snprintf(buf, BIG_BUFF, "ERROR%s %s", errText, userMsg);

	if(flushStdout)
		fflush(stdout);
	fputs(buf,stderr);
	fflush(stderr);
}

void
errMsg(const char *format, ...)
{
	va_list argList;
	int savedErrorno;

	savedErrorno = errno; /* In case we change it here */

	va_start(argList, format);
	outputError(TRUE, errno, TRUE, format, argList);
	va_end(argList);

	errno = savedErrorno;
}

void
errExit(const char *format, ...)
{
	va_list argList;

	va_start(argList, format);
	outputError(TRUE, errno, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void
err_exit(const char *format, ...)
{
	va_list argList;

	va_start(argList, format);
	outputError(TRUE, errno, FALSE, format, argList);
	va_end(argList);

	terminate(FALSE);
}

void
errExitEN(int errnum, const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	outputError(TRUE, errnum, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void
fatal(const char* format, ...)
{
	va_list argList;

	va_start(argList, format);
	outputError(FALSE, 0, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void
usageErr(const char *format, ...)
{
	va_list argList;

	fflush(stdout);

	fprintf(stderr, "Usage: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
	exit(EXIT_FAILURE);

}

void
cmdLineErr(const char* format, ...)
{
	va_list argList;

	fflush(stdout);
	fprintf(stderr, "Command-line usage error: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
	exit(EXIT_FAILURE);
}







