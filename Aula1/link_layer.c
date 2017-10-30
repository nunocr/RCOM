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


unsigned char C1 = 0x40;
//unsigned char BCC2 = 0x00;

void switchC1(){
	if (C1 == 0x00) C1 = 0x40;
	else            C1 = 0x00;
}

volatile int STOP = FALSE;

const short FLAG_SUB = 0x5E7D;
const short ESC_SUB = 0x5D7D;

unsigned int retry_counter, state, connected = FALSE;

int llopen(int port, char mode){

  if(port != 0 && port != 1){
    printf("ERROR:llopen: invalid serial port number: %d\n", port);
    exit(-1);
  }

    if((mode != TRANSMITTER) && (mode != RECEIVER)){
		  printf("Usage: invalid mode: %d\n", mode);
		  exit(-1);
}

char portstring[] = "/dev/ttyS";
char *portnum;
if(port == 1) portnum = "1";
else portnum = "0";
char *serial_name = strcat(portstring, portnum);
printf("Serial port: %s\n", serial_name);

unsigned char set_message[5] = {FLAG, A, C_SET, A^C_SET, FLAG};
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


//TRANSMITTER


if(mode == TRANSMITTER){

  retry_counter = 0;
  connected = 0;
  state = START;

  printf("Sending SET message...\n");

  /*
  set_message[0] = FLAG;
  set_message[1] = A;
  set_message[2] = C_SET;
  set_message[3] = set_message[1] ^ set_message[2];
  set_message[4] = FLAG;
  */

  res = write(fd, set_message, sizeof(set_message));
  printf("llopen:write: %d bytes written\n", res);

	  while(!connected){
		if(state != END){
			printf("fd: %d | byte: %02x | sizeofbyte: %lu\n", fd, byte, sizeof(byte));
			if(read(fd, &byte, sizeof(byte)) == 0){
				printf("Nothing read from UA.\n");
			}
			printf("Current byte being proccessed: %02x\n", byte);
		}

    printf("Received State: %d\n", state);

    switch(state){

      case START:
      if(byte == FLAG){
        state = FLAG_RCV;
        printf("UA First FLAG processed successfully: %02x\n", byte);
      }
      else { state = START; printf("UA START if 1\n"); }
      break;

      case FLAG_RCV:
      if(byte == A){
        state = A_RCV;
        printf("UA A processed successfully: %02x\n", byte);
      }
      else if(byte == FLAG) state = FLAG_RCV;
      else state = START;
      break;

      case A_RCV:
      if(byte == UA){
        state = UA_RCV;
        printf("UA C_SET processed successfully: %02x\n", byte);
      }
      else if(byte == FLAG) state = FLAG_RCV;
      else state = START;
      break;

      case UA_RCV:
      if(byte == (A ^ UA)){
        state = BCC_OK;
        printf("UA UA_RCV processed successfully: %02x\n", byte);
      }
      else if(byte == FLAG) state = START;
      else state = START;
      break;

      case BCC_OK:
      if(byte == FLAG){
        state = END;
        printf("UA Last FLAG processed successfully: %02x\n", byte);
      }
      break;

      case END:
      printf("UA processed successfully.\n");
      connected = TRUE;
      break;

      default:
      printf("You shouldnt be here. Leave.\n");
      break;
    }

  }
}



//RECEIVER

else{

  state = START;
  STOP = FALSE;

  /* state machine for SET message processing */
  int j;
  for(j=0; j<5; j++){
    printf("START SET[%d]: %02x\n", j, set_message[j]);
  }

  while(!STOP){

    if(state != END){
      res = read(fd, &byte, sizeof(byte));
      printf("Current byte being proccessed: %02x\n", byte);
      printf("SET RECEIVE FD: %d\n", res);
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
      printf("\nProcessing C_RCV\n");
      printf("set_message[1]: %02x | set_message[2]: %02x\n", set_message[1], set_message[2]);
      printf("Byte: %02x | SET: %02x\n", byte, set_message[3]);
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

  /*
  if(write(fd, ua_message, sizeof(ua_message)) == 0){
  printf("ERROR:llopen: failed to send UA message.\n");
  exit(-1);
}
*/

int k;
for(k=0; k < 5; k++){
  printf("UA[%d]: %02x\n", k, ua_message[k]);
}

int wfd;
wfd = write(fd, ua_message, sizeof(ua_message));
printf("UA Write FD: %d\n", wfd);

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

  char* i_frame = malloc(3);

  i_frame[0] = 0x77; //D
  i_frame[1] = 0x7d; //DATA
  i_frame[2] = 0x55; //Dn


	unsigned char BCC2 = 0;
	int h;
	for(h=0; h < strlen(i_frame); h++){
		BCC2 ^= i_frame[h];
	}
	printf("\n\nBCC2: %02x\n\n", BCC2);

  printf("\nI-Frame pre-stuffing: %lu\n", strlen(i_frame));

  for(h=0; h < strlen(i_frame); h++){
    printf("Frame[%d]: %02x\n",h, i_frame[h]);
  }

  char* new_frame = stuffing(i_frame, strlen(i_frame));

  printf("\nI-Frame post-stuffing: %lu\n", strlen(new_frame));
  for(h=0; h < strlen(new_frame); h++){
    printf("Frame[%d]: %02x\n",h, new_frame[h]);
  }

  char* frame_to_send = malloc(6 + strlen(new_frame));
	//BCC2 = new_frame[0] ^ new_frame[1] ^ new_frame[2];

  frame_to_send[0] = FLAG;
  frame_to_send[1] = A;
  frame_to_send[2] = C1;
  frame_to_send[3] = C1^A; // BCC1
	frame_to_send[8] = BCC2;
  frame_to_send[9] = FLAG;

  memmove(frame_to_send+4, new_frame, strlen(new_frame));

/*
	BCC2 = frame_to_send[2];
	for( = 3;  < strlen(frame_to_send) - 1; ++){
		BCC2 ^= frame_to_send[];
	}

	frame_to_send[8] = BCC2;
*/

  printf("\nI-Frame to be sent to llread: %lu\n", strlen(frame_to_send));
  for( h=0; h < 10; h++){
    printf("Frame[%d]: %02x\n",h, frame_to_send[h]);
  }

	//send i_frame to llread
		sleep(1);
	  int ret = write(fd, frame_to_send, strlen(frame_to_send));
	  printf("llwrite:write: %d bytes written\n", ret);

  return 0;
}


int llread(int fd, char *buffer){

	printf("\nReading I Frame...\n");

	state = START;
	STOP = FALSE;

	char *buff = malloc(3000);
	unsigned char byte;
	unsigned int size = 0;
	// = calculateBCC2(buffer, sizeof(buffer));

	while(!STOP){

    if(state != END){
/*
		  int test;
		  test = read(fd, &buffer, sizeof(buffer));
			printf("\n\nLLREAD: READ RESULT: %d\n\n", test);
			}
*/
			if(read(fd, &byte, sizeof(byte)) == 0){
				printf("Error: Nothing read from llread.\n");
				exit(-1);
			}
      printf("Current byte being proccessed: %02x\n", byte);
		}
	    switch(state){

	      case START:
	      if(byte == FLAG){
	        state = FLAG_RCV;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
	        printf("First FLAG processed successfully: %02x\n", byte);
	      }
	      else { state = START; printf("START if 1\n"); }
	      break;

	      case FLAG_RCV:
	      if(byte == A) {
	        state = A_RCV;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
	        printf("A processed successfully: %02x\n", byte);
	      }
	      else if(byte == FLAG){ state = FLAG_RCV; printf("FLAG_RCV if 1\n"); }
	      else{ state = START; printf("FLAG_RCV if 2\n"); }
	      break;

	      case A_RCV:
	      if(byte == C1) {
	        state = C1_RCV;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
	        printf("C1 processed successfully: %02x\n", byte);
	      }
	      else if(byte == FLAG){ state = FLAG_RCV; printf("A_RCV if 1\n"); }
	      else{ state = START; printf("A_RCV if 2\n"); }
	      break;

	      case C1_RCV:
	      printf("Processing C1_RCV\n");
	      if(byte == (C1^A)){
	        state = BCC1_OK;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
	        printf("BCC1 processed successfully: %02x\n", byte);
	      }
	      else if(byte == FLAG){ state = FLAG_RCV; printf("\nC1_RCV if 1\n"); }
	      else { state = START; printf("\nC1_RCV if 2\n"); }
	      break;

	      case BCC1_OK:
				printf("Processing BCC1_OK\n");
	      if(byte == FLAG){
	        state = END;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
	        printf("BCC1_OK processing failure: %02x\n", byte);
	      }
	      else {
					state = DATA_PROCESSING;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
				  printf("Starting to proccess Data from I Frame...\n");
				}
	      break;

				case DATA_PROCESSING:
				if(byte == FLAG){
					state = END;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
					printf("Finished processing Data: %02x\n", byte);
				}
				else {
					state = DATA_PROCESSING;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
					//printf("Expected BCC2: %02x\n", BCC2);
					printf("Processing data...\n");
				}
				break;

				case BCC2_OK:
				if(byte == FLAG){
					state = END;
					buff[size] = byte;
					printf("[%d]th element of buff: %02x\n", size, buff[size]);
					size++;
					printf("BCC2 processed successfully: %02x\n", byte);
				}
				else{ state = START; printf("Failed BCC2 processing: %02x\n", byte); }
				break;

	      case END:
	      printf("Reached end of I Frame Processing State Machine\n");
	      STOP = TRUE;
	      break;
			}
		}

	int k;
	printf("Buff(size = %d) values pre-destuffing:\n", size);
	for(k=0; k < strlen(buff); k++){
		printf("buff[%d]: %02x\n", k, buff[k]);
	}

	unsigned int newsize = deStuffing(buff, strlen(buff));

	printf("\nBuff(newsize = %d) values post-destuffing:\n", newsize);
	for(k=0; k < strlen(buff); k++){
		printf("buff[%d]: %02x\n", k, buff[k]);
	}

//Send RR confirmation packet



  return 0;
}


int llclose(int fd){
  return 0;
}

char* stuffing(char * package, int length){


  int i, new_length = length;
  for(i = 0; i < length; i++){
    if(package[i] == FLAG || package[i] == ESC)
      new_length++;
  }

  char* buf = malloc(new_length);
  memcpy(buf,package,length);

  for(i = 0; i < new_length; i++)
  switch(buf[i]){
    case FLAG:
    memmove(
      buf+i+2,
      buf+i+1,
      new_length - i
    );
    memcpy(buf+i++, &FLAG_SUB, sizeof(FLAG_SUB));
    break;

    case ESC:
    memmove(
      buf+i+2,
      buf+i+1,
      new_length - i
    );
    memcpy(buf+i++, &ESC_SUB, sizeof(ESC_SUB));
    break;
  }

  return buf;
  /*
  int size = length;
  int i;
  for(i = 0; i < length;i++)
  {
  char oct = package[i]; //oct means byte
  if(oct == FLAG || oct == ESC){
  size++;
}
}
if(size == length) //same size no need to stuff
return size;

for(i = 0; i < size; i++)
{
char oct = package[i];
if(oct == FLAG || oct == ESC)
{
memmove(package + i + 2, package + i+1, size - i); //moving everything to the front
if (oct == FLAG) {
package[i+1] = XOR_7E_20;
package[i] = ESC ;
}
else package[i+1] = XOR_7D_20;
}
}
return size; //return the new size of the package*/
}

int deStuffing(char * package, int length){

  int size = length;
  int i;
  for(i = 0; i < size; i++)
  {
    char oct = package[i]; //oct means byte
    if(oct == ESC)
    {
      if(package[i+1] == XOR_7E_20){
        package[i] = FLAG; //7E means that was a previous byte flag in there
        memmove(package + i + 1, package + i + 2,length - i + 2);

      }
      else if(package[i+1] == XOR_7D_20){
        memmove(package + i + 1, package + i + 2,length - i + 2); //was a previous ESC in there, just need to remove
      }
      size--;
    }
  }
  return size; //return the new size of the package
}

unsigned char calculateBCC2(unsigned char* buffer, unsigned int size) {
	unsigned char BCC2 = 0;

	int i = 0;
	for (i = 0; i < size; i++)
		BCC2 ^= buffer[i];

	return BCC2;
}
