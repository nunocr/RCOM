#include "application_layer.h"
#include "link_layer.h"

//static int mode = 0; //different modes for different data
//static int PACK_SIZE = 0;
//static int TRAMA_SIZE = 0;
static int DEBUG_FLAG = 0;
static int numRetries = 0;
unsigned char packNum = 0;

int transmitter(char * fileName, int fd) //envio da trama com SET
{
  FILE * file = NULL;
  //openfile
   int fileSize = 0;
  DEBUG_FLAG = open_file(&file, fileName);
  if(DEBUG_FLAG != 0)
    return -1;
  unsigned long size = file_size(file, &fileSize);

  //criar uma package com SET
  //receiver envia mensagem!
  //depois avançar
  //enviar mensagem com o START_PACK (2) END_PACK(3)
  char * packStart = malloc(1024);
  int packageSize = create_start_end_package(START_PACK, fileName, size, packStart);
  llwrite(fd, packStart, packageSize);      //int fd, char *buffer, int len);
  free(packStart);

  //SEND FILE
  char *data = malloc(1024);
  //char count = get_sequence_number(); // TODO::controloooo
  char packCount = 0;
  int bytesWritten = 0;
  int writeInt = 0;
  int aux = 0;
  int res = 0;
  int bytesRead = 0;
  while(bytesWritten < size && numRetries < MAXRETRIES)
  {
    if(aux == 0)
    {
      res = fread(data, 1, PACK_SIZE, file);
      printf("res: %d\n", res);
      bytesRead = res;

      res = create_data_package(data, res, packCount);
      packCount++;
      packCount %= 255; //caso ultrupasse os 255bytes disponíveis do count
    }
    writeInt = llwrite(fd, data, res);
    if(writeInt == 0)
    {
      bytesWritten += bytesRead;
      aux = 0;
      //count ^= 1; aqui incrementava-se
    } else if(writeInt != 0){
      numRetries++;
      printf("CONECTION LOST\n");
      aux = 1;

    }
  }
  if(numRetries == 3)
  {
    numRetries = 0;
    free(data);
    exit(1);
  }
  numRetries = 0;
  printf("ENVIADO TUDO");
  free(data);


  //FINALIZAR
  char * end = malloc(1024);
  packageSize = create_start_end_package(END_PACK, fileName, size, end);

  if(llwrite(fd, end, packageSize) != 0)
  {
    printf("Unable to send END PACKAGE\n" );
    free(end);
    exit(1);
  }
  free(end);

  //ENVIAR O DISC E FECHAR
  if(file != NULL)
  fclose(file);
  else{
      printf("File NULL\n");
  }

  return 0;
}

