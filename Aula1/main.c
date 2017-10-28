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

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("ERROR: Wrong number of arguments.\n");
		exit(0);
	}

int descriptor;
char *buffer = malloc(sizeof(*buffer));

if(strcmp(argv[2], "TRANSMITTER") == 0){

	descriptor = llopen((*argv[1])-'0', 0);
	printf("\nmain.c: Transmitter: descriptor after llopen: %d\n", descriptor);
	llwrite(descriptor, buffer, sizeof(buffer));
  printf("\nmain.c: Transmitter: descriptor after llwrite: %d\n", descriptor);
	free(buffer);

}

else if(strcmp(argv[2], "RECEIVER") == 0) {

	int newdesc;
	char *newbuf = malloc(sizeof(*newbuf));

	//descriptor = llopen((*argv[1])-'0', 1);
	newdesc = llopen((*argv[1])-'0', 1);
	printf("\nmain.c: Receiver: descriptor after llopen: %d\n", newdesc);
	//if(llread(descriptor, buffer) == 0){
	if(llread(newdesc, newbuf) == 0){
		printf("\nError: main.c: Receiver: Nothing to read from llread.\n");
	}
	printf("\nmain.c: Receiver: descriptor after llread: %d\n", newdesc);
	free(newbuf);

}
else{
	printf("\nERROR: Invalid argument provided: %s\n", argv[2]);
	exit(-1);
}

//llopen((*argv[1])-'0', argv[2]);

	return 0;
}
