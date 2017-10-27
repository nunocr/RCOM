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

/*
	if((strcmp(argv[2], "TRANSMITTER") == 0) && strcmp(argv[2], "RECEIVER") == 0){
		printf("ERROR: Invalid argument provided: %s", argv[2]);
		exit(-1);
	}

if(strcmp(argv[2], "TRANSMITTER") == 0){
	llopen((*argv[1])-'0', TRANSMITTER);
}
else{
	llopen((*argv[1])-'0', RECEIVER);
}
*/

if(strcmp(argv[2], "TRANSMITTER") == 0){
	int descriptor;
	char *buffer = malloc(sizeof(*buffer));

	descriptor = llopen((*argv[1])-'0', 0);
	llwrite(descriptor, buffer, sizeof(buffer));

	free(buffer);

}
else if(strcmp(argv[2], "RECEIVER") == 0) {
	llopen((*argv[1])-'0', 1);
}
else{
	printf("ERROR: Invalid argument provided: %s\n", argv[2]);
	exit(-1);
}

//llopen((*argv[1])-'0', argv[2]);

	return 0;
}
