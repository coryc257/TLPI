/*
 * random.c
 *
 *  Created on: May 22, 2021
 *      Author: cory
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "tlpi_hdr.h"
#include "../lib/random.h"

typedef struct x{
	int initialized;
	int rnd[];
} RND_USE;

static RND_USE *rnd = NULL;

int
GENERATE_RANDOM()
{
	#define RND_SIZE 1000000
	#define MODULO 999999
	if (rnd == NULL)
	{
		rnd = malloc(offsetof(RND_USE,rnd) + sizeof(rnd->rnd[0])*RND_SIZE + 1);
		memset(&(rnd->rnd[0]),0,RND_SIZE);
	}
	if (rnd == NULL)
		errExit("malloc\n");

	while(1)
	{
		rnd->initialized = random()%MODULO;
		if (rnd->rnd[rnd->initialized] == 0)
		{
			rnd->rnd[rnd->initialized] = 1;
			break;
		}
	}

	return rnd->initialized;
}
