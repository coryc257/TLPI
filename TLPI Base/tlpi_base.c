/*
 * tlpi_base.c
 *
 *  Created on: Jul 15, 2021
 *      Author: cory
 */


/*
 * ugid_functions.c
 *
 *  Created on: May 15, 2021
 *      Author: cory
 */

#define _DEFAULT_SOURCE

#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>

#include "lib/curr_time.h"          /* Declares function defined here */
#include "lib/tlpi_pwd.h"
#include "lib/get_num.h"
#include "lib/exec_container.h"
#include "lib/error_functions.h"
#include "lib/tlpi_hdr.h"
#include "lib/ename.c.inc"
#include "lib/daemonize.h"
#include "lib/tlpi_hdr.h"
#include "lib/read_line.h"
#include "lib/inet_sockets.h"
#include "lib/unix_sockets.h"
#include "lib/io_n.h"

char *		/* Return name corresponding to 'uid' or NULL on error */
userNameFromId(uid_t uid)
{
	struct passwd *pwd;
	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

uid_t		/* return UID corresponding to 'name' or -1 on error */
userIdFromName(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;

	if(name == NULL || *name == '\0')	/* On null or empty string 	*/
		return -1;						/* return an error			*/

	u = strtol(name, &endptr, 10);
	if(*endptr == '\0')					/* Return the UID if the user passes us "number" */
		return u;

	pwd = getpwnam(name);
	if(pwd == NULL)
		return -1;

	return pwd->pw_uid;
}

char *		/* return group name giving valid GID */
groupNameFromid(gid_t gid)
{
	struct group *grp;
	grp = getgrgid(gid);
	return (grp == NULL) ? NULL : grp->gr_name;
}

gid_t		/* Return GID corresponding to 'name', or -1 on error */
groupIdFromName(const char *name)
{
	struct group *grp;
	gid_t gid;
	char *end;

	gid = strtol(name,&end,10);
	if(*end == '\0')	/* Return gid if given a gid */
		return gid;

	grp = getgrnam(name);
	return (grp == NULL) ? -1 : grp->gr_gid;
}


/*
 * get_num.c
 *
 *  Created on: May 5, 2021
 *      Author: cory
 */




static void
gnFail(const char *fname, const char *msg, const char *arg, const char *name)
{
	fprintf(stderr, "%s error", fname);
	if(name != NULL)
		fprintf(stderr, " (int %s)", name);
	fprintf(stderr, ": %s\n", msg);
	if(arg != NULL && *arg != '\0')
		fprintf(stderr, "          offending text: %s\n", arg);

	exit(EXIT_FAILURE);
}

static long
getNum(const char *fname, const char *arg, int flags, const char *name)
{
	long res;
	char *endptr;
	int base;

	if(arg == NULL || *arg == '\0')
		gnFail(fname, "null or empty string", arg, name);

	base = (flags & GN_ANY_BASE) ? 0 :
			(flags & GN_BASE_8) ? 8 : (flags & GN_BASE_16) ? 16: 10;

	errno = 0;
	res = strtol(arg, &endptr, base);
	if (errno != 0)
		gnFail(fname, "strol() failed", arg, name);

	if (*endptr != '\0')
		gnFail(fname, "nonumeric characters", arg, name);

	if((flags & GN_NONNEG) && res < 0)
		gnFail(fname, "negative value not allowed", arg, name);

	if((flags & GN_GT_0) && res <= 0)
		gnFail(fname, "value must be > 0", arg, name);

	return res;
}

long
getLong(const char *arg, int flags, const char *name)
{
	return getNum("getLong", arg, flags, name);
}

int
getInt(const char *arg, int flags, const char *name)
{
	long res;

	res = getNum("getInt", arg, flags, name);

	if (res > INT_MAX || res < INT_MIN)
		gnFail("getInt", "integer out of range", arg, name);


	return (int)res;
}


/*
 * exec_container.c
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */


void
construct_args(int argc, char**args, EXEC_CONTAINER *ec)
{
	char **xargs, **cargs;
	ec->exe = strdup(args[0]);
	if (argc > 0) {
		ec->args = malloc(sizeof(char*)*(argc+1));
		xargs = ec->args;
		cargs = args;
		for (int j = 0; j < argc; j++) {
			*xargs = strdup(*cargs);
			xargs++;
			cargs++;
		}
		*cargs = NULL;
	} else {
		ec->args = NULL;
	}
}


/*
 * error_functions.c
 *
 *  Created on: May 5, 2021
 *      Author: cory
 */



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







/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* become_daemon.c

   A function encapsulating the steps in becoming a daemon.
*/

int                                     /* Returns 0 on success, -1 on error */
DAEMONIZE(int flags)
{
    int maxfd, fd;

    switch (fork()) {                   /* Become background process */
    case -1: return -1;
    case 0:  break;                     /* Child falls through... */
    default: _exit(EXIT_SUCCESS);       /* while parent terminates */
    }

    if (setsid() == -1)                 /* Become leader of new session */
        return -1;

    switch (fork()) {                   /* Ensure we are not session leader */
    case -1: return -1;
    case 0:  break;
    default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);                       /* Clear file mode creation mask */

    if (!(flags & BD_NO_CHDIR))
        chdir("/");                     /* Change to root directory */

    if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);            /* Reopen standard fd's to /dev/null */

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)         /* 'fd' should be 0 */
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}

