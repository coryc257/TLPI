/*
 * exec_container.h
 *
 *  Created on: Jun 15, 2021
 *      Author: cory
 */

#ifndef EXEC_CONTAINER_H_
#define EXEC_CONTAINER_H_

typedef struct exec_container {
	char **args;
	char *exe;
} EXEC_CONTAINER;
void construct_args(int argc, char**args, EXEC_CONTAINER *ec);

#endif /* EXEC_CONTAINER_H_ */
