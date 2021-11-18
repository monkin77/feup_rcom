#include "common.h"

u_int8_t generateBCC2(u_int8_t* data, int dataSize) {
  u_int8_t result = data[0];

  for (int i = 1; i < dataSize; i++) {
    result ^= data[i];
  }

  return result;
}

int sendSupervisionFrame(int fd, u_int8_t addr, u_int8_t ctrl) {
    const u_int8_t BCC = addr ^ ctrl;
    u_int8_t message[5] = {FLAG_BYTE, addr, ctrl, BCC, FLAG_BYTE};
    int res;
    res = write(fd, message, sizeof(message));
    if (res == -1) {
        printf("Error writing\n");
        return -1;
    }
    return 0;
}

int receiveSupervisionFrame(State* state, int fd, u_int8_t addr, u_int8_t ctrl, u_int8_t* rej, u_int8_t* mem) {
  int res, isRejected; u_int8_t byte;
  res = read(fd, &byte, 1);
  if (res == -1) {
    printf("Read error\n");
    return -1;
  }

  switch (*state) {

    case START:
      if (byte == FLAG_BYTE) *state = FLAG_RCV;
      break;

    case FLAG_RCV:
      isRejected = 0;
      if (byte == FLAG_BYTE) return 0;
      else if (byte == addr) {
        mem[0] = byte;
        *state = ADDR_RCV;
      }
      else *state = START;
      break;

    case ADDR_RCV:
      if (rej != NULL)
        if (byte == *rej) isRejected = 1;

      if (byte == FLAG_BYTE) *state = FLAG_RCV;
      else if (byte == ctrl || isRejected) {
        mem[1] = byte;
        mem[2] = mem[0] ^ mem[1];
        *state = CTRL_RCV;
      }

      else *state = START;
      break;

    case CTRL_RCV:
      if (byte == FLAG_BYTE) *state = FLAG_RCV;
      else if (mem[2] == byte) *state = BCC_OK;
      else *state = START;
      break;

    case BCC_OK:
      if (byte == FLAG_BYTE) *state = STOP;
      else *state = START;
      break;
  }
  if (isRejected) return -1;
  return 0;
}
