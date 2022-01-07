#include "connection.h"

// Test with ./download ftp://anonymous:ola@ftp.gnu.org/
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    char responseCode[4];
    memset(responseCode, 0, 4);

    char user[MAX_USER_SIZE]; // TODO: dynamic memory
    memset(user, 0, MAX_USER_SIZE);
    char pass[MAX_PASS_SIZE];
    memset(pass, 0, MAX_PASS_SIZE);
    char host[MAX_HOST_SIZE];
    memset(host, 0, MAX_HOST_SIZE);
    char path[MAX_PATH_SIZE];
    memset(path, 0, MAX_PATH_SIZE);

    if (parseInput(argv[1], user, pass, host, path))
        exit(-1);

    printf("User: %s\nPass: %s\nHost: %s\nPath: %s\n", user, pass, host, path);

    struct hostent *h = getip(host);
    char *address = inet_ntoa(*((struct in_addr *) h->h_addr));

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", address);

    int sockfd = connectSocket(address, SERVER_PORT);

    printf("Getting connection response:\n");
    getResponse(sockfd, responseCode, NULL);

    if (login(sockfd, user, pass) < 0) exit(-1);

    printf("Getting port from server...\n");
    int port;
    if (getPort(sockfd, &port) < 0) {
        fprintf(stderr, "Error while getting port from server\n");
        return -1;
    }

    printf("NEW PORT: %d\n", port);

    printf("Closing connection...\n");
    if (close(sockfd) < 0) {
        fprintf(stderr, "Failed to close socket");
        exit(-1);
    }

    printf("Connection closed\n");

    return 0;
}
