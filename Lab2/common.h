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

#define EMISSOR_CMD_ABYTE 0x03
#define EMISSOR_ANSWER_ABYTE 0x01
#define RECEPTOR_ANSWER_ABYTE 0x03
#define RECEPTOR_CMD_ABYTE 0x01

typedef enum State {
  START,
  FLAG_RCV,
  ADDR_RCV,
  CTRL_RCV,
  BCC_OK,
  STOP
} State;
