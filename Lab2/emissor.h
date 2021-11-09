/**
 * @brief Attempts to setup connection, by sending a SET and verifying the response (UA)
 * 
 * @param fd Serial Port fd
 * @param alarmInterval Time interval between alarm triggers
 * @return int 0 if successful, 1 otherwise
 */
int sendSet(int fd, int alarmInterval);

void atende();