/* curr_time.c

   Implement our currTime() function.
*/


#define CT_BUF_SIZE 1000

/* Return a string containing the current time formatted according to
   the specification in 'format' (see strftime(3) for specifiers).
   If 'format' is NULL, we use "%c" as a specifier (which gives the'
   date and time as for ctime(3), but without the trailing newline).
   Returns NULL on error. */

char *
currTime(const char *format)
{
    static char buf[CT_BUF_SIZE];  /* Nonreentrant */
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL)
        return NULL;

    s = strftime(buf, CT_BUF_SIZE, (format != NULL) ? format : "%c", tm);

    return (s == 0) ? NULL : buf;
}



/*
 * READ LINE
 */

ssize_t
readLine(int fd, void *buffer, size_t n)
{
	ssize_t numRead;
	size_t totRead;
	char *buf;
	char ch;

	if (n <= 0 || buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	buf = buffer;

	totRead = 0;
	for (;;) {
		numRead = read(fd, &ch, 1);

		if (numRead == -1) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		} else if (numRead == 0) { //EOF
			if (totRead == 0)
				return 0;
			else
				break;
		} else {
			if (totRead < n -1) {
				totRead++;
				*buf = ch;
				buf++;
			}
		}

		if (ch == '\n')
			break;
	}

	*buf = '\0';
	return totRead;
}


struct __BSR {
	int fd;
	size_t block_size;
	size_t buf_size;
	size_t hold_len;
	char *buf;
	char *hold;
	void *hold_allocate;
	int hold_lines;
};


