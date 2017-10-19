/*Non-Canonical Input Processing*/

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "link_layer.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

#define FLAG  0x7e
#define A     0x03
#define C_SET 0x03

unsigned int retry_counter, state;

int llopen(int port, char mode){

  if(port != 0 && port != 1){
    printf("ERROR:llopen: invalid serial port number: %d\n", port);
    exit(-1);
  }

/*
  if(mode != TRANSMITTER && mode != RECEIVER){
    printf("Usage: invalid mode: %d\n", mode);
    exit(-1);
  }
  */

/*
  if((strcmp(mode, "TRANSMITTER") != 0) && (strcmp(mode, "RECEIVER") != 0)){
    printf("ERROR:llopen: invalid mode: %s\n", mode);
    exit(-1);
  }
*/

  char portstring[] = "/dev/ttyS";
  char *portnum;
  if(port == 1) portnum = "1";
  else portnum = "0";
  char *serial = strcat(portstring, portnum);
  printf("Serial port: %s\n", serial);

  unsigned char set_message[5];

  int fd, res;
  struct termios oldtio,newtio;
  char buf[255];

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    if((fd = open(serial, O_RDWR | O_NOCTTY )) < 0){
      printf("llopen()::could not open serial port %d\n", port);
      exit(4);
    }

    if (fd < 0 ) {
       perror(serial);
       exit(-1);
   }

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
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

    /*
      VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
      leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    //if(strcmp(mode, "TRANSMITTER") == 0) {

    if(mode == TRANSMITTER){
      /* transmitter stuff: send SET and stuff */

      retry_counter = 0;
      state = SET_SEND;

      printf("oi mig\n");

      set_message[0] = FLAG;
      set_message[1] = A;
      set_message[2] = C_SET;
      set_message[3] = set_message[1] ^ set_message[2];
      set_message[4] = FLAG;

      write(*serial, set_message, sizeof(set_message));
    }
    else{
      /* receiver stuff: state machine and stuff */
    }

    printf("New termios structure set\n");

    //Send
    int i;
    printf("\nMessage to send:\n");
    for(i=0; i < 5; i++){
      printf("SET[%d] = %x\n", i, set_message[i]);
    }

    res = write(fd, buf, strlen(buf) + 1);
    printf("\n%d bytes written\n", res);

  /*
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
    o indicado no gui�o
  */

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;

}

int llwrite(int fd, char *buffer, int len){
  return 0;
}
int llread(int fd, char *buffer){
  return 0;
}
int llclose(int fd){
  return 0;
}
