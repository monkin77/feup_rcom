#include "receptor.h"

int receiveSet(int fd) {
  State state = START;

  while (state != STOP) {
    u_int8_t byte, addr_byte, bcc;
    int res_read;
    res_read = read(fd, &byte, 1);
    if (res_read == -1) {
      printf("Read error\n");
      return 1;
    }

    if (state == START) {
      if (byte == FLAG_BYTE) state = FLAG_RCV;
    }
    else if (state == FLAG_RCV) {
      if (byte == FLAG_BYTE) continue;
      else if (byte == EMISSOR_CMD_ABYTE) {
        addr_byte = byte;
        state = ADDR_RCV;
      }
      else state = START;
    }
    else if (state == ADDR_RCV) {
      if (byte == FLAG_BYTE) state = FLAG_RCV;
      else if (byte == SET_CONTROL_BYTE) {
        bcc = addr_byte ^ byte;
        state = CTRL_RCV;
      }
      else state = START;
    }
    else if (state == CTRL_RCV) {
      if (byte == FLAG_BYTE) state = FLAG_RCV;
      else if (bcc == byte) state = BCC_OK;
      else state = START;
    }
    else if (state == BCC_OK) {
      if (byte == FLAG_BYTE) state = STOP;
      else state = START;
    }
  }
  return 0;
}

int sendUA(int fd) {
  const u_int8_t BCC = RECEPTOR_ANSWER_ABYTE ^ UA_CONTROL_BYTE;
  u_int8_t message[5] = {FLAG_BYTE, RECEPTOR_ANSWER_ABYTE, UA_CONTROL_BYTE, BCC, FLAG_BYTE};
  int res;
  res = write(fd, message, sizeof(message));
  if (res == -1) {
    printf("Error writing\n");
    return 1;
  }
  return 0;
}


int main(int argc, char** argv) {
    int fd, c, res_read, i = 0;
    struct termios oldtio, newtio;
    u_int8_t buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */
    
    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(argv[1]); exit(-1);
    }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
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
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
    printf("Reading the SET message...\n");

    if (receiveSet(fd)) exit(1);

    printf("Received Set\n");
    printf("Sending UA...\n");

    if (sendUA(fd)) exit(1);

    sleep(1); // Avoid changing config before sending data (transmission error)

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    printf("Done\n");
    return 0;
}
