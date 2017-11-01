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
#include "application_layer.h"

int main(int argc, char** argv) {


if(strcmp(argv[2], "TRANSMITTER") == 0){
	if (argc != 4) {
		printf("ERROR: Wrong number of arguments.\n");
		exit(0);
	}
  int t_fd;
	char *t_buf = malloc(sizeof(*t_buf));

	t_fd = llopen((*argv[1])-'0', 0);
	printf("\nmain.c: Transmitter: descriptor after llopen: %d\n", t_fd);
  transmitter(argv[3], t_fd); //nome do ficheiro
  llclose(t_fd, 0); //0 significa o transmitter

}

else if(strcmp(argv[2], "RECEIVER") == 0) {

	int r_fd;
	char *r_buf = malloc(sizeof(*r_buf));


	r_fd = llopen((*argv[1])-'0', 1);
	receiver(r_fd);
	printf("\nmain.c: Receiver: descriptor after llopen: %d\n", r_fd);
	/*
	if(llread(r_fd, r_buf) == 0){
		printf("\nError: main.c: Receiver: Nothing to read from llread.\n");
	}
	*/
	free(r_buf);
	llclose(r_fd, 1); //1 é o RECEIVER

}

else{
	printf("\nERROR: Invalid argument provided: %s\n", argv[2]);
	exit(-1);
}

	return 0;
}
