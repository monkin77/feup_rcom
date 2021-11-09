#include "common.h"
#include "signal.h"
#include "emissor.h"

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

const u_int8_t BCC_SET = EMISSOR_CMD_ABYTE ^ SET_CONTROL_BYTE; // Protection fields
const u_int8_t BCC_UA = RECEPTOR_ANSWER_ABYTE ^ UA_CONTROL_BYTE;

volatile int STOP_EXEC=FALSE;

int messageFlag = 1, conta = 0;

void atende() {
   printf("alarme # %d\n", conta + 1);
   messageFlag = 1;
   conta++;
}

int sendSet(int fd, int alarmInterval) {
  int res_read = 0, i = 0;
  u_int8_t buf[255];

  u_int8_t ans[5] = {FLAG_BYTE, EMISSOR_CMD_ABYTE, SET_CONTROL_BYTE, BCC_SET, FLAG_BYTE}; 

  // Send SET and receive answer
  while (STOP_EXEC==FALSE) {

    if (conta == 3) {
      printf("Communication failed\n");
      return 1;
    }

    if (messageFlag) {
      write(fd, ans, sizeof(ans));
      messageFlag = 0;
      i = 0;
      alarm(alarmInterval);
    }

    u_int8_t byte;
    res_read = read(fd, &byte, 1);
    // printf("Received %x\n", byte);
    if (res_read == -1) {
      continue;
    }
    if (i == 0 && byte != FLAG_BYTE) continue;
    if (i > 0 && byte == FLAG_BYTE) {
      if (i != 4 || buf[3] != BCC_UA) {
        i = 0;
        continue;
      }
      STOP_EXEC = TRUE;
    }
    buf[i++] = byte;
  }

  if (STOP_EXEC) {
    printf("Received:\n");
    for (int x = 0; x < i; ++x)
      printf("0x%x ", buf[x]);
  }

  return 0;
}

int main(int argc, char** argv) {
    int fd,c, res_read = 0;
    struct termios oldtio,newtio;
    u_int8_t buf[255];
    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    (void)signal(SIGALRM, atende);

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
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

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    if (sendSet(fd, 3)) exit(1);
    
    sleep(1); // Avoid changing config before sending data (transmission error)
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
          perror("tcsetattr");
          exit(-1);
    }
    close(fd);
    printf("Done\n");
    return 0;
}
