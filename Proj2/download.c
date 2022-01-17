#include "connection.h"

// Test with ./download ftp://anonymous:ola@ftp.gnu.org/video/fry720.jpg
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    char responseCode[4];
    memset(responseCode, 0, 4);

    char user[MAX_USER_SIZE];
    memset(user, 0, MAX_USER_SIZE);
    char pass[MAX_PWD_SIZE];
    memset(pass, 0, MAX_PWD_SIZE);
    char host[MAX_HOST_SIZE];
    memset(host, 0, MAX_HOST_SIZE);
    char path[MAX_PATH_SIZE];
    memset(path, 0, MAX_PATH_SIZE);

    if (parseInput(argv[1], user, pass, host, path))
        exit(-1);

    printf("User: %s\nPass: %s\nHost: %s\nPath: %s\n", user, pass, host, path);

    struct hostent *h = getip(host);
    char *address = inet_ntoa(*((struct in_addr *)h->h_addr));

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", address);

    int sockfd = connectSocket(address, SERVER_PORT);
    if (sockfd < 0)
    {
        fprintf(stderr, "Error while connecting to socket\n");
        return -1;
    }

    printf("Getting connection response:\n");
    getResponse(sockfd, responseCode, NULL);

    if (login(sockfd, user, pass) < 0)
        exit(-1);
    
    printf("Getting port from server...\n");
    int port;
    if (getPort(sockfd, &port) < 0)
    {
        fprintf(stderr, "Error while getting port from server\n");
        return -1;
    }

    printf("NEW PORT: %d\n", port);

    int downloadFd = connectSocket(address, port);
    if (downloadFd < 0)
    {
        fprintf(stderr, "Error while connecting to socket\n");
        return -1;
    }

    printf("Downloading file...\n");

    if (downloadFile(sockfd, downloadFd, path) < 0)
    {
        fprintf(stderr, "Error while downloading file\n");
        return -1;
    }

    printf("Closing connection...\n");
    if (close(sockfd) < 0)
    {
        fprintf(stderr, "Failed to close socket\n");
        exit(-1);
    }

    if (close(downloadFd) < 0)
    {
        fprintf(stderr, "Failed to close socket\n");
        exit(-1);
    }

    printf("Connection closed\n");

    return 0;
}
