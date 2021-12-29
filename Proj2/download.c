#include "connection.h"
#include "parse.h"

#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(-1);
    }

    int parseCredentials = hasCredentials(argv[1]);

    char user[MAX_USER_SIZE]; // TODO: dynamic memory
    char pass[MAX_PASS_SIZE];
    char host[MAX_HOST_SIZE];
    char path[MAX_PATH_SIZE];

    if (parseInput(argv[1], user, pass, host, path, parseCredentials))
        exit(-1);

    printf("user: %s\npass: %s\nhost: %s\npath: %s\n", user, pass, host, path);
    return 0;
}
