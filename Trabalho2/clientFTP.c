#include "clientFTP.h"

int main(int argc, char **argv) {

//verificar o link

	if(argc != 2){
	printf("Error in clientFTP: wrong number of args");
	exit(1);	
	}

    char link[128];
    strcpy(link, argv[1]);

    char *ip = malloc(64);
    char *host = malloc(64);
    char *path = malloc(64);
    char *file = malloc(64);

	printf("Parsing link...\n");
    parseLink(link, host, path, file);

	printf("ip: %s\n", ip);
	printf("host: %s\n", host);
	printf("path: %s\n", path);
	printf("file: %s\n", file);

	printf("Getting IP...\n");
    getIP(host, ip);

    printf("IP: %s\n", ip);
    printf("host: %s\n", host);
    printf("path: %s\n", path);
    printf("file: %s\n", file);

    char * user;
    char * pass;
    
    user = malloc(sizeof(char)*64);
    pass = malloc(sizeof(char)*64);

	printf("Parsing name and Password...\n");
    parseNameAndPass(link,user,pass);
    
    ftp FTP;

   printf("FTP Connect\n");
   FTPconnect(&FTP, ip, 21);

   printf("FTP login\n");
   FTPlogin(&FTP, user, pass);

   printf("FTP pasv\n");
   FTPpasv(&FTP);

   printf("FTP download\n");
   FTPdownload(path, file, &FTP);

   printf("FTP Disconnect\n");
   FTPdisconnect(&FTP);

    free(ip);
    free(host);
    free(path);
    free(file);
	
	printf("End of program\n");
	return 0;
}

