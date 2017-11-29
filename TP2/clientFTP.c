#include "clientFTP.h"
#include "getip.c"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Put: ./clientFTP ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

//verificar o link

    return 0;
}

static int connectSocket(const char *IP, int PORT) {
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP); /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(PORT); /*server TCP port must be network byte ordered */

    /*open an TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(0);
    }

    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0) {
        perror("connect()");
        exit(0);
    }

    return sockfd;
}
