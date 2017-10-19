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

	return 0;
}
