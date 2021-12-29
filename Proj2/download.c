#include "connection.h"
#include "parse.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    int parseCredentials = hasCredentials(argv[1]);

    char user[MAX_USER_SIZE]; // TODO: dynamic memory
    memset(user, 0, MAX_USER_SIZE);
    char pass[MAX_PASS_SIZE];
    memset(pass, 0, MAX_PASS_SIZE);
    char host[MAX_HOST_SIZE];
    memset(host, 0, MAX_HOST_SIZE);
    char path[MAX_PATH_SIZE];
    memset(path, 0, MAX_PATH_SIZE);

    if (parseInput(argv[1], user, pass, host, path, parseCredentials))
        exit(-1);

    struct hostent *h = getip(host);
    char *address = inet_ntoa(*((struct in_addr *) h->h_addr));

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", address);

    int sockfd = connectSocket(address);

    char response[4];
    response[3] = '\0';

    printf("Sending username...\n");
    sendCommand(sockfd, "user", "anonymous");

    printf("Getting response...\n");
    getResponse(sockfd, response);

    printf("Response : %s\n", response);

    printf("Closing connection...\n");
    if (close(sockfd) < 0) {
        fprintf(stderr, "Failed to close socket");
        exit(-1);
    }

    printf("Connection closed\n");

    return 0;
}
