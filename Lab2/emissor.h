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

void atende();

int sendDataFrame(int fd, u_int8_t* data, int dataSize);

/**
 * @brief Resets alarm variables
 * 
 */
void resetAlarmVariables();

#endif
