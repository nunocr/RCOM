#include "application_layer.h"
#include "link_layer.h"

//static int mode = 0; //different modes for different data
//static int PACK_SIZE = 0;
//static int TRAMA_SIZE = 0;
static int DEBUG_FLAG = 0;

int mainn(int argc, char** argv)
{
  int mode = 3;
  if( argc == 3 && (strcmp("RECEIVER",argv[2]) == 0))
      mode = 0;
  else if( argc == 4  && (strcmp("TRANSMITTER",argv[2]) == 0))
      mode = 1;
  else {
      printf("Wrong number of arguments \n");
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS0  RECEIVER\n");
      printf("      \tex: nserial /dev/ttyS0  TRANSMITTER  FilePath \n");
      exit(1);
  }

  if ((strcmp("/dev/ttyS0", argv[1])!=0) &&  (strcmp("/dev/ttyS1", argv[1])!=0) )
  {
      printf("Wrong seraial port: choose /dev/ttyS0 or /dev/ttyS1 \n");
      exit(1);
  }

  if(llopen((*argv[1])-'0', mode) != 0)
  {
    exit(1);
  }

  if(mode == TRANSMITTER)
        transmitter(argv[3]); //nome do ficheiro
    else if (mode == RECEIVER)
        exit(1);
        //receiver();

    llclose(mode);
    //mandar as estatisticas da transmissao
return 0;
}

int transmitter(char * fileName) //envio da trama com SET
{
  FILE * file = NULL;
  //openfile
  int fileSize = 0;
  DEBUG_FLAG = open_file(file, fileName);
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
  char * packStart = malloc(1);
  create_start_package(1, fileName, fileSize, packStart);

  return 0;
}

int open_file(FILE * file, char * fileName)
{
  file = fopen(fileName, "r+");
  if(file == NULL )
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
  package[0] = mode;
  package[1] = 0; //TODO::TSIZE = 0,tamanho ficheiro, TNAME = 1, tamanho do nome
  package[2] = numBytesSize ; //dividir o size
  int i;
  for(i=3; i<numBytesSize;i++)
  {
    package[i] = (unsigned char) size;
    size >>= 8;
  }
  return 0;
}

int check_num_bytes(int size)
{
  int count = 0;
  while(size > 2)
  {
    size = size / 2;
    count++;
  }
  return count;
}
