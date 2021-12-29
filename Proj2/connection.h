#ifndef CONNECTION_H
#define CONNECTION_H

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 21
#define MULTI_LINE_SYMBOl '-'
#define LAST_LINE_SYMBOl ' '


typedef enum ResponseState {
  GET_STATUS_CODE,
  IS_MULTI_LINE,
  READ_MULTI_LINE,
  READ_LINE
} ResponseState;

struct hostent* getip(char hostname[]);

int connectSocket(char *addr);

int getResponse(int socketFd, char* response);

#endif
