#ifndef EMISSOR_H
#define EMISSOR_H

#include "common.h"
#include <signal.h>
#include <time.h>
#include <stdlib.h>

#define ALARM_INTERVAL 3 // Time interval between alarm triggers

/**
 * @brief Opens the emissor for sending data.
 * 
 * @param fileName Name of the file representing the serial port.
 * @return File descriptor or negative if there's an error.
 */
int openEmissor(char fileName[]);

/**
 * @brief Closes the emissor and the respective serial port.
 * 
 * @param fd Name of the file representing the serial port.
 * @return int 0 if successful, -1 otherwise
 */
int closeEmissor(int fd);

/**
 * @brief Attempts to setup connection, by sending a SET and verifying the response (UA)
 * 
 * @param fd Serial Port fd
 * @return int 0 if successful, -1 otherwise
 */
int sendSet(int fd);


/**
 * @brief Attempts to end connection, by sending a DISC, verifying the response DISC and sending UA
 * 
 * @param fd Serial Port fd
 * @return int 0 if successful, -1 otherwise
 */
int discEmissor(int fd);

/**
 * Handles an alarm interruption (to then resend a frame)
 */
void atende();

/**
 * @brief Stuffs data into stuffedData
 * 
 * @param buffer 
 * @param size 
 * @param stuffedData 
 * @return int stuffedData size
 */
int stuffData(u_int8_t* buffer, int size, u_int8_t bcc2, u_int8_t* stuffedData);

/**
 * @brief Sends a frame with the given data. The data can't exceed the specified max
 * 
 * @param fd File descriptor of the serial port
 * @param data Buffer containing the data
 * @param dataSize Size of the buffer
 * @return int Data size if successful and -1 otherwise
 */
int sendDataFrame(int fd, u_int8_t* data, int dataSize);

/**
 * @brief Resets alarm variables
 */
void resetAlarmVariables();

/**
 * @brief Inserts an error in a given buffer, given a certain probability
 * 
 * @param data buffer that may receive errors
 * @param dataIndex Index that will be changed
 * @param probability probability of introducing error
 */
void insertError(u_int8_t *data, int dataIndex, int probability);

#endif
