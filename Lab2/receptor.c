#include "common.h"

#define CMD_ABYTE 0x01
#define ANSWER_ABYTE 0x03

volatile int STOP=FALSE;

const u_int8_t BCC = CMD_ABYTE ^ ANSWER_ABYTE;

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
    printf("Reading from Serial port...\n");

    while (STOP==FALSE) {       /* loop for input */
      u_int8_t byte;
      res_read = read(fd, &byte, 1);
      if (res_read == -1) {
        printf("Read error\n");
        exit(1);
      }
      if (i == 0 && byte != FLAG_BYTE) continue;
      if (i > 0 && byte == FLAG_BYTE) STOP = TRUE;
      buf[i++] = byte;
    }

    printf("Received:\n");
    for (int i = 0; i < 5; ++i)
      printf("0x%x ", buf[i]);
    printf("\nSending UA...\n");

    u_int8_t answer[5] = {FLAG_BYTE, ANSWER_ABYTE, UA_CONTROL_BYTE, BCC, FLAG_BYTE};

    int res_write;
    res_write = write(fd, answer, sizeof(answer));
    if (res_write == -1) {
      printf("Error writing\n");
      exit(1);
    }

    sleep(1); // Avoid changing config before sending data (transmission error)

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    printf("Done\n");
    return 0;
}
