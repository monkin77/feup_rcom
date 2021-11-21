#include "interface.h"

int status;

int llopen(char* fileName, int newStatus) {
  status = newStatus;
  int fd;

  if (status == EMISSOR) fd = openEmissor(fileName);
  else if (status == RECEPTOR) fd = openReceptor(fileName);
  else {
    fprintf(stderr, "Status is either emissor (0) or receptor (1)\n");
    return -1;
  }

  return fd;
}

int llclose(int fd) {
  int ans;

  if (status == EMISSOR) ans = closeEmissor(fd);
  else ans = closeReceptor(fd);

  if (ans < 0) return ans;
  return 1;
}

int llwrite(int fd, u_int8_t* buffer, int length) {
  if (status == RECEPTOR) {
    fprintf(stderr, "The receptor can't send data!\n");
    return -1;
  }

  return sendDataFrame(fd, buffer, length);
}

int llread(int fd, u_int8_t* buffer) {
  if (status == EMISSOR) {
    fprintf(stderr, "The emissor can't read data!\n");
    return -1;
  }

  return receiveDataFrame(fd, buffer);
}

