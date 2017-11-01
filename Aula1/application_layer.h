#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TO_READ "r"
#define TO_WRITE "w"

#define DATA_PACK 1
#define START_PACK 2
#define END_PACK 3
#define TSIZE 0
#define TNAME 1

#define PACK_SIZE 600

int transmitter(char * fileName, int fd);
int receiver(int fd);
int open_file(FILE ** file, char * fileName);
unsigned long file_size(FILE * file,  int * fileSize);
int create_start_end_package(int type, char * fileName,  int size, char * package);
int check_num_bytes(int size);
int get_file_info(char* buffer, int buffsize,  int *size, char *name);
int create_file(FILE ** file, char * fileName);
int get_data (char * buffer, int size);
int create_data_package(char *buffer, int size, char packageID);
