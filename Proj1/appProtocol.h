#ifndef APP_H
#define APP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#include "emissor.h"
#include "receptor.h"

#define FRAME_DATA_SIZE 200

// EMISSOR OR RECEPTOR
#define EMISSOR 0
#define RECEPTOR 1

// T BYTE 
#define T_FILE_SIZE 0
#define T_FILE_NAME 1

// PACKETS CONSTANTS
#define DATA_CTRL 1
#define START_CTRL 2
#define END_CTRL 3

// DATA PACKET CONSTANTS
#define NUM_DATA_ADDITIONAL_FIELDS 4
#define NUM_OCTETS_MULTIPLIER 256

#define min(a,b) (((a) < (b)) ? (a) : (b))

/**
*   @brief Opens connection with file
*
*   @param fileName 
*   @param status 0 -> Transmitter, 1 -> Receiver
*   @return file descriptor if OK, negative otherwise
*/
int llopen(char* fileName, int status);


/**
*   @brief Closes connection with file
*
*   @param fd -> File descriptor of the file 
*   @param status 0 -> Transmitter, 1 -> Receiver
*/
int llclose(int fd, int status);


/**
* @brief Builds data packet, from a given data buffer
*/
void buildDataPacket(u_int8_t* dataPacket, int dataPacketSize, u_int8_t* frameData, int frameDataSize, u_int8_t sequenceNum);

/**
* @brief Sends data packet
* @param fd File Descriptor 
* @param ptr Pointer to file being trasnferred
* @param fileSize Size of the file in bytes
* @return Positive if succeeded, negative otherwise
*/
int sendDataPacket(int fd, FILE* ptr, long fileSize);

int sendControlPacket (int fd, u_int8_t controlField, long fileSize, char fileName[]);

#endif
