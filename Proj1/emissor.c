#include "emissor.h"

const u_int8_t BCC_SET = EMISSOR_CMD_ABYTE ^ SET_CONTROL_BYTE; // Protection fields
const u_int8_t BCC_DISC = EMISSOR_CMD_ABYTE ^ DISC_CONTROL_BYTE;
const u_int8_t BCC_UA = RECEPTOR_ANSWER_ABYTE ^ UA_CONTROL_BYTE;

volatile int STOP_EXEC=FALSE;

int messageFlag = 1, conta = 0, s = 0;
struct termios oldtio;

void resetAlarmVariables() {
  messageFlag = 1, conta = 0;
}

void atende() {
   messageFlag = 1;
   conta++;
}

int openEmissor(char fileName[]) {
  int fd;
  struct termios newtio;

  (void)signal(SIGALRM, atende);

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(fileName, O_RDWR | O_NOCTTY );
  if (fd < 0) {
    fprintf(stderr, "%s", fileName);
    return -1;
  }

  if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    fprintf(stderr, "tcgetattr");
    return -1;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 1;   /* unblock after 0.1secs or 1 char received */
  newtio.c_cc[VMIN] = 0;

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
    fprintf(stderr, "tcsetattr");
    return -1;
  }

  if (sendSet(fd) < 0) return -1;
  return fd;
}

int closeEmissor(int fd) {
  if (discEmissor(fd)) return -1;
  
  sleep(1); // Avoid changing config before sending data (transmission error)
  if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        fprintf(stderr, "tcsetattr");
        return -1;
  }
  close(fd);
  return 0;
}

int stuffData(u_int8_t* buffer, int size, u_int8_t bcc2, u_int8_t* stuffedData) {
  int stuffedDataIdx = 0;

  for (int i = 0; i < size; i++) {
    u_int8_t currByte = buffer[i];
    if (currByte == ESC_BYTE) {
      stuffedData[stuffedDataIdx++] = ESC_BYTE;
      stuffedData[stuffedDataIdx++] = STUFFED_ESC_BYTE;
    } 
    else if (currByte == FLAG_BYTE) {
      stuffedData[stuffedDataIdx++] = ESC_BYTE;
      stuffedData[stuffedDataIdx++] = STUFFED_FLAG_BYTE;
    } else {
      stuffedData[stuffedDataIdx++] = currByte;
    }
  }

  if (bcc2 == ESC_BYTE) {
    stuffedData[stuffedDataIdx++] = ESC_BYTE;
    stuffedData[stuffedDataIdx++] = STUFFED_ESC_BYTE;
  } else if (bcc2 == FLAG_BYTE) {
    stuffedData[stuffedDataIdx++] = ESC_BYTE;
    stuffedData[stuffedDataIdx++] = STUFFED_FLAG_BYTE;
  } else {
    stuffedData[stuffedDataIdx++] = bcc2;
  }

  return stuffedDataIdx;
}


int sendSet(int fd) {
  resetAlarmVariables();

  State state = START;
  u_int8_t mem[3], ans[5] = {FLAG_BYTE, EMISSOR_CMD_ABYTE, SET_CONTROL_BYTE, BCC_SET, FLAG_BYTE}; 

  while (state != STOP) {
    if (conta == 3) {
      fprintf(stderr, "Communication failed\n");
      return -1;
    }

    if (messageFlag) {
      write(fd, ans, 5);
      messageFlag = 0;
      state = START;
      alarm(ALARM_INTERVAL);
    }

    if (receiveSupervisionFrame(&state, fd, RECEPTOR_ANSWER_ABYTE, UA_CONTROL_BYTE, NULL, mem) < 0) {
      return -1;
    }
  }

  alarm(0); // deactivate alarm
  return 0;
}


void insertError(u_int8_t *data, int dataSize) {
  int r = rand() % 100;
  
  if (r < 5) {
    data[0] = 20;
  }

}


int sendDataFrame(int fd, u_int8_t* data, int dataSize) {
  if (dataSize > FRAME_DATA_SIZE) {
    fprintf(stderr, "Datasize cannot exceed the defined max frame size\n");
    return -1;
  }
  resetAlarmVariables();

  u_int8_t mem[3]; 

  u_int8_t controlByte = INFO_CONTROL_BYTE(s);
  u_int8_t BCC1 = EMISSOR_CMD_ABYTE ^ controlByte;
  u_int8_t BCC2 = generateBCC2(data, dataSize);

  u_int8_t frameHead[4] = {FLAG_BYTE, EMISSOR_CMD_ABYTE, controlByte, BCC1};
  u_int8_t flagByte = FLAG_BYTE;
  u_int8_t stuffedData[MAX_STUFFED_DATA_SIZE];

  State state = START;
  while (state != STOP) {
    
    int stuffedDataSize = stuffData(data, dataSize, BCC2, stuffedData);
    insertError(stuffedData, stuffedDataSize);

    if (conta == 3) {
      fprintf(stderr, "Communication failed\n");
      return -1;
    }

    if (messageFlag) {
      write(fd, frameHead, 4);
      write(fd, stuffedData, stuffedDataSize);
      write(fd, &flagByte, 1);

      messageFlag = 0;
      state = START;
      alarm(ALARM_INTERVAL);
    }

    u_int8_t rejByte = REJ_CONTROL_BYTE(s);
    
    int ret = receiveSupervisionFrame(&state, fd, RECEPTOR_ANSWER_ABYTE, RR_CONTROL_BYTE(1-s), &rejByte, mem);

    if (ret < 0) {
      fprintf(stderr, "Error receiving UA\n");
      return -1;
    }
    else if (ret > 0) {
      messageFlag = 1; // Resend the frame
      conta++;
    }
  }
  s = 1 - s;
  alarm(0); // deactivate alarm
  return dataSize;
}


int discEmissor(int fd) {
  resetAlarmVariables();
  
  State state = START;
  u_int8_t mem[3], ans[5] = {FLAG_BYTE, EMISSOR_CMD_ABYTE, DISC_CONTROL_BYTE, BCC_DISC, FLAG_BYTE};
  
  while (state != STOP) {
    if (conta == 3) {
      fprintf(stderr, "Communication failed \n");
      return -1;
    }

    if (messageFlag) {
      write(fd, ans, 5); 
      messageFlag = 0;
      state = START;
      alarm(ALARM_INTERVAL);
    }

    int ret = receiveSupervisionFrame(&state, fd, RECEPTOR_ANSWER_ABYTE, DISC_CONTROL_BYTE, NULL, mem);
    
    if (ret < 0) return -1;
  }
  
  alarm(0);

  if (sendSupervisionFrame(fd, EMISSOR_ANSWER_ABYTE, UA_CONTROL_BYTE) < 0) return -1;
  return 0;
}
