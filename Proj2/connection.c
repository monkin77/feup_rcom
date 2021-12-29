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


int getResponse(int socketFd, char* response) {
    char c;
    ResponseState state = GET_STATUS_CODE;

    while(1) {
        switch(state) {

            case GET_STATUS_CODE: {
                read(socketFd, response, 3);
                printf("%s\n", response);
                state = IS_MULTI_LINE;
                break;
            }

            case IS_MULTI_LINE: {
                read(socketFd, &c, 1);
                printf("%c", c);

                if (c == MULTI_LINE_SYMBOL) {
                    state = READ_MULTI_LINE;
                } 
                else {
                    state = READ_LINE;
                }

                break;
            }

            case READ_MULTI_LINE: {

                int idxCounter = 0;
                char str[4];
                str[3] = '\0';

                while( read(socketFd, &c, 1) ) {
                    printf("%c", c);
                    if (c == '\n') break;
                    
                    if (idxCounter <= 2) 
                        str[idxCounter] = c;

                    if (idxCounter == 3 && strcmp(str, response) == 0 && c == LAST_LINE_SYMBOL) {
                        state = READ_LINE;
                        break;
                    }   

                    idxCounter++;
                }

                break;
            }
            case READ_LINE: {
                while( read(socketFd, &c, 1) ) {
                    printf("%c", c);
                    if (c == '\n') 
                        break; 
                }

                return 0;
            }

            default:
                break;
        }
    }
}

int sendCommand(int sockfd, char* cmd, char* argument) {
    size_t cmd_len = strlen(cmd);
    size_t arg_len = strlen(argument);

    if (write(sockfd, cmd, cmd_len) != cmd_len) {
        fprintf(stderr, "Error while sending command\n");
        return -1;
    }

    char c = ' ';
    if (write(sockfd, &c, 1) != 1) {
        fprintf(stderr, "Error while sending command\n");
        return -1;
    }

    if (write(sockfd, argument, arg_len) != arg_len) {
        fprintf(stderr, "Error while sending command argument\n");
        return -1;
    }

    return 0;
}