BUFFERED_STREAM_READER
buffered_stream_reader_new(int fd, size_t block_size)
{
	struct __BSR *BSR;

	BSR = malloc(sizeof(struct __BSR));

	if (BSR == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	BSR->fd = dup(fd);
	//close(fd);
	BSR->buf_size = block_size;
	BSR->buf = malloc(sizeof(char)*BSR->buf_size);
	BSR->hold = NULL;
	BSR->hold_len = 0;
	BSR->hold_lines = 0;

	//strcat(BSR->buf, "The End Of The Line\nPOOP SHOOT");

	if (BSR->buf == NULL) {
		free(BSR);
		errno = ENOMEM;
		return NULL;
	}

	return (BUFFERED_STREAM_READER)BSR;
}


int
char_index(char *str, size_t len, char search)
{
	for (int j = 0; j < len; j++) {
		if (str[j] == search) {
			return j;
		}
	}

	return -1;
}

void
append_to_hold(BUFFERED_STREAM_READER stream, char *str, size_t len)
{
	struct __BSR *b;
	size_t local_offset;

	b = stream;

	local_offset = b->hold_len;
	if (b->hold == NULL) {
		b->hold = malloc(sizeof(char)* len);
	} else {
		b->hold = realloc(b->hold, sizeof(char) * (b->hold_len+len));
	}

	b->hold_len += len;

	memcpy(b->hold + local_offset, str, len);
}

char *
get_line(BUFFERED_STREAM_READER stream, size_t line_stop)
{
	char *line;
	struct __BSR *b;
	int total_required;
	b = stream;
	total_required = b->hold_len+line_stop+1;

	line = malloc(sizeof(char)*total_required);
	if (b->hold_len > 0)
		memcpy(line, b->hold, b->hold_len);
	memcpy(line+b->hold_len, b->buf, line_stop);

	line[total_required-1] = '\0';

	return line;
}

char*
yank_line(BUFFERED_STREAM_READER stream)
{
	struct __BSR *b;
	char *line, *swap;
	int nl;

	b = stream;
	nl = char_index(b->hold, b->hold_len, '\n');
	line = strndup(b->hold, nl + 1);

	b->hold_len = b->hold_len - nl - 1;

	swap = strndup(b->hold + nl + 1, b->hold_len);

	free(b->hold);
	b->hold = swap;
	b->hold_lines--;

	return line;
}

void
increment_line_count(BUFFERED_STREAM_READER stream)
{
	struct __BSR *b;
	b = stream;
	b->hold_lines++;
}

char *
buffered_stream_reader_read_line(BUFFERED_STREAM_READER stream)
{
	struct __BSR *b;
	ssize_t numRead;
	char *line;
	int offset, nl;
	b = (struct __BSR*)stream;

	line = NULL;
	offset = 0;

	if (b->hold_lines > 0)
		return yank_line(stream);

	for (;;) {
		numRead = read(b->fd, b->buf, b->buf_size);
		if (numRead == -1 && errno == EAGAIN) {
			continue;
		} else if (numRead == -1) {
			return NULL;
		} else if (numRead == 0) {
			errno = 0;
			return NULL;
		} else {
			offset = 0;
			while (offset < numRead) {
				nl = char_index(b->buf+offset, numRead-offset, '\n');

				if (nl == -1) {
					append_to_hold(stream, b->buf+offset, numRead-offset);
					break;
				}

				if (line == NULL) {
					line = get_line(stream, nl+1);
				} else {
					append_to_hold(stream, b->buf+offset, nl+1);
					increment_line_count(stream);
				}

				offset += nl + 1;
			}
		}
		if (line != NULL)
			return line;
	}
	return NULL;
}



/*
 * Socket Library
 */

int // Generally for a client
inetConnect(const char *host, const char *service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;

	s = getaddrinfo(host, service, &hints, &result);
	if (s != 0) {
		errno = ENOSYS;
		return -1;
	}

	/* Walk through returned list until we find an address structure
	 * that can be used to successfully connect as socket */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break; // Success

		// connect failed, close this socket and try the next

		close(sfd);
	}

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
}


static int
inetPassiveSocket(const char *service, int type, socklen_t *addrlen,
		int doListen, int backlog)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s, optval;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = type;
	hints.ai_family = AF_UNSPEC; // Allows IPV4 or IPV6
	hints.ai_flags = AI_PASSIVE; // Use wildcard IP address

	s = getaddrinfo(NULL, service, &hints, &result);
	if (s != 0) {
		return -1;
	}

	/* Walk through returned list until we find an address
	 * structure that can be used  to successfully create an bind a socket */
	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (doListen == 1) {
			if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
				close(sfd);
				freeaddrinfo(result);
				return -1;
			}
		}

		if (bind(sfd,rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		// bind failed, go to the next
		close(sfd);
	}

	if (rp != NULL && doListen == 1) {
		if (listen(sfd,backlog) == -1) {
			freeaddrinfo(result);
			return -1;
		}
	}

	if (rp != NULL && addrlen != NULL)
		*addrlen = rp->ai_addrlen;

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
}

