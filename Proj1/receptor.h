#ifndef RECEPTOR_H
#define RECEPTOR_H

#include "common.h"

/**
 * State machine to receive a SET message from the receptor.
 */
int receiveSet(int fd);

/**
 * @brief Destuf data into buffer
 * 
 * @param stuffed_data 
 * @param size 
 * @param buffer 
 * @param bcc2 
 * @return int size of the data
 */
int destuffData(u_int8_t* stuffed_data, int size, u_int8_t* buffer, u_int8_t* bcc2);

/**
 * @brief Receives a data frame
 * 
 * @param fd 
 * @param data 
 * @return int 
 */
int receiveDataFrame(int fd, u_int8_t* data);

#endif
