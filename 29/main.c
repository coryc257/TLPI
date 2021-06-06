/*
 * main.c
 *
 *  Created on: Jun 6, 2021
 *      Author: cory
 */

#include <pthread.h>

static pthread_t thread;

void *
handy(void *arg)
{
	void *ret;
	if (pthread_equal(thread, pthread_self()))
	{
		pthread_join(thread,&ret);
		printf("%s\n", (char*)arg);
		return (void*)0;
	}
	else
	{
		pthread_join(thread,&ret);
	}
	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_create(&thread,NULL, handy, "Mal And Zoe");
	handy(NULL);

	simple_thread(argc,argv);
}
