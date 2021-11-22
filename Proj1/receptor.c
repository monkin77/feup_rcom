#include "receptor.h"

int r = 1;
struct termios oldtioreceptor;

int openReceptor(char filename[]) {
  int fd;
  struct termios newtio;

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(filename, O_RDWR | O_NOCTTY);
  if (fd < 0) {
      fprintf(stderr, "%s\n", filename);
      return -1;
  }

  if (tcgetattr(fd,&oldtioreceptor) == -1) { /* save current port settings */
    fprintf(stderr, "tcgetattr");
    return -1;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* reading 1 char at a time */


/* 
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
  leitura do(s) pr�ximo(s) caracter(es)
*/

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
    fprintf(stderr, "tcsetattr error\n");
    return -1;
  }

  if (receiveSet(fd) < 0) return -1;
  return fd;
}

int closeReceptor(int fd) {
  if (discReceptor(fd)) return -1;

  sleep(1); // Avoid changing config before sending data (transmission error)

  tcsetattr(fd,TCSANOW,&oldtioreceptor);
  close(fd);
  return 0;
}

int receiveSet(int fd) {
  State state = START;
  u_int8_t mem[3];

  while (state != STOP) {
    if (receiveSupervisionFrame(&state, fd, EMISSOR_CMD_ABYTE, SET_CONTROL_BYTE, NULL, mem) < 0)
      return -1;
  }

  if (sendSupervisionFrame(fd, RECEPTOR_ANSWER_ABYTE, UA_CONTROL_BYTE) < 0) return -1;

  return 0;
}

int discReceptor(int fd) {
  State state = START;
  u_int8_t mem[3];
  
  while (state != STOP) {
    if (receiveSupervisionFrame(&state, fd, EMISSOR_CMD_ABYTE, DISC_CONTROL_BYTE, NULL, mem) < 0)
      return -1;
  }

  if (sendSupervisionFrame(fd, RECEPTOR_ANSWER_ABYTE, DISC_CONTROL_BYTE) < 0) return -1;

  while (state != STOP) {
    if (receiveSupervisionFrame(&state, fd, EMISSOR_CMD_ABYTE, UA_CONTROL_BYTE, NULL, mem) < 0)
      return -1;
  }

  return 0;
}

int destuffData(u_int8_t* stuffed_data, int size, u_int8_t* buffer, u_int8_t* bcc2) {
  u_int8_t destuffed_buffer[FRAME_DATA_SIZE + 1];
  int bufferIdx = 0;

  for (int i = 0; i < size; i++) {
    u_int8_t currByte = stuffed_data[i];
    if (currByte == ESC_BYTE) {
      u_int8_t nextByte = stuffed_data[++i];
      if (nextByte == STUFFED_FLAG_BYTE) destuffed_buffer[bufferIdx++] = FLAG_BYTE;
      else if (nextByte == STUFFED_ESC_BYTE) destuffed_buffer[bufferIdx++] = ESC_BYTE;
      else fprintf(stderr, "There should be no isolated ESC byte \n");
    } else destuffed_buffer[bufferIdx++] = currByte;
  }
  bufferIdx--;

  *bcc2 = destuffed_buffer[bufferIdx];

  memcpy(buffer, destuffed_buffer, bufferIdx);
  return bufferIdx;
}

int receiveDataFrame(int fd, u_int8_t* data) {
  State state = START;
  u_int8_t receivedAddress, receivedControl, calculatedBCC,
          ctrl = INFO_CONTROL_BYTE(1-r), repeatedCtrl = INFO_CONTROL_BYTE(r), calculatedBCC2, bcc2;

  int currentDataIdx, isRepeated, dataSize;
  u_int8_t stuffed_data[MAX_STUFFED_DATA_SIZE];

  while (state != STOP) {
    int res; u_int8_t byte;
    res = read(fd, &byte, 1);
    if (res == -1) {
      fprintf(stderr, "Read error. Serial cable is probably disconnected\n");
      return -1;
    }

    switch (state) {
      case START:
        if (byte == FLAG_BYTE) state = FLAG_RCV;
        break;

      case FLAG_RCV:
        if (byte == FLAG_BYTE) continue;
        else if (byte == EMISSOR_CMD_ABYTE) {
          isRepeated = 0;
          receivedAddress = byte;
          state = ADDR_RCV;
        }
        else state = START;
        break;

      case ADDR_RCV:
        if (byte == repeatedCtrl) isRepeated = 1;

        if (byte == FLAG_BYTE) state = FLAG_RCV;
        else if (byte == ctrl || isRepeated) {
          receivedControl = byte;
          calculatedBCC = receivedAddress ^ receivedControl;
          state = CTRL_RCV;
        }
        else state = START;
        break;

      case CTRL_RCV:
        if (byte == FLAG_BYTE) state = FLAG_RCV;
        else if (calculatedBCC == byte) {
          state = BCC_OK;
          currentDataIdx = 0;
        }
        else state = START;
        break;

      case BCC_OK:
        if (currentDataIdx >= MAX_STUFFED_DATA_SIZE) state = START;
        else if (byte == FLAG_BYTE) {
          dataSize = destuffData(stuffed_data, currentDataIdx, data, &bcc2);
          calculatedBCC2 = generateBCC2(data, dataSize);

          if (isRepeated) {
            if (sendSupervisionFrame(fd, RECEPTOR_ANSWER_ABYTE, RR_CONTROL_BYTE(1 - r)) < 0) return -1;
            state = START;
          }
          else if (calculatedBCC2 != bcc2) {
            if (sendSupervisionFrame(fd, RECEPTOR_ANSWER_ABYTE, REJ_CONTROL_BYTE(1 - r)) < 0) return -1;
            state = START;
          }
          else {
            state = STOP;
          }
        }
        else stuffed_data[currentDataIdx++] = byte;
        break;

      default:
        break;
    }
  }
  if (sendSupervisionFrame(fd, RECEPTOR_ANSWER_ABYTE, RR_CONTROL_BYTE(r)) < 0) return -1;
  r = 1 - r;
  return dataSize;
}
