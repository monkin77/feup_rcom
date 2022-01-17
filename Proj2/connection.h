#ifndef CONNECTION_H
#define CONNECTION_H

#include "parse.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT 21
#define MULTI_LINE_SYMBOL '-'
#define LAST_LINE_SYMBOL ' '
#define CARRIAGE_RETURN 13

#define PSV_RESPONSE_MAXSIZE 1000
#define FILE_BUFFER_SIZE 256

typedef enum ResponseState
{
  GET_STATUS_CODE,
  IS_MULTI_LINE,
  READ_MULTI_LINE,
  READ_LINE
} ResponseState;

struct hostent *getip(char hostname[]);

int connectSocket(char *addr, int port);

int getResponse(int socketFd, char *code, char *text);

int sendCommand(int sockfd, char *cmd, char *argument);

int login(int sockfd, char *user, char *pass);

int getPort(int sockfd, int *port);

int downloadFile(int sockfd, int downloadFd, char *path);

int saveFile(int downloadFd, char *filename, size_t fileSize);

/**
* sends a command and interprets its reply
*/
int handleCommand(int sockfd, char *cmd, char *argument, char *text);

#endif
