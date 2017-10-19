#ifndef LINK_LAYER_H
#define LINK_LAYER_H

typedef enum {TRANSMITTER, RECEIVER} UserMode;

int llopen(int port, char* mode);
int llwrite(int fd, char *buffer, int len);
int llread(int fd, char *buffer);
int llclose(int fd);

/* State Machine */
#define SET_SEND 0
#define START 1
#define FLAG_RCV 2
#define A_RCV 3
#define C_RCV 4
#define BCC_OK 5
#define END 6

#endif
