#include "connection.h"

struct hostent* getip(char hostname[]) {
    struct hostent *h;

    if ((h = gethostbyname(hostname)) == NULL) {
        herror("Failed to get host by name");
        exit(1);
    }

    return h;
}
