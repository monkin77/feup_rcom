#include "appProtocol.h"

int llopen(char* fileName, int status) {
  int fd;

  if (status == EMISSOR) fd = openEmissor(fileName);
  else fd = openReceptor(fileName); 

  return fd;
}

int llclose(int fd, int status) {
  int ans;

  if (status == EMISSOR) ans = closeEmissor(fd);
  else ans = closeReceptor(fd);

  if (ans < 0) return ans;
  return 1;
}
// Se leste és burro
int llwrite(int fd, char filePath[]) {
  FILE *ptr;
  if ((ptr = fopen(filePath, "rb")) == NULL) {
    perror("Error opening file\n");
    return -1;
  }

  fseek(ptr, 0, SEEK_END);
  long fileSize = ftell(ptr) + 1;
  rewind(ptr);
  if (sendControlPacket(fd, START_CTRL, fileSize, basename(filePath)) < 0) return -1;

  if (sendDataPacket(fd, ptr, fileSize) < 0) {
    return -1;
  }

  return 1;
}

int sendControlPacket (int fd, u_int8_t controlField, long fileSize, char fileName[]) {
  size_t fileNameSize = strlen(fileName) + 1;
  if (fileNameSize > 0xff) {
    perror("File name can only be up to 255 chars long\n");
    return -1;
  }

  u_int8_t *controlPacket = malloc(5 + sizeof(long) + fileNameSize); 

  controlPacket[0] = controlField;

  controlPacket[1] = T_FILE_SIZE;
  controlPacket[2] = (u_int8_t) sizeof(long);
  memcpy(controlPacket + 3, &fileSize, sizeof(long));

  controlPacket[3 + sizeof(long)] = T_FILE_NAME;
  controlPacket[4 + sizeof(long)] = (u_int8_t) fileNameSize;
  memcpy(controlPacket + 5 + sizeof(long), fileName, fileNameSize);

  sendDataFrame(fd, controlPacket, sizeof(controlPacket));
  free(controlPacket);

  return 1;
}

int sendDataPacket(int fd, FILE* ptr, long fileSize) {
  u_int8_t sequenceNum = 0;

  u_int8_t* data = malloc(fileSize);
  fread(data, sizeof(u_int8_t), fileSize, ptr);
  for (int idx = 0; idx < fileSize; idx += FRAME_DATA_SIZE) {
    int frameDataSize = min(FRAME_DATA_SIZE, fileSize - idx);
    u_int8_t* frameData = malloc(frameDataSize);
    memcpy(frameData, data + idx, frameDataSize);
    
    int dataPacketSize = frameDataSize + NUM_DATA_ADDITIONAL_FIELDS;
    u_int8_t* dataPacket = malloc(dataPacketSize);
    buildDataPacket(dataPacket, dataPacketSize, frameData, frameDataSize, sequenceNum);
    if (sendDataFrame(fd, dataPacket, sizeof(dataPacket)) < 0) {
      perror("Error sending data frame\n");
      return -1;
    }

    free(frameData); free(dataPacket);

    sequenceNum = (sequenceNum + 1) % 255;
  }

  free(data);
  return 1;
}

void buildDataPacket(u_int8_t* dataPacket, int dataPacketSize, u_int8_t* frameData, int frameDataSize, u_int8_t sequenceNum) {
  u_int8_t l2 = frameDataSize / NUM_OCTETS_MULTIPLIER;
  u_int8_t l1 = frameDataSize % NUM_OCTETS_MULTIPLIER;
  dataPacket[0] = DATA_CTRL; dataPacket[1] = sequenceNum; dataPacket[2] = l2; dataPacket[3] = l1;
  memcpy(dataPacket + NUM_DATA_ADDITIONAL_FIELDS, frameData, frameDataSize);
}


int main(int argc, char** argv) {
    if (argc < 3 ||
  	  ((strcmp("/dev/ttyS0", argv[1])) && (strcmp("/dev/ttyS1", argv[1])))) {

      printf("Usage: appProtocol serialPort type\nex: appProtocol /dev/ttyS0 0\n");
      exit(1);
    }

    int fd, status = atoi(argv[2]);

    if (status != EMISSOR && status != RECEPTOR) {
      printf("Usage: appProtocol serialPort type\n\n0-Emissor\n1-Receptor");
      exit(1);
    }

    if ((fd = llopen(argv[1], status)) < 0) {
      perror("llopen failed\n");
      exit(1);
    }

    return 0;
}
