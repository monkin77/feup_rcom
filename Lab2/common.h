#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define BIT(n) 1 << n

#define FLAG_BYTE 0x7E

#define SET_CONTROL_BYTE 0x03
#define UA_CONTROL_BYTE 0x07
#define RR_CONTROL_BYTE(r) (BIT(7*r) | 0x05) // r -> 0 or 1  
#define REJ_CONTROL_BYTE(r) (BIT(7*r) | 0x01)  // r -> 0 or 1  
#define INFO_CONTROL_BYTE(s) (BIT(6*s) & 0x40)  // s -> 0 or 1

#define EMISSOR_CMD_ABYTE 0x03
#define EMISSOR_ANSWER_ABYTE 0x01
#define RECEPTOR_ANSWER_ABYTE 0x03
#define RECEPTOR_CMD_ABYTE 0x01

#define FRAME_DATA_SIZE 9

typedef enum State {
  START,
  FLAG_RCV,
  ADDR_RCV,
  CTRL_RCV,
  BCC_OK,
  DATA_RCV,
  BCC2_OK,
  STOP
} State;

/**
 * @brief Calculate BCC2 according to data by doing XOR iteratively
 * 
 * @param data 
 * @return u_int8_t BCC2
 */
u_int8_t generateBCC2(u_int8_t* data);

/**
 * @brief 
 * 
 */
int sendSupervisionFrame(int fd, u_int8_t addr, u_int8_t ctrl);

#endif
