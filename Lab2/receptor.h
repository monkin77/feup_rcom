#ifndef RECEPTOR_H
#define RECEPTOR_H

#include "common.h"

/**
 * State machine to receive a SET message from the receptor.
 */
int receiveSet(int fd);

/**
 * Sends a UA answer.
 */
int sendUA(int fd);

#endif
