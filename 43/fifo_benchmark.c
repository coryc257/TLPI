/*
 * fifo_benchmark.c
 *
 *  Created on: Jun 26, 2021
 *      Author: cory
 */



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include "tlpi_hdr.h"
#include <errno.h>

#define FIFO_SERVER_NAME "/home/cory/43_fifo"

struct local_global {
	int fifo_message_length;
	int fifo_message_count;
	pid_t parent;
	pid_t child;
};

struct local_global this;

static sig_atomic_t EOF_REACHED = 0;
static void
child_handler(int sig, siginfo_t *info, void *ucontext)
{
	printf("Ding\n");

	if (sig == SIGUSR1) {
		if (info->si_int == SIGKILL)
			EOF_REACHED = 1;
	}
}

static void
try_open_fifo()
{
	int attempts = 0;
	open_fifo: remove(FIFO_SERVER_NAME);
	if (mkfifo(FIFO_SERVER_NAME, S_IRUSR | S_IWUSR) == 0)
		goto fifo_open;
	else {
		attempts++;
		if (attempts >= 5)
			errExit("Cannot mkfifo\n");

		goto open_fifo;
	}
	fifo_open: printf("FIFO CREATED\n");
}

static void
receive_data()
{
	sigset_t xopen;
	char buf[this.fifo_message_length];
	int hold, reader, numRead, totalRead;
	sigemptyset(&xopen);
	hold = open(FIFO_SERVER_NAME, O_RDONLY);
	reader = open(FIFO_SERVER_NAME, O_RDONLY | O_NONBLOCK);
	totalRead = 0;
	sigprocmask(SIG_SETMASK, &open, NULL);
	for (;;) {
		numRead = read(reader,buf,this.fifo_message_length);
		//printf("%d\n", numRead);
		if (errno == EAGAIN) {
			errno = 0;
			continue;
		}

		if (numRead == 0 && EOF_REACHED)
			break;

		if (numRead == 0)
			continue;

		if (numRead == -1 || errno != 0 || numRead != this.fifo_message_length)
			errExit("Bad Read\n");

		totalRead++;
		//printf("Got Data(%d)\n", totalRead);
	}
	printf("Total Data Reads:%d\n", totalRead);
}

static void
send_data()
{
	char buf[this.fifo_message_length];
	int writer = open(FIFO_SERVER_NAME, O_WRONLY);
	for (int j = 0; j < this.fifo_message_count; j++) {
		write(writer,buf,this.fifo_message_length);
	}
	close(writer);
}

static void
kill_child()
{
	union sigval x;
	x.sival_int = SIGKILL;
	sigqueue(this.child, SIGUSR1, x);
}

static void
__init()
{
	try_open_fifo();
	this.parent = getpid();
	sigset_t block;
	struct sigaction act;
	sigfillset(&block);
	sigprocmask(SIG_SETMASK, &block, NULL);
	act.sa_flags = SA_SIGINFO | SA_RESTART;
	act.sa_sigaction = child_handler;
	sigfillset(&act.sa_mask);
	sigaction(SIGUSR1, &act, NULL);


	switch(this.child = fork()) {
	case -1:
		errExit("fork\n");
		break;
	case 0:
		receive_data();
		break;
	default:
	{
		struct timespec start, finish;
		int total_seconds, swap, total_nanoseconds;
		clock_gettime(CLOCK_REALTIME,&start);
		send_data();
		kill_child();
		waitpid(this.child,NULL,0);
		clock_gettime(CLOCK_REALTIME,&finish);
		total_seconds = finish.tv_sec-start.tv_sec;
		swap = finish.tv_nsec-start.tv_nsec;
		if (swap < 0)
		{
			total_seconds--;
			total_nanoseconds = 1000000000+swap;
		}
		else
			total_nanoseconds = swap;
		printf("%d.%d => %d.%d\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
		printf("%d.%d\n", total_seconds, total_nanoseconds);
	}
	break;
	}
}

void
fifo_benchmark(int argc, char *argv[])
{
	if (argc != 3 || (argc == 2 && strcmp(argv[1],"--help") == 0 )) {
		usageErr("%s fifo-message-length fifo-message-count", argv[0]);
	}

	this.fifo_message_length = getInt(argv[1], GN_GT_0, "fifo-message-length");
	this.fifo_message_count = getInt(argv[2], GN_GT_0, "fifo-message-count");

	__init();
}

