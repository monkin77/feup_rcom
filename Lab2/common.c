#include "common.h"

u_int8_t generateBCC2(u_int8_t* data) {
  u_int8_t result = data[0];

  for (int i = 1; i < FRAME_DATA_SIZE; i++) {
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
        return 1;
    }
    return 0;
}
