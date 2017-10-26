#ifndef LINK_LAYER_H
#define LINK_LAYER_H

typedef enum {TRANSMITTER, RECEIVER} UserMode;

int llopen(int port, char mode);
int llwrite(int fd, char *buffer, int len);
int llread(int fd, char *buffer);
int llclose(int fd);
int stuffing(char * package, int length);
int deStuffing(char * package, int length);

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define TRANSMITTER 0
#define RECEIVER 1

#define RETRY_NUM 4

#define FLAG  0x7e
#define A     0x03
#define C_SET 0x03
#define UA 	  0x07

/* State Machine */
#define SET_SEND 0
#define START    1
#define FLAG_RCV 2
#define A_RCV 	 3
#define C_RCV 	 4
#define BCC_OK 	 5
#define END 	 6
#define UA_RCV 	 7

/* I frame */
#define ESC 	  0x7D
#define SUB 	  0x20
#define XOR_7E_20 0x5E
#define XOR_7D_20 0x5D


#endif
