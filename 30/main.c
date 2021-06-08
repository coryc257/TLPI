/*
 * main.c
 *
 *  Created on: Jun 7, 2021
 *      Author: cory
 */

#include <pthread.h>
#include "tree/lib/tree.h"
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
	TREE *tree;
	tree = malloc(sizeof(TREE));
	if (initialize(tree)!=0)
		errExitEN(tree->error, "tree::initialize\n");
	add(tree, "best cats", "mal and zoe");
	add(tree, "good dog", "armani");
	add(tree, "zbad dog", "HELLHOUND");
	add(tree, "abad dog", "1l");
	add(tree, "0bad dog", "2l");
	add(tree, "0cad dog", "2l");

	printf("%d,%d\n",strcmp("a","z"),strcmp("b","a"));
}
