/*
 * 11.h
 *
 *  Created on: May 16, 2021
 *      Author: cory
 */

#ifndef LIB_11_H_
#define LIB_11_H_

typedef struct __return {
	int status;
	void *obj;
} RETURN;

typedef struct __mparm {
	int status;
	void *obj;
} PARAM;

typedef RETURN (*method)(PARAM* obj);

method get_method(char *method);



#endif /* LIB_11_H_ */
