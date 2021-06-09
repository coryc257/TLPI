/*
 * main.c
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */

#include <pthread.h>
#include "tree/lib/tree.h"
#include "tlpi_hdr.h"

static TREE *tree;

static void *
t1_thread(void *arg)
{
	add(tree, "zbad dog", "HELLHOUND");
	add(tree, "zaad dog", "HELLHOUND");
	add(tree, "zabd dog", "HELLHOUND");
	add(tree, "za0d dog", "HELLHOUND");
	add(tree, "zcad dog", "HELLHOUND");
	add(tree, "zcbd dog", "HELLHOUND");
	printf("HELLHOUNDS INC\n");
	return NULL;
}

static void *
t2_thread(void *arg)
{
	add(tree, "abad dog", "1l");
	add(tree, "best cats", "mal and zoe");
	add(tree, "good dog", "armani");
	add(tree, "0bad dog", "2l");
	add(tree, "0cad dog", "2l");
	printf("Wow animals\n");
	return NULL;
}

static void *
t3_thread(void *arg)
{
	printf("DIE\n");
	delete(tree, "zbad dog");
	delete(tree, "zabd dog");
	delete(tree, "zaad dog");
	delete(tree, "zcad dog");
	delete(tree, "zcbd dog");
	delete(tree, "za0d dog");
	printf("HELLHOUNDS SLAYED\n");
	return NULL;
}

static void *
t4_thread(void *arg)
{
	printf("Can't have these bad dogs\n");
	delete(tree, "0bad dog");
	delete(tree, "0cad dog");
	delete(tree, "abad dog");
	printf("Bad Dogs Sent Packin\n");
	return NULL;
}

static void *
tmzh(void *arg)
{
	void *lookup_item;
	lookup_item = NULL;
	check_again:
	if(lookup(tree, "best cats", &lookup_item))
		printf("best cats:%s\n", (char*)lookup_item);
	else
		goto check_again;
	return NULL;
}

int
main(int argc, char *argv[])
{


	tree = malloc(sizeof(TREE));
	if (initialize(tree)!=0)
		errExitEN(tree->error, "tree::initialize\n");

	pthread_t t1, t2, tmz;
	pthread_t kill, pack;

	pthread_create(&tmz,NULL,tmzh,NULL); //printf("001\n");
	pthread_create(&t1,NULL,t1_thread, NULL); //printf("002\n");
	pthread_create(&t2,NULL,t2_thread, NULL); //printf("003\n");


	pthread_join(t1, NULL); //printf("004\n");
	pthread_join(t2, NULL); //printf("005\n");



	pthread_create(&kill,NULL, t3_thread, NULL); //printf("006\n");
	pthread_create(&pack,NULL, t4_thread, NULL); //printf("007\n");


	pthread_join(kill,NULL); //printf("008\n");
	pthread_join(pack,NULL); //printf("009\n");
	pthread_join(tmz,NULL);

	printf("God is in his heaven and\nAll is right with the world\n");
}
