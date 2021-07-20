/*
 * io_n.h
 *
 *  Created on: Jul 19, 2021
 *      Author: cory
 */

#ifndef LIB_IO_N_H_
#define LIB_IO_N_H_

#include <sys/types.h>

ssize_t readn(int fd, void *buf, size_t len);
ssize_t writen(int fd, const void *buf, size_t len);

#endif /* LIB_IO_N_H_ */