int receiver(int fd){
  //RECEIVER

  //receive START signal
  char *start = malloc(1024);
  int startSize = llread(fd, start);
  int fileSize;
  char *name = malloc(1024);
  if( get_file_info(start, startSize, &fileSize, name) == -1)
  {
    printf("Error reading start package\n");
    exit(1);
  }
  printf("FILEZISE: %x\n", fileSize);
  FILE *file = NULL;

  if(create_file(&file, name) != 0)
    printf("not opened file\n");
  printf("opened file\n");
  free(name);

  //receber ficheiro e gravar
  int bytesRead = 0;
  char * buffer = malloc(1024);
  int checkRead = 0;
  while(bytesRead<fileSize)
  {
    int size;
    checkRead = llread(fd, buffer);
    if(checkRead == -1)
    {
      printf("Retrying Reading the same package\n");
    } else {
      size = get_data(buffer, size);
      printf("Size:%d --- %d\n",bytesRead, size);
      if(size!= -1 && getRR() == 0)
      {
        fwrite(buffer, 1, size, file);
        bytesRead += size;
      }
      else if(size != 0 && getRR() == 1)
      {
        fseek(file,bytesRead, SEEK_SET);
        fwrite(buffer, 1, size, file);
        bytesRead += size;
        setRR();
      }
      else{ printf("Size is negative'-'\n");}
    }
  }
  free(buffer);
  if(bytesRead != fileSize)
    printf("Wrong Number Bytes\n");
  printf("Enviado Direito\n");

  //Finalizar
  char *end = malloc(1024);
  int endSize = llread(fd, end);
  name = malloc(1);
  if(get_file_info(end, endSize, &fileSize, name) == -1)
  {
	   printf("Error reading end package\n");
     exit(1);
  }
  printf("\nEnd Read name: %s - size: %d \n", name, fileSize);
  free(name);

return 0;
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

unsigned long file_size(FILE * file,  int * fileSize)
{
  unsigned long size = 0;
  int fd = fileno(file);
  lseek(fd, 0L, SEEK_END); // seek to end of file

  size = ( int) ftell(file);   // get current file pointer
  lseek(fd, 0L, SEEK_SET); // seek back to beginning of file
                             // proceed with allocating memory and reading the file
  if(size <= 0)
  {
    printf("File size is 0 or less");
    return -1;
  }
  return size;
}

int create_start_end_package(int type, char * fileName,  int size, char * package)
{
  //size = 10968
  int nameLength = strlen(fileName);
  int sizeLength = check_num_bytes(size);

  int packSize = 5 + nameLength + sizeLength;
  //package = realloc(package, packSize);

  package[0] = type;
  package[1] = TSIZE;
  package[2] = (char) sizeLength;

  int i = 3;
  while(size != 0){
      package[i] = (unsigned char) size;
      i++;
      size >>=8;
  } //size is written backwards

  package[i] = TNAME;
  ++i;
  package[i] = (char) nameLength;
  ++i;

  int j;
  for(j = 0; j < nameLength; i++, j++)
    package[i] = fileName[j];

  return packSize;
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

int get_file_info(char* buffer, int buffsize,  int *size, char *name)
{
  printf("%d\n", buffsize);
  int fileSize = 0;
  int i=0; //START
  if(buffer[i] != START_PACK && buffer[i] != END_PACK)
  {
    printf("%x\n", buffer[i]);
    printf("get file info: buffer error\n");
    return -1;
  }
  i++; //TSIZE
  if(buffer[i] != TSIZE)
  {
    printf("get file info: tsize\n");
    return -1;
  }

  i++; //numBytesSize - T
  int sizeLength = (int) buffer[i];
  i++; //Numero de Bytes do ficheiro - L
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
  printf("Tamanho do ficheiro %d\n", *size);

  //i++; //TNAME
  printf("buffer[%d]=%02x\n", i, buffer[i]);

  if(buffer[i] != TNAME)
    return -1;
  i++; //NAME size
  int nameLength = buffer[i];
  printf("name length: %02x\n", buffer[i]);
  i++; //NAME data
  //name = realloc(name, nameLength);
  printf("Nome do ficheiro:\n");
  for(j=0; j<nameLength; j++, i++)
  {
    name[j] = buffer[i];
    printf("%d", name[j]);
  }
  name[j] = '\0';

  return 0;
}

int create_data_package(char *buffer, int size, char packageID)
{
  int length = size + 4;
  char * copy = malloc(size); //buffer para adicionar depois
  memcpy(copy, buffer, size);
//  buffer = realloc(buffer, length);
  buffer[0] = DATA_PACK; // 1
  buffer[1] = (unsigned char) packageID; //num sequencia
  buffer[2] = (unsigned char) (size / 256); //num bytes
  buffer[3] = (unsigned char) (size % 256); //restantes bytes

  memcpy(buffer+4, copy, size);

  free (copy);

  return length;
}

int get_data (char * buffer, int size)
{
  printf("%d\n", size);
  if(buffer[0] != DATA_PACK)
    return -1;
  if((unsigned char) buffer[1] != packNum)
    return -1;

  packNum = (packNum +1) % 255; //incrementar e ficar no máximo em 255bytes

  unsigned char l1 = (unsigned char) buffer[2];
  unsigned char l2 = (unsigned char) buffer[3];
  int length = (int) (256*l1 + l2); //nos slides, numBytes
  char *copy = malloc(length);

  memcpy(copy, buffer + 4, length);
  memcpy(buffer, copy, length);

  free(copy);
  return length;
}
