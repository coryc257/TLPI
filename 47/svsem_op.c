/*
 * svsem_op.c
 *
 *  Created on: Jul 2, 2021
 *      Author: cory
 */


#include <sys/types.h>
#include <sys/sem.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include "curr_time.h"			/* Declaration of currTime() */
#include "tlpi_hdr.h"

#define MAX_SEMOPS 1000			/* Maximum operations that we permit for a single semop() */

static void
usageError(const char *progName)
{
	fprintf(stderr, "Usage: %s semid op[,op...] ...\n\n", progName);
	fprintf(stderr, "'op' is either: <sem#>{+|-}<value>[n][u]\n");
	fprintf(stderr, "            or: <sem#>=0[n]\n");
	fprintf(stderr, "       \"n\" means include IPC_NOWAIT in 'op'\n");
	fprintf(stderr, "       \"u\" means include SEM_UNDO in 'op'\n\n");
	fprintf(stderr, "The operations in each argument are"
			        " performed in a single semop() call\n\n");
	fprintf(stderr, "e.g: %s 12345 0+1,1-2un\n", progName);
	fprintf(stderr, "     %s 12345 0=0n 1+1,2-1u 1=0\n", progName);
	exit(EXIT_FAILURE);
}

static int
parseOps(char *arg, struct sembuf sops[])
{
	char *comma, *sign, *remaining, *flags;
	int numOps;

	for (numOps = 0, remaining = arg; ; numOps++) {
		if (numOps > MAX_SEMOPS)
			cmdLineErr("Too many operations (maximum=%d): \"%s\"\n", MAX_SEMOPS, arg);

		if (*remaining == '\0')
			fatal("Trailing comma or empty argument: \"%s\"", arg);
		if(!isdigit((unsigned char)*remaining))
			cmdLineErr("Expected initial digit: \"%s\"\n", arg);

		sops[numOps].sem_num = strtol(remaining, &sign, 10);

		if (*sign == '\0' || strchr("+-=", *sign) == NULL)
			cmdLineErr("Expected '+', '-' or '=' int \"%s\"\n", arg);
		if (!isdigit((unsigned char)*(sign + 1)))
			cmdLineErr("Expected digit after '%c' in \"%s\"\n", *sign, arg);

		sops[numOps].sem_op = strtol(sign + 1, &flags, 10);

		if (*sign == '-')
			sops[numOps].sem_op = - sops[numOps].sem_op;
		else if (*sign == '=')
			if (sops[numOps].sem_op != 0)
				cmdLineErr("Expected \"=0\" in \"%s\"\n", arg);

		sops[numOps].sem_flg = 0;
		for(;; flags++) {
			if(*flags == 'n')
				sops[numOps].sem_flg |= IPC_NOWAIT;
			else if (*flags == 'u')
				sops[numOps].sem_flg |= SEM_UNDO;
			else
				break;
		}

		if (*flags != ',' && *flags != '\0')
			cmdLineErr("Bad trailing character (%c) in \"%s\"\n", *flags, arg);

		comma = strchr(remaining, ',');
		if (comma == NULL)
			break;
		else
			remaining = comma + 1;
	}
	return numOps + 1;
}

int
svsem_op(int argc, char *argv[])
{


	struct sembuf sops[MAX_SEMOPS];
	int ind, nsops;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageError(argv[0]);

	for (ind = 2; argv[ind] != NULL; ind++) {
		nsops = parseOps(argv[ind], sops);

		printf("%5ld, %s: about to semop() [%s]\n", (long)getpid(), currTime("%T"), argv[ind]);

		if (semop(getInt(argv[1],0, "semid"),sops,nsops) == -1)
			errExit("semop (PID=%ld)\n", (long)getpid());

		printf("%5ld, %s: semop() completed [%s]\n", (long) getpid(), currTime("%T"), argv[ind]);
	}

	exit(EXIT_SUCCESS);
}
