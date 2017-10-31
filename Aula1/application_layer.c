#include "application_layer.h"
#include "link_layer.h"

//static int mode = 0; //different modes for different data
//static int PACK_SIZE = 0;
//static int TRAMA_SIZE = 0;
static int DEBUG_FLAG = 0;

int transmitter(char * fileName, int fd) //envio da trama com SET
{
  FILE * file = NULL;
  //openfile
  int fileSize = 0;
  DEBUG_FLAG = open_file(&file, fileName);
  if(DEBUG_FLAG != 0)
    return -1;

  DEBUG_FLAG = file_size(file, &fileSize);
  if(DEBUG_FLAG != 0)
    return -1;

  //criar uma package com SET TODO::provavelmente colocar no llopen!!!
  /*char* set = malloc(5 * sizeof(char));

	set[0] = FLAG;
	set[1] = A;
	set[2] = C_SET;
	set[3] = A ^ C_SET;
  set[4] = FLAG;*/

  //enviar o pacote para receber confirmação que pode começar a enviar os dados
  /*int res  = 0;
	while(res <= 0)
  {
		res=write(fd, message, length);
  }

  free(msg);*/

  //receiver envia mensagem! TODO::colocar o receiver a enviar um pacote
  //depois avançar
  //enviar mensagem com o START_PACK (2) END_PACK(3) DATA_PACK(1) TODO::fazer defines disto
  char * packStart = malloc(1024);
  int packageSize = create_start_package(START_PACK, fileName, fileSize, packStart);
  llwrite(fd, packStart, packageSize);      //int fd, char *buffer, int len);
  free(packStart);

  return 0;
}

int receiver(int fd){
  //RECEIVER

  //receive START signal
  char *start = malloc(1024);
  int startSize = llread(0, start);
  int fileSize;
  char *name = malloc(1024);
  if( getFileInfo(start, startSize, &fileSize, name) == -1)
  {
    printf("Error reading start package\n");
    exit(1);
  }

  FILE *file = NULL;
  if(create_file(&file, name) != 0)
    printf("opened file\n");

free(name);
}

int open_file(FILE ** file, char * fileName)
{
  *file = fopen(fileName, "r+");
  if(*file == NULL )
  {
    printf("File does not exist");
    return -1;
  }

  return 0;
}

int create_file(FILE ** file, char * fileName)
{
  *file = fopen(fileName, "w+");
  if(*file == NULL )
  {
    printf("File does not exist");
    return -1;
  }

  return 0;
}

int file_size(FILE * file, int * fileSize)
{
  fseek(file, 0, SEEK_END); // seek to end of file

  *fileSize = (unsigned int) ftell(file);   // get current file pointer
  fseek(file, 0, SEEK_SET); // seek back to beginning of file
                             // proceed with allocating memory and reading the file
  if(fileSize <= 0)
  {
    printf("File size is 0 or less");
    return -1;
  }
  return 0;
}

int create_start_package(int mode, char * fileName, int size, char * package)
{
  //size = 10968
  int numBytesSize = check_num_bytes(size);
  int nameSize = strlen(fileName);
  package[0] = mode;
  package[1] = TSIZE; //TSIZE = 0,tamanho do ficheiro, TNAME = 1, tamanho do nome
  package[2] = (char) numBytesSize ; //dividir o size
  int i;
  for(i=3; i<=numBytesSize;i++)
  {
    package[i] = (unsigned char) size;
    size >>= 8;
  }
  i++;
  package[i] = 1; //TNAME
  i++;
  package[i] = (char) nameSize;

  int j;
  for(j=0; j<nameSize; i++, j++)
  {
    package[i] = fileName[j];
  }

  return i;
}

int check_num_bytes(int size)
{
  int count = 0;
  while(size != 0)
  {
    size >>= 8;
    count++;
  }
  return count;
}

int getFileInfo(char* buffer, int buffsize, int *size, char *name)
{
  int fileSize = 0;
  int i=0; //START
  if(buffer[i] != START_PACK && buffer[i] != END_PACK)
    return -1;
  i++; //TSIZE
  if(buffer[i] != TSIZE)
    return -1;
  i++; //numBytesSize
  int sizeLength = (int) buffer[i];
  i++; //Numero de Bytes do ficheiro
  int j;
  for(j=0; j< sizeLength; j++, i++)
  {
    int k;
    unsigned char ch = (unsigned char) buffer[i];
    unsigned int curr = (unsigned int) ch;
    for(k = 0; k < j; k++)
        curr = curr << 8;
    fileSize += curr;
  }
  *size = fileSize;

  i++; //TNAME
  if(buffer[i] != TNAME)
    return -1;
  i++; //NAME size
  int nameLenght = buffer[i];
  i++; //NAME data
  name = realloc(name, nameLength);
  for(j=0; j<nameLength; j++, i++)
  {
    name[j] = buffer[i];
  }
  name[j] = '\0';
  return 0;
}
