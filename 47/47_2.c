/*
 * 47_2.c
 *
 *  Created on: Jul 2, 2021
 *      Author: cory
 */




/* fork_sig_sync.c

   Demonstrate how signals can be used to synchronize the actions
   of a parent and child process.
*/
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "curr_time.h"                  /* Declaration of currTime() */
#include "tlpi_hdr.h"

#define SYNC_SIG SIGUSR1                /* Synchronization signal */

static key_t semaphore;

static void             /* Signal handler - does nothing but return */
handler(int sig)
{
}

static void
exit_handler()
{

}

int
x47_2__main(int argc, char *argv[])
{
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;
    struct sembuf sop[1];

    setbuf(stdout, NULL);               /* Disable buffering of stdout */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);    /* Block signal */
    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
        errExit("sigprocmask");

    atexit(exit_handler);

    semaphore = semget(IPC_PRIVATE,1, S_IRUSR | S_IWUSR);

    if (semaphore == -1)
    	errExit("semget\n");
    sop[0].sem_num = 0;
	sop[0].sem_flg = 0;
	sop[0].sem_op = 0;
	semop(semaphore,sop,1);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1)
        errExit("sigaction");

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0: /* Child */

        /* Child does some required action here... */

        printf("[%s %ld] Child started - doing some work\n",
                currTime("%T"), (long) getpid());
        sleep(2);               /* Simulate time spent doing some work */

        /* And then signals parent that it's done */

        printf("[%s %ld] Child about to signal parent\n",
                currTime("%T"), (long) getpid());
        /*if (kill(getppid(), SYNC_SIG) == -1)
            errExit("kill");*/
        sop[0].sem_num = 0;
        sop[0].sem_flg = 0;
        sop[0].sem_op = 1;
        semop(semaphore,sop,1);

        /* Now child can do other things... */

        _exit(EXIT_SUCCESS);

    default: /* Parent */

        /* Parent may do some work here, and then waits for child to
           complete the required action */

        printf("[%s %ld] Parent about to wait for signal\n",
                currTime("%T"), (long) getpid());

        sop[0].sem_num = 0;
        sop[0].sem_flg = 0;
        sop[0].sem_op = -1;
        if (semop(semaphore,sop,1) == -1)
        	errExit("semop\n");

        /*sigemptyset(&emptyMask);
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");*/
        printf("[%s %ld] Parent got signal\n", currTime("%T"), (long) getpid());

        /* If required, return signal mask to its original state */

        if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
            errExit("sigprocmask");

        /* Parent carries on to do other things... */

        exit(EXIT_SUCCESS);
    }
}

