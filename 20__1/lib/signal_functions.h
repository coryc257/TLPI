/*
 * signal_functions.h
 *
 *  Created on: May 31, 2021
 *      Author: cory
 */

#ifndef SIGNAL_FUNCTIONS_H_
#define SIGNAL_FUNCTIONS_H_

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "../lib/signal_functions.h"
#include "tlpi_hdr.h"

void printSigset(FILE *of, const char *prefix, const sigset_t *sigset);
int printSigMask(FILE *of, const char *msg);
int printPendingSigs(FILE *of, const char *msg);

#endif /* SIGNAL_FUNCTIONS_H_ */
