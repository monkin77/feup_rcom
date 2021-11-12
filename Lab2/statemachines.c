#include "statemachines.h"

int receive_supervision_machine(State* state, int fd, u_int8_t addr, u_int8_t ctrl, u_int8_t* mem) {
  int res; u_int8_t byte;
  res = read(fd, &byte, 1);
  if (res == -1) {
    printf("Read error\n");
    return 1;
  }

  if (*state == START) {
    if (byte == FLAG_BYTE) *state = FLAG_RCV;
  }
  else if (*state == FLAG_RCV) {
    if (byte == FLAG_BYTE) return 0;
    else if (byte == addr) {
      mem[0] = byte;
      *state = ADDR_RCV;
    }
    else *state = START;
  }
  else if (*state == ADDR_RCV) {
    if (byte == FLAG_BYTE) *state = FLAG_RCV;
    else if (byte == ctrl) {
      mem[1] = byte;
      mem[2] = mem[0] ^ mem[1];
      *state = CTRL_RCV;
    }
    else *state = START;
  }
  else if (*state == CTRL_RCV) {
    if (byte == FLAG_BYTE) *state = FLAG_RCV;
    else if (mem[2] == byte) *state = BCC_OK;
    else *state = START;
  }
  else if (*state == BCC_OK) {
    if (byte == FLAG_BYTE) *state = STOP;
    else *state = START;
  }
  return 0;
}
