#ifndef EMISSOR_H
#define EMISSOR_H

#define ALARM_INTERVAL 3 // Time interval between alarm triggers

/**
 * @brief Attempts to setup connection, by sending a SET and verifying the response (UA)
 * 
 * @param fd Serial Port fd
 * @return int 0 if successful, 1 otherwise
 */
int sendSet(int fd);


/**
 * @brief Attempts to end connection, by sending a DISC, verifying the response DISC and sending UA
 * 
 * @param fd Serial Port fd
 * @return int 0 if successful, 1 otherwise
 */
int sendDisc(int fd);

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

int sendDataFrame(int fd, u_int8_t* data, int dataSize);

/**
 * @brief Resets alarm variables
 * 
 */
void resetAlarmVariables();

#endif
