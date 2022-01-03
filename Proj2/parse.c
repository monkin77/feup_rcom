#include "parse.h"

int parseInput(char* input, char* user, char* pass, char* host, char* path, int parseCredentials) {
    int i = 0, j = 0;
    char protocol[PROTOCOL_SIZE + 1];
    InputState state = PROTOCOL;

    while (1) {
        char c = input[i++];

        if (c == '\0' && state != PATH) {
            fprintf(stderr, "Wrong URL format:\nftp://[<user>:<password>@]<host>/<url-path>\n");
            return -1;
        }

        switch (state)
        {
        case PROTOCOL:
            protocol[j++] = c;

            if (j == PROTOCOL_SIZE) {
                if (strcmp(protocol, "ftp://") != 0) {
                    fprintf(stderr, "Wrong protocol! Please use ftp://\n");
                    return -1;
                }

                if (parseCredentials) state = USER;
                else state = HOST;
                j = 0;
            }

            break;
        case USER:
            if (c == ':') {
                state = PASS;
                j = 0;
                break;
            }

            user[j++] = c;

            if (j > MAX_USER_SIZE) {
                fprintf(stderr, "Username is too large! Max: %d\n", MAX_USER_SIZE);
                return -1;
            }

            break;
        case PASS:
            if (c == '@') {
                state = HOST;
                j = 0;
                break;
            }

            pass[j++] = c;

            if (j > MAX_PASS_SIZE) {
                fprintf(stderr, "Password is too large! Max: %d\n", MAX_PASS_SIZE);
                return -1;
            }

            break;
        case HOST:
            if (c == '/') {
                state = PATH;
                j = 0;
                break;
            }

            host[j++] = c;

            if (j > MAX_HOST_SIZE) {
                fprintf(stderr, "Hostname is too large! Max: %d\n", MAX_HOST_SIZE);
                return -1;
            }

            break;
        case PATH:
            if (c == '\0') return 0;

            path[j++] = c;

            if (j > MAX_PATH_SIZE) {
                fprintf(stderr, "File path is too large! Max: %d\n", MAX_PATH_SIZE);
                return -1;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

int hasCredentials(char* input) {
    int foundColon;

    for (int i = 0; input[i] != '\0'; ++i) {
        if (input[i] == ':') foundColon = 1;

        else if (foundColon && input[i] == '@')
            return 1;
    }

    return 0;
}

char* strrev(char* str) {
    if (!str || ! *str) return str;

    int i = strlen(str) - 1, j = 0;

    while (i > j) {
        char c = str[i];
        str[i--] = str[j];
        str[j++] = c;
    }

    return str;
}

int parsePort(char* response, int* port) {
    size_t i = strlen(response) - 1, currIdx = 0;
    char first[4], second[4];
    memset(first, 0, 4);
    memset(second, 0, 4);

    if (response[i--] != ')') {
        if (response[i + 1] != '.' || response[i--] != ')') {
            fprintf(stderr, "Wrong response format: %s\n", response);
            return -1;
        }
    }

    while (response[i] != ',')
        second[currIdx++] = response[i--];
    i--;
    currIdx = 0;
    while (response[i] != ',')
        first[currIdx++] = response[i--];

    int p1 = atoi(strrev(first));
    int p2 = atoi(strrev(second));

    *port = (p1 * 256) + p2;
    return 0;
}
