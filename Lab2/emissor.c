#include "common.h"

#define MODEMDEVICE "/dev/ttyS1"

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

volatile int STOP=FALSE;

int main(int argc, char** argv) {
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf_write[255];
    int i, sum = 0, speed = 0;
    
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

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



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
    
    // Read input string
    gets(buf_write);
    printf("String being sent: %s\n", buf_write);
    
    res = write(fd, buf_write, sizeof(buf_write));
    printf("%d bytes written\n", res);
 

  /* 
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
    o indicado no gui�o 
  */

    printf("Reading from Serial Port...\n");

    char buf_read[255];
    res = 0, i = 0;
    while (STOP == FALSE) {
      char c;
      res = read(fd, &c, 1);
      if (res == -1) {
        printf("Read error\n");
        exit(1);
      }

      buf_read[i++] = c;
      if (c == '\0') {
          STOP = TRUE;
      }
    }

    printf("Received: %s\n", buf_read);

    sleep(1); // Avoid changing config before sending data (transmission error)
    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
          perror("tcsetattr");
          exit(-1);
    }
    close(fd);
    printf("Done\n");
    return 0;
}
