#ifndef INTERFACE_H
#define INTERFACE_H

#include "emissor.h"
#include "receptor.h"

// EMISSOR OR RECEPTOR
#define EMISSOR 0
#define RECEPTOR 1



/**
*   @brief Opens connection with file
*
*   @param fileName 
*   @param status 0 -> Transmitter, 1 -> Receiver
*   @return file descriptor if OK, negative otherwise
*/
int llopen(char* fileName, int status);

/**
*   @brief Closes connection with file
*
*   @param fd -> File descriptor of the file
*/
int llclose(int fd);

/**
 * @brief Sends a packet with the given data. The data can't exceed the specified max
 * 
 * @param fd File descriptor of the serial port
 * @param buffer Buffer containing the data
 * @param length Size of the buffer
 * @return int Data size if successful and -1 otherwise
 */
int llwrite(int fd, u_int8_t* buffer, int length);

/**
 * @brief Receives a data packet
 * 
 * @param fd File descriptor of the serial port
 * @param buffer Buffer containing the data
 * @return Data size if success, -1 otherwise 
 */
int llread(int fd, u_int8_t *buffer);

#endif
