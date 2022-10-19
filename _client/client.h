#ifndef LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H
#define LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "../shared/crypt_tools.h"
#include <pthread.h>

typedef struct {
    int key_size;
    struct sockaddr_in servaddr;
} thread_args;

typedef struct node{
    pthread_t* id;
    struct node* next;
} node;

typedef struct {
    node* head;
    node* current;
    node* tail;
    int size;
} list;

int thread_job(thread_args* args);

list* list_init();
void list_push(list *l, pthread_t* id);

#endif //LINFO2241_ARCH_PROJECT1_PART1_CLIENT_H
