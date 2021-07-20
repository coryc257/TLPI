/*
 * make_pipe.h
 *
 *  Created on: Jul 19, 2021
 *      Author: cory
 */

#ifndef LIB_MAKE_PIPE_H_
#define LIB_MAKE_PIPE_H_

typedef int PD;
typedef struct __pipe {
	PD input;
	PD output;
} PIPE;

int make_pipe(PIPE **p);


#endif /* LIB_MAKE_PIPE_H_ */
