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

/**
 * @brief Receives a data frame
 * 
 * @param fd 
 * @param data 
 * @return int 
 */
int receiveDataFrame(int fd, u_int8_t* data);

#endif
