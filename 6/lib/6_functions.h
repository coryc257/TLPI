/*
 * 6_functions.h
 *
 *  Created on: May 12, 2021
 *      Author: cory
 */

#ifndef LIB_6_FUNCTIONS_H_
#define LIB_6_FUNCTIONS_H_

int __mem_segments__main(int argc, char **argv);
int __necho__main(int argc, char **argv);
int __display_env__main(int argc, char **argv);
int __modify_env__main(int argc, char **argv);
int __longjmp__main(int argc, char **argv);
int __setjmp_vars__main(int argc, char **argv);
int __i_tripped__main(int argc, char **argv);

int my_unsetenv(const char *name);
int my_setenv(const char *name, const char *value, int overwrite);

#endif /* LIB_6_FUNCTIONS_H_ */
