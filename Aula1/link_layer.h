#ifndef LINK_LAYER_H
#define LINK_LAYER_H

typedef enum Modes {TRANSMITTER, RECEIVER} UserMode;

int llopen(int port, UserMode mode);
int llwrite(int fd, char *buffer, int len);
int llread(int fd, char *buffer);
int llclose(int fd);

#endif
