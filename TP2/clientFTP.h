#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

typedef struct ftp {
  int fd_socket; // o file descriptor do control socket
  int fd_data; // o  file descriptor do data socket
} ftp;

typedef struct URL {
	url_content user; // string to user
	url_content password; // string to password
	url_content host; // string to host
	url_content ip; // string to IP
	url_content path; // string to path
	url_content filename; // string to filename
	int port; // integer to port
} url;


int parseLink(char *link, char *host, char *path, char *file);
static int connectSocket(const char * IP,int PORT);
int FTPdownload(char *path, char *filename, ftp *ftp);
int FTPconnect(ftp *FTP, char *ip, int port);
int FTPsend(ftp *FTP, char *msg, int size);
int FTPread(ftp *FTP, char *msg, unsigned int size);
int FTPdisconnect(ftp * FTP);
int FTPpasv(ftp * FTP);
