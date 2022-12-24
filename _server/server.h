#ifndef LINFO2241_ARCH_PROJECT1_PART1_SERVER_H
#define LINFO2241_ARCH_PROJECT1_PART1_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <x86intrin.h>
#include "../shared/verbose.h"
#if OPTIM == 0 || OPTIM == 1
#include "../shared/crypt_tools.h"
#define ARRAY_TYPE uint32_t
#elif OPTIM == 2 || OPTIM == 3
#include "../shared/float_crypt_tools.h"
#define ARRAY_TYPE float
#endif

typedef struct {
    int socket_fd;
    int file_size;
    ARRAY_TYPE** files;
} thread_args;

int thread_job(thread_args* args);

#endif //LINFO2241_ARCH_PROJECT1_PART1_SERVER_H


