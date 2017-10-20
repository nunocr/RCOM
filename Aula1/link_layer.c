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


volatile int STOP=FALSE;


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

  char portstring[] = "/dev/ttyS";
  char *portnum;
  if(port == 1) portnum = "1";
  else portnum = "0";
  char *serial_name = strcat(portstring, portnum);
  printf("Serial port: %s\n", serial_name);

  unsigned char set_message[5];
  unsigned char byte;

  int fd, res;
  struct termios oldtio,newtio;

  /*
    Open serial port dmevice for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    if((fd = open(serial_name, O_RDWR | O_NOCTTY )) < 0){
      printf("llopen()::could not open serial port %d\n", port);
      exit(-1);
    }

    if (fd < 0 ) {
       perror(serial_name);
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


	//TRANSMITTER


    if(mode == TRANSMITTER){

      retry_counter = 0;
      state = SET_SEND;

	  printf("Sending SET message...\n");
	  
      set_message[0] = FLAG;
      set_message[1] = A;
      set_message[2] = C_SET;
      set_message[3] = set_message[1] ^ set_message[2];
      set_message[4] = FLAG;

      res = write(fd, set_message, sizeof(set_message));
	  printf("llopen:write: %d bytes written\n", res);
	  }
    
    
    
    //RECEIVER
    
    
    else{
      
      state = START;
      
      /* state machine for SET message processing */
      
      while(!STOP){
      
      if(state != END){
		  read(fd, &byte, 1);
		  printf("Current byte being proccessed: %02x\n", byte);
	  }
	  
		  switch(state){
		  
			case START:
				if(byte == FLAG){
					state = FLAG_RCV;
					printf("First FLAG processed successfully: %02x\n", byte);
				}
				else { state = START; printf("START if 1\n"); }
				break;
				
			case FLAG_RCV:
				if(byte == A) {
					state = A_RCV;
					printf("A processed successfully: %02x\n", byte);
				}
				else if(byte == FLAG){ state = FLAG_RCV; printf("FLAG_RCV if 1\n"); }
				else{ state = START; printf("FLAG_RCV if 2\n"); }
				break;
				
			case A_RCV:
				if(byte == C_SET) {
					state = C_RCV;
					printf("C_SET processed successfully: %02x\n", byte);
				}
				else if(byte == FLAG){ state = FLAG_RCV; printf("A_RCV if 1\n"); }
				else{ state = START; printf("A_RCV if 2\n"); }
				break;
				
			case C_RCV:
				if(byte == (set_message[1] ^ set_message[2])){
					state = BCC_OK;
					printf("BCC processed successfully: %02x\n", byte);
				}
				else if(byte == FLAG){ state = FLAG_RCV; printf("\nC_RCV if 1\n"); }
				else { state = START; printf("\nC_RCV if 2\n"); }
				break;
				
			case BCC_OK:
				if(byte == FLAG){
					state = END;
					printf("Last FLAG processed successfully: %02x\n", byte);
				}
				else { state = START; printf("BCC_OK if 1\n"); }
				break;
				
			case END:
				printf("Reached end of State Machine\n");
				STOP = TRUE;
				break;
				
			default:
				printf("You shouldnt be here. go away.\n");
				break;
		  }
	  }
	  
	  printf("\nSET processed successfully, sending UA message:\n");
	  
	  unsigned char ua_message[5];
	  ua_message[0] = FLAG;
	  ua_message[1] = A;
	  ua_message[2] = UA;
	  ua_message[3] = UA ^ A;
	  ua_message[4] = FLAG;
	  
	  if(write(fd, ua_message, sizeof(ua_message)) == 0){
		printf("ERROR:llopen: failed to send UA message.\n");
		exit(-1);
	  }
	 printf("Connection established\n");
	 printf("Serial port: %d", fd);
    }
    
	return fd;

/*
    printf("New termios structure set\n");

    //Send
    int i;
    printf("\nMessage to send:\n");
    for(i=0; i < 5; i++){
      printf("SET[%d] = %x\n", i, set_message[i]);
    }

    res = write(fd, buf, strlen(buf) + 1);
    printf("\n%d bytes written\n", res);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
    
*/
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