int
inetListen(const char *service, int backlog, socklen_t *addrlen)
{
	return inetPassiveSocket(service, SOCK_STREAM, addrlen, 1, backlog);
}

int
inetBind(const char *service, int type, socklen_t *addrlen)
{
	return inetPassiveSocket(service, type, addrlen, 0, 0);
}

char *
inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
		char *addrStr, int addrStrLen)
{
	char host[NI_MAXHOST], service[NI_MAXSERV];
	if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) == 0)
		snprintf(addrStr, addrStrLen, "(%s, %s)", host, service);
	else
		snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");
	addrStr[addrStrLen - 1] = '\0'; /* Ensure result is null-terminated */
	return addrStr;

}

int
unixConnect(const char *name)
{
	int s;
	struct sockaddr_un unix_socket;

	if (strlen(name) > sizeof(unix_socket.sun_path)-1) {
		errno = E2BIG;
		return -1;
	}

	memset(&unix_socket, 0, sizeof(struct sockaddr_un));
	unix_socket.sun_family = AF_UNIX;
	strcpy(unix_socket.sun_path, name);


	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		errno = ENOSYS;
		return -1;
	}

	if (connect(s, SOCK_ADDR(&unix_socket), sizeof(unix_socket.sun_path)) != 0) {
		close(s);
		return -1;
	}

	return s;
}

static int
unixPassive(const char *name, int max_pending, int *umask_value, int do_listen)
{
	int s, pm;
	struct sockaddr_un unix_socket;

	if (strlen(name) > sizeof(unix_socket.sun_path)-1) {
		errno = E2BIG;
		return -1;
	}

	memset(&unix_socket, 0, sizeof(struct sockaddr_un));
	unix_socket.sun_family = AF_UNIX;
	strcpy(unix_socket.sun_path, name);

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		errno = ENOSYS;
		return -1;
	}

	if (umask_value != NULL) {
		pm = umask(*umask_value);
	}

	if (bind(s, SOCK_ADDR(&unix_socket), sizeof(struct sockaddr_un)) != 0) {
		close(s);
		return -1;
	}

	if (umask_value != NULL) {
		umask(pm);
	}

	if (listen(s,max_pending) == -1) {
		close(s);
		return -1;
	}

	return s;
}

int
unixListen(const char *name, int max_pending, int *umask_value)
{
	return unixPassive(name, max_pending, umask_value, 1);
}

int
unixBind(const char *name, int max_pending, int *umask_value)
{
	return unixPassive(name, max_pending, umask_value, 0);
}

/*
 * io_n.h
 */

ssize_t
readn(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total # of bytes read so far */
    char *buf;

    buf = buffer;                       /* No pointer arithmetic on "void *" */
    for (totRead = 0; totRead < n; ) {
        numRead = read(fd, buf, n - totRead);

        if (numRead == 0)               /* EOF */
            return totRead;             /* May be 0 if this is first read() */
        if (numRead == -1) {
            if (errno == EINTR)
                continue;               /* Interrupted --> restart read() */
            else
                return -1;              /* Some other error */
        }
        totRead += numRead;
        buf += numRead;
    }
    return totRead;                     /* Must be 'n' bytes if we get here */
}

ssize_t
writen(int fd, const void *buffer, size_t n)
{
    ssize_t numWritten;                 /* # of bytes written by last write() */
    size_t totWritten;                  /* Total # of bytes written so far */
    const char *buf;

    buf = buffer;                       /* No pointer arithmetic on "void *" */
    for (totWritten = 0; totWritten < n; ) {
        numWritten = write(fd, buf, n - totWritten);

        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;               /* Interrupted --> restart write() */
            else
                return -1;              /* Some other error */
        }
        totWritten += numWritten;
        buf += numWritten;
    }
    return totWritten;                  /* Must be 'n' bytes if we get here */
}

