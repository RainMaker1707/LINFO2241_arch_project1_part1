#ifndef LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H
#define LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "../shared/crypt_tools.h"
#include <pthread.h>

typedef struct {
    int key_size;
    struct sockaddr_in servaddr;
} thread_args;

int thread_job(thread_args* args);

#endif //LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H
