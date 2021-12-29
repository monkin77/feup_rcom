#include "connection.h"

struct hostent* getip(char hostname[]) {
    printf("Getting IP Address from Host Name...\n");
    struct hostent *h;

    if ((h = gethostbyname(hostname)) == NULL) {
        herror("Failed to get host by name");
        exit(1);
    }

    return h;
}

int connectSocket(char *addr) {
    printf("Connecting to Server Socket...\n");

    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr);    /* 32 bit Internet address network byte ordered */
    server_addr.sin_port = htons(SERVER_PORT);        /* server TCP port must be network byte ordered */
    

    /* open a TCP socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }

    /* connect to the server */
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    printf("Server Socket Connected\n");

    return sockfd;
}

