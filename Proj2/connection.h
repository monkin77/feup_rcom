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

struct hostent* getip(char hostname[]);

int connectSocket(char *addr);

#endif
