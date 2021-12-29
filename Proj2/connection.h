#ifndef CONNECTION_H
#define CONNECTION_H

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct hostent* getip(char hostname[]);

#endif
