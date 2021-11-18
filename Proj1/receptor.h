#ifndef RECEPTOR_H
#define RECEPTOR_H

#include "common.h"

/**
 * @brief Opens the receptor for sending data.
 * 
 * @param fileName Name of the file representing the serial port.
 * @return File descriptor or negative if there's an error.
 */
int openReceptor(char fileName[]);

/**
 * @brief Closes the receptor and the respective serial port.
 * 
 * @param fd Name of the file representing the serial port.
 * @return int 0 if successful, -1 otherwise
 */
int closeReceptor(int fd);

/**
 * @brief Attempts to end the connection, by receiving a DISC, re-sending it and receiving a UA
 * 
 * @param fd Serial Port fd
 * @return int 0 if successful, 1 otherwise
 */
int discReceptor(int fd);

/**
 * Receives a SET message from the receptor
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
 * @return int 0 if success, -1 otherwise 
 */
int receiveDataFrame(int fd, u_int8_t* data);

#endif
