/*
 * thread_incr.c
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */


#include <pthread.h>
#include "tlpi_hdr.h"

static int glob = 0;
struct t_param {
	int loops;
	int id;
};

static void *
threadFunc(void *arg)
{
	struct t_param *param = (struct t_param*)arg;
	int loc, j;

	for (j = 0; j < param->loops; j++) {
		loc = glob;
		loc++;
		glob = loc;
		printf("Iteration From %d::%d->%d\n", param->id, j+1, glob);
	}

	return NULL;
}

int
thread_incr(int argc, char *argv[])
{
	pthread_t t1, t2;
	struct t_param tp1, tp2;
	int loops, s;
	setbuf(stdout, NULL);

	loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

	tp1.id = 1;
	tp2.id = 2;
	tp2.loops = tp1.loops = loops;


	s = pthread_create(&t1, NULL, threadFunc, &tp1);
	if (s != 0)
		errExitEN(s, "pthread_create");
	s = pthread_create(&t2, NULL, threadFunc, &tp2);
	if (s != 0)
		errExitEN(s, "pthread_create");

	s = pthread_join(t1, NULL);
	if (s != 0)
		errExitEN(s, "pthread_join");
	s = pthread_join(t2, NULL);
	if (s != 0)
		errExitEN(s, "pthread_join");

	printf("glob = %d\n", glob);
	exit(EXIT_SUCCESS);

}
