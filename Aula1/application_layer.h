#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TO_READ "r"
#define TO_WRITE "w"

#define START_PACK 2
#define END_PACK 3
#define TSIZE 0
#define TNAME 1

int transmitter(char * fileName, int fd);
int receiver(int fd);
int open_file(FILE ** file, char * fileName);
int file_size(FILE * file,  int * fileSize);
int create_start_package(int mode, char * fileName,  int size, char * package);
int check_num_bytes(int size);
int get_file_info(char* buffer, int buffsize,  int *size, char *name);
int create_file(FILE ** file, char * fileName);
