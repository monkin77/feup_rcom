#include "common.h"
#include "signal.h"

#define MODEMDEVICE "/dev/ttyS1"
#define CMD_ABYTE 0x03
#define ANSWER_ABYTE 0x01

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

const u_int8_t BCC = CMD_ABYTE ^ ANSWER_ABYTE; // Protection field

volatile int STOP=FALSE;

int messageFlag = 1, conta = 0;

void atende() {
   printf("alarme # %d\n", conta + 1);
   messageFlag = 1;
   conta++;
}

int main(int argc, char** argv) {
    int fd,c, res, res_read = 0;
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

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 char received */



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

    // Send SET
    u_int8_t ans[5] = {FLAG_BYTE, CMD_ABYTE, SET_CONTROL_BYTE, BCC, FLAG_BYTE}; 

    // Receive UA
    while (STOP==FALSE) {       /* loop for input */

      if (conta == 3) {
        printf("Communication failed\n");
        break;
      }

      if (messageFlag) {
        write(fd, ans, sizeof(ans));
        messageFlag = 0;
        i = 0;
        alarm(3);
      }

      u_int8_t byte;
      res_read = read(fd, &byte, 1);
      if (res_read == -1) {
        continue;
      }
      if (i == 0 && byte != FLAG_BYTE) continue;
      if (i > 0 && byte == FLAG_BYTE) {
        if (i != 4 || buf[3] != ans[3]) {
          i = 0;
          continue;
        } // TODO: TEST IF IT'S STILL RECEIVING GARBAGE
        STOP = TRUE;
      }
      buf[i++] = byte;
    }

    if (STOP) {
      printf("Received:\n");
      for (int x = 0; x < i; ++x)
        printf("0x%x ", buf[x]);
    }

    sleep(1); // Avoid changing config before sending data (transmission error)
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
          perror("tcsetattr");
          exit(-1);
    }
    close(fd);
    printf("Done\n");
    return 0;
}
