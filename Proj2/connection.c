#include "connection.h"

struct hostent *getip(char hostname[])
{
    printf("Getting IP Address from Host Name...\n");
    struct hostent *h;

    if ((h = gethostbyname(hostname)) == NULL)
    {
        herror("Failed to get host by name");
        exit(1);
    }

    return h;
}

int connectSocket(char *addr, int port)
{
    printf("Connecting to Server Socket...\n");

    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr); /* 32 bit Internet address network byte ordered */
    server_addr.sin_port = htons(port);            /* server TCP port must be network byte ordered */

    /* open a TCP socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }

    /* connect to the server */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect()");
        return -1;
    }

    printf("Server Socket Connected\n");

    return sockfd;
}

int getResponse(int socketFd, char *code, char *text)
{
    char c;
    int i = 0;
    ResponseState state = GET_STATUS_CODE;

    while (1)
    {
        switch (state)
        {

        case GET_STATUS_CODE:
        {
            if (read(socketFd, code, 3) < 0)
            {
                fprintf(stderr, "Error reading status code\n");
                return -1;
            }

            printf("%s", code);
            state = IS_MULTI_LINE;
            break;
        }

        case IS_MULTI_LINE:
        {
            if (read(socketFd, &c, 1) < 0)
            {
                fprintf(stderr, "Error reading response\n");
                return -1;
            }

            printf("%c", c);

            if (c == MULTI_LINE_SYMBOL)
            {
                state = READ_MULTI_LINE;
            }
            else
            {
                state = READ_LINE;
            }

            break;
        }

        case READ_MULTI_LINE:
        {

            int idxCounter = 0;
            char str[4];
            str[3] = '\0';

            while (1)
            {
                if (read(socketFd, &c, 1) < 0)
                {
                    fprintf(stderr, "Error reading response\n");
                    return -1;
                }

                printf("%c", c);
                if (c == '\n')
                    break;

                if (idxCounter <= 2)
                    str[idxCounter] = c;

                if (idxCounter == 3 && strcmp(str, code) == 0 && c == LAST_LINE_SYMBOL)
                {
                    state = READ_LINE;
                    break;
                }

                if (idxCounter > 3 && text != NULL && c != CARRIAGE_RETURN)
                    text[i++] = c;

                idxCounter++;
            }

            break;
        }
        case READ_LINE:
        {
            while (1)
            {
                if (read(socketFd, &c, 1) < 0)
                {
                    fprintf(stderr, "Error reading response\n");
                    return -1;
                }

                printf("%c", c);
                if (c == '\n')
                    break;

                if (text != NULL && c != CARRIAGE_RETURN)
                    text[i++] = c;
            }

            if (text != NULL)
                text[i++] = '\0';
            return 0;
        }

        default:
            break;
        }
    }
}

int sendCommand(int sockfd, char *cmd, char *argument)
{
    size_t cmd_len = strlen(cmd);

    if (write(sockfd, cmd, cmd_len) != cmd_len)
    {
        fprintf(stderr, "Error while sending command\n");
        return -1;
    }

    if (argument != NULL)
    {
        size_t arg_len = strlen(argument);
        char c = ' ';

        if (write(sockfd, &c, 1) != 1)
        {
            fprintf(stderr, "Error while sending command\n");
            return -1;
        }

        if (write(sockfd, argument, arg_len) != arg_len)
        {
            fprintf(stderr, "Error while sending command argument\n");
            return -1;
        }
    }

    char c = '\n';
    if (write(sockfd, &c, 1) != 1)
    {
        fprintf(stderr, "Error while sending command\n");
        return -1;
    }

    return 0;
}

int login(int sockfd, char *user, char *pass)
{

    printf("Sending username...\n");
    int res = handleCommand(sockfd, "user", user, NULL);

    if (res < 0)
    {
        fprintf(stderr, "Error while sending username\n");
        return -1;
    }

    if (res == 1)
    {
        printf("Sending password...\n");

        if (handleCommand(sockfd, "pass", pass, NULL) < 0)
        {
            fprintf(stderr, "Error while sending pasv\n");
            return -1;
        };
    }

    return 0;
}

int getPort(int sockfd, int *port)
{
    char responseCode[4];
    memset(responseCode, 0, 4);

    char response[PSV_RESPONSE_MAXSIZE];

    printf("Sending pasv...\n");

    if (handleCommand(sockfd, "pasv", NULL, response) < 0)
    {
        fprintf(stderr, "Error while sending pasv\n");
        return -1;
    }

    if (parsePort(response, port) < 0)
    {
        fprintf(stderr, "Error parsing port\n");
        return -1;
    }

    return 0;
}

int downloadFile(int sockfd, int downloadFd, char *path)
{
    char responseCode[4];
    memset(responseCode, 0, 4);
    char fileName[MAX_PATH_SIZE];

    char cmdResponse[MAX_RESPONSE_SIZE];

    int res = handleCommand(sockfd, "retr", path, cmdResponse);

    size_t fileSize = parseFileSize(cmdResponse);

    if (res < 0)
    {
        fprintf(stderr, "Error while sending retr\n");
        return -1;
    }
    else if (res != 2)
    {
        fprintf(stderr, "Server refused to transfer file\n");
        return -1;
    }

    parseFileName(path, fileName);
    if (saveFile(downloadFd, fileName, fileSize) < 0)
        return -1;

    if (getResponse(sockfd, responseCode, NULL) < 0)
    {
        fprintf(stderr, "Failed to confirm file transfer\n");
        return -1;
    }

    return 0;
}

int saveFile(int downloadFd, char *fileName, size_t fileSize)
{
    FILE *file = fopen(fileName, "wb");

    uint8_t buf[FILE_BUFFER_SIZE];
    int bytes;
    size_t readBytes = 0;
    size_t percentage = -1;
    while ((bytes = read(downloadFd, buf, FILE_BUFFER_SIZE)) > 0)
    {
        if (bytes < 0)
        {
            fprintf(stderr, "Error while reading file\n");
            return -1;
        }
        fwrite(buf, bytes, 1, file);

        if (fileSize > 0)
        {
            readBytes += bytes;
            size_t newPercentage = (readBytes * 100) / fileSize;

            fflush(stdout);
            if (newPercentage != percentage)
            {
                printf("\33[2K\r");
                printf("Progress: %lu%%", newPercentage);
            }
            percentage = newPercentage;
        }
    }
    printf("\n");

    fclose(file);
    return 0;
}

int handleCommand(int sockfd, char *cmd, char *argument, char *text)
{

    char responseCode[4];
    memset(responseCode, 0, 4);

    if (sendCommand(sockfd, cmd, argument))
    {
        fprintf(stderr, "Error while sending username\n");
        return -1;
    }

    int code;
    if (getResponse(sockfd, responseCode, text) < 0)
        return -1;
    code = responseCode[0] - '0';

    switch (code)
    {
    // expecting another response
    case 1:
    {
        return 2;
    }

    // positive completion reply
    case 2:
        break;

    // waiting for more information
    case 3:
        return 1;

    // resend the command
    case 4:
        if (handleCommand(sockfd, cmd, argument, text) < 0)
            return -1;
        break;

    // permanent negative completion reply
    case 5:
        fprintf(stderr, "Command wasn't accepted\n");
        return -1;
    }

    return 0;
}
