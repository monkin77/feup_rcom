#ifndef PARSE_H
#define PARSE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PROTOCOL_SIZE 6
#define MAX_USER_SIZE 256
#define MAX_PWD_SIZE 256
#define MAX_HOST_SIZE 512
#define MAX_PATH_SIZE 1024
#define MAX_RESPONSE_SIZE 4096

typedef enum InputState
{
  PROTOCOL,
  USER,
  PASS,
  HOST,
  PATH
} InputState;

int parseInput(char *input, char *user, char *pass, char *host, char *path);

int parsePort(char *response, int *port);
int getStartHostIdx(char *input);

char *strrev(char *str);

void parseFileName(char *path, char *fileName);

/**
 * @brief 
 * 
 * @param text 
 * @return size_t = 0 if error 
 */
size_t parseFileSize(char *text);

#endif