int FTPpasv(ftp * FTP){
    char msg[2048] ;

    sprintf(msg,"PASV\n");

    if(FTPsend(FTP,msg,strlen(msg))){
        printf("FAIL: unable to send PASV \n");
        return 1;
    }

//    printf("%s\n",msg);

    if(FTPread(FTP,msg,sizeof(msg))){

        printf("FAIL: unable to receive to enter in passive mode \n");
        return 1;
    }

    //printf("PASV resp:\n%s\n",msg);

    unsigned int ip1,ip2,ip3,ip4;
    int port1, port2;

    // 227 Entering Passive Mode (90,130,70,73,91,233).
    if(sscanf(msg,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",&ip1,&ip2,&ip3,&ip4,&port1,&port2) < 0){
        printf("FAIL: sscanf\n");
        return 1;
    }


    int port = port1*256 + port2;

    memset(msg, 0, sizeof(msg));

    if (sprintf(msg, "%d.%d.%d.%d", ip1, ip2, ip3, ip4) < 0) {
        printf("ERROR: ip address make error.\n");
        return 1;
    }

    //printf("IP %s \n", msg);
    //printf("PORT %d \n", port);

    if((FTP->fd_data = connectSocket(msg,port)) < 0){
        printf("FAIL: PASV connectSocket \n");
        return 1;
    }
    return 0;
}

int FTPlogin(ftp* FTP, char *user, char *pass) {

    char msg[2048];

    sprintf(msg,"USER %s\n",user);

    if(FTPsend(FTP,msg,strlen(msg))){
        printf("FAIL: Unable to send user name\n");
        return 1;
    }

    //printf("SEND  USER:   %s\n",msg);

    if(FTPread(FTP,msg,sizeof(msg))){
        printf("FAIL: Unable to get response \n");
        return 1;
    }

        //  printf("READ USER response:   %s\n",msg);

    memset(msg,0,sizeof(msg));

    sprintf(msg,"PASS %s\n",pass);
    if(FTPsend(FTP,msg,strlen(msg))){
        printf("FAIL: unable to send password \n");
        return 1;
    }
    //printf("SEND PASS: %s\n", pass);


    if(FTPread(FTP,msg,sizeof(msg))){
        printf("FAIL: unable to get response password \n");
        return 1;
    }

    //printf("%s\n",msg);
    return 0;
}

int parseLink(char *link, char *host, char *path, char *file) {
    char tmpHost[64];
    char tmpPath[64];
    char tmpFile[64];
    int i = 0;
    int dataF = 0;

    if(link[6] != '[') //finds if there is a user and a password on link
    {
        dataF = 2;
        link += 5;
    }

    while (*link != '\0') {
        if (*link == '[' || *link == ']')
            ++dataF;

        link++;
        if (dataF == 2) {
            if (*link == '/')
                break;

            tmpHost[i] = *link;
            i++;
        }
    }
    tmpHost[i] = '\0';

    i = 0;
    char *startPath = link;
    int last = 0;
    while(link[i] != '\0'){
        if(link[i] == '/')
            last = i;
        i++;
    }

    for(i = 0; i <= last; i++)
        tmpPath[i] = startPath[i];
    tmpPath[i] = '\0';

    int j = 0;
    while(link[i] != '\0'){
        tmpFile[j] = link[i];
        i++;
        j++;
    }
    tmpFile[j] = '\0';



    strcpy(host, tmpHost);
    strcpy(path, tmpPath);
    strcpy(file, tmpFile);
    if (dataF == 2)
        return 0;
    return 1;
}

int parseNameAndPass(char *link,char * user, char * pass){
    int len = strlen(link);
    int index = 0;
    while( index < len && link[index] != '[')
    {
        index++;
    } 

    char tempUser[64];
    char tempPass[64];

    index++;
    int i = 0;
    while(index < len && link[index] != ':' && i < 64)
    {
        tempUser[i] = link[index];
        index++;
        i++;
    }

    tempUser[i] = '\0';

    index++;
    i=0;
    while(index < len && link[index] != '@' && i < 64)
    {
        tempPass[i] = link[index];
        index++;
        i++;
    }

    tempPass[i] = '\0';
    
    strcpy(user,tempUser);
    strcpy(pass,tempPass);

    printf("%s \n",tempUser);
    printf("%s \n",tempPass);


    return 0;
} 

int FTPdownload(char *path, char *filename, ftp *ftp) {
    FILE *file;

    if (!(file = fopen(filename, "w"))) {
        printf("Unable to open file.\n");
        return 1;
    }
    int bytesRead;
    int bytesWrote;
    int totalBytesWrote = 0;

    char reqMsg[128];
    char response[128];
    sprintf(reqMsg,  "CWD %s\n", path);
    FTPsend(ftp, reqMsg, strlen(reqMsg));
    FTPread(ftp, response, sizeof(response));


    sprintf(reqMsg, "RETR %s\n", filename);
    FTPsend(ftp, reqMsg, strlen(reqMsg));
    FTPread(ftp, response, sizeof(response));

    char buf[2048]; // TODO  not sure what size should be

    while ((bytesRead = read(ftp->fd_data, buf, sizeof(buf)))) {
        if (bytesRead < 0) {
            printf("Fail: Nothing to read\n");
            return 1;
        }

        if (!(bytesWrote = fwrite(buf, bytesRead, 1, file))) {
            printf("Fail: writen in file\n");
            return 1;
        }

        totalBytesWrote += bytesWrote;
    }

    fclose(file);

    close(ftp->fd_data);

    return 0;
}

    // TODO FALTA FAZER VERIFICAÇOES e ver se recebe ( 1* -> abrir data socket???
    // how knows)
int FTPconnect(ftp *FTP, char *ip, int port) {

    int socket_fd;


	printf("FTP Connect: connectSocket with args:\n");
	printf("ip: %s\n", ip);
	printf("port: %d\n", port);
    if ((socket_fd = connectSocket(ip, port)) < 0) {
        printf("Fail: Socket Connect \n");
        return 1;
    };

    FTP->fd_socket = socket_fd;
    FTP->fd_data = 0;

    char msg[2048];
	printf("FTP Connect: FTPread\n");
    if(FTPread(FTP, msg, sizeof(msg)) != 0)
    {
        printf("error reading after connect\n");
        return -1;
    }
    //printf("Connect message: %s\n", msg);
    // 1*
    return 0;
}

// receber mesagem socket control
int FTPsend(ftp *FTP, char *msg, int size) {

    int bytes;
    printf("SEND: %s\n", msg);
    if ((bytes = write(FTP->fd_socket, msg, size)) <= 0) {
        printf("Warning mensage wasnt sent\n");
        return 1;
    }
    if (bytes != size) {
        printf("Warning the whole mensage wasn't sent \n");
        return 1;
    }
    return 0;
}

// maddar mensagem pro socket control
int FTPread(ftp *FTP, char *msg, unsigned int size) {
    FILE *sock = fdopen(FTP->fd_socket, "r");

    memset(msg,0,size);

    do{
        msg = fgets(msg, size, sock);
        printf("READ %s\n",msg );
    }while((!('1' <= msg[0] && msg[0] <= '5' ))|| msg[3] != ' ');


    return 0;
}


int FTPdisconnect(ftp * FTP) {

    char msg[2048];

    if(FTPread(FTP,msg,sizeof(msg))){
        printf("Fail: canno disconetc account\n");
        return 1;
    }

    sprintf(msg, "QUIT\r\n");

    if(FTPsend(FTP,msg,strlen(msg))){
        printf("Fail: cannnot send QUIT\n" );
        return 1;
    }


    close(FTP->fd_socket);

    return 0;

}

int connectSocket(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	
	server_addr.sin_addr.s_addr = inet_addr(ip); /*32 bit Internet address network byte ordered*/
	if(server_addr.sin_addr.s_addr == -1){
		printf("FTP Connect: connectSocket: inet_addr: error\n");
	}
	printf("inet_addr return: %d\n", server_addr.sin_addr.s_addr);

    server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */
	printf("htons return: %d\n", server_addr.sin_port);

    /*open an TCP socket*/
	printf("FTP Connect: connectSocket: socket\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(0);
    }

    /*connect to the server*/
	printf("FTP Connect: connectSocket: connect\n");
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0) {
        perror("connect()");
        exit(0);
    }

	printf("FTP Connect: connectSocket: end of function\n");
    return sockfd;
}

int getIP(char *link, char *ip)
{
	struct hostent *h;

/*
struct hostent {
	char    *h_name;	Official name of the host.
    	char    **h_aliases;	A NULL-terminated array of alternate names for the host.
	int     h_addrtype;	The type of address being returned; usually AF_INET.
    	int     h_length;	The length of the address in bytes.
	char    **h_addr_list;	A zero-terminated array of network addresses for the host.
				Host addresses are in Network Byte Order.
};

#define h_addr h_addr_list[0]	The first address in h_addr_list.
*/

//	if ((h = gethostbyname(link)) == NULL) {
	if((h = gethostbyname("193.137.29.15")) == NULL) {
		herror("gethostbyname");
		exit(1);
	}

	// printf("Host name  : %s\n", h->h_name);
	// printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

	char *tmpIP;
	tmpIP = inet_ntoa(*((struct in_addr *)h->h_addr));
	strcpy(ip, tmpIP);
	return 0;
}
