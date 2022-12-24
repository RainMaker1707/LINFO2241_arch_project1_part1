#include "client.h"

list* list_init(){
    list *l = malloc(sizeof(list));
    l->size = 0;
    l->current = NULL;
    l->head = NULL;
    l->tail = NULL;
    return l;
}

void list_push(list *l, pthread_t* id){
    struct node *n = malloc(sizeof(node));
    if(!n) return;
    n->id = id;
    n->next = NULL;
    if(l->head == NULL) {
        l->head = n;
        l->current = l->head;
        l->tail = l->head;
    }else{
        l->tail->next = n;
        l->tail = l->tail->next;
    }
    l->size++;
}

void join_and_free_threads_list(list *l){
    while(l->current != NULL){
        pthread_t* to_free = l->current->id;
        node* trash = l->current;
        l->current = l->current->next;
        pthread_join(*to_free, NULL);
        free(to_free);
        free(trash);
    }
    free(l);
}

int thread_job(thread_args* args){
    struct sockaddr_in servaddr = args->servaddr;
    int key_size = args->key_size;
    int sockfd = args->sockfd;
    int key_size_network = htonl(key_size);

    int request_size = (key_size*key_size*sizeof(ARRAY_TYPE))+8;
    ssize_t read_in, written;

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))
        != 0) {
        fprintf(stderr,"Connection with the server failed...\n");
        return EXIT_FAILURE;
    }

    // Generation of index and key
//    int index = rand() % 1000;
    int index = 0;
    ARRAY_TYPE *key = (ARRAY_TYPE*)malloc(sizeof(ARRAY_TYPE)*key_size*key_size*sizeof(char));
    if(!key) return EXIT_FAILURE;

    // To delete
    for(int i=0 ; i<key_size*key_size ; i++){
        key[i] = (float)1;
    }

    // Preparing request buffer
    char *buff_request = (char*)malloc(sizeof(char)*request_size);
    if(!buff_request) return EXIT_FAILURE;
    index = htonl(index);
    memcpy(buff_request, &index, sizeof(char)*4);
    memcpy(buff_request+(sizeof(char)*4), &key_size_network, sizeof(char)*4);
    memcpy(buff_request+(sizeof(char)*8), key, sizeof(ARRAY_TYPE)*key_size*key_size*sizeof(char));

    // Send request
    written = write(sockfd, buff_request, sizeof(char)*request_size);
    if (written != request_size){
        verbose("%d WRITTEN : %zd\nERROR : %s\n",sockfd,written, strerror(errno));
        errno = 0;
    }

    // Timers
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Get error field from response
    uint8_t *error = malloc(sizeof(char));
    if(!error) return EXIT_FAILURE;
    read_in = read(sockfd, error, sizeof(char)*1);
    if (read_in != 1){
        verbose("%d READ IN 1 : %zd\nERROR : %s\n",sockfd,read_in, strerror(errno));
        errno = 0;
    }
    gettimeofday(&end, NULL);
    verbose("MAT: %ld\n", (((end.tv_sec - start.tv_sec)*1000000)+(end.tv_usec - start.tv_usec)));
    if(*error != 0){
        fprintf(stderr, "Server send error code: %u\n", *error);
        return EXIT_FAILURE;
    }
    int file_size;
    read_in = read(sockfd, &file_size, sizeof(char)*4);
    file_size = ntohl(file_size);
    if (read_in != 4){
        verbose("%d READ IN 2 : %zd\nERROR : %s\n",sockfd,read_in, strerror(errno));
        errno = 0;
    }
    if(file_size == 0){
        fprintf(stderr,"No file received: file size = 0\n");
        return EXIT_FAILURE;
    }
    // Get encrypted file from response (size sent by server is already multiplied by sizeof(ARRAY_TYPE)
    float *encrypted_file = (float*)malloc(sizeof(float)*file_size);
    if(!encrypted_file) return EXIT_FAILURE;
    read_in = 0;
    while ((unsigned long)read_in < (unsigned long)file_size && read_in != -1){
        read_in += read(sockfd, encrypted_file+read_in, (file_size) - read_in);
    }
    if (read_in != file_size){
        verbose("%d READ IN 3 : %zd\nERROR : %s\n",sockfd,read_in, strerror(errno));
        errno = 0;
    }

    // close the socket
    close(sockfd);
    //garbage
    free(key);
    free(buff_request);
    free(error);
    free(encrypted_file);

    return EXIT_SUCCESS;
}


uint64_t ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) * 1000000000  / lambda;
}


int main(int argc, char **argv){
    verbose("Client running...\n");

    // Arguments parsing
    int key_size = 8;
    int request_rate = 1;
    int request_time = 1;
    int port = 0;
    char target_ip[15];
    char *ip_and_port = NULL;

    int index = 1;
    char* err;
    while(index < argc){
        char cmd = argv[index++][1];
        switch(cmd){
            case 'k':
                key_size = strtol(argv[index++], &err, 10);
                if (*err != '\0' || key_size <= 0){
                    printf("Arguments error: Key size (-k) should be a positive number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'r':
                request_rate = strtol(argv[index++], &err, 10);
                if (*err != '\0' || request_rate <= 0){
                    printf("Arguments error: Request rate (-r) should be a positive number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                request_time = strtol(argv[index++], &err,10);
                if (*err != '\0' || request_time <= 0){
                    printf("Arguments error: Request time (-t) should be a positive number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'v':
                setVerbose(true);
                break;
            default:
                ip_and_port = argv[index-1];
                char delim = ':';
                int i = 0;
                int end = strlen(ip_and_port);
                bool before = true;
                char port_char[5];
                int sub = 0;
                while(i < end){
                    if(ip_and_port[i] == delim){
                        before = false;
                        target_ip[i] = '\0';
                        sub = i+1;
                    }else{
                        if(before){
                            target_ip[i] = ip_and_port[i];
                        }else{
                            port_char[i-sub] = ip_and_port[i];
                        }
                    }
                    i++;
                }
                port = strtol(port_char, &err, 10);
                break;
        }
    }

    verbose("Arguments:\n\tTarget IP: \t\t%s\n\tPort: \t\t\t%i\n\tKey size: \t\t%i\n\tRequest rate:"
           "\t\t%i\n\tRequest time:\t\t%i\n",target_ip, port, key_size, request_rate, request_time);

    struct sockaddr_in servaddr;

    // assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(target_ip);
    servaddr.sin_port = htons(port);


    struct timespec start, current;

    int n = 0;
    list *thread_list = list_init();
    clock_gettime(CLOCK_REALTIME, &start);
    while(1) {
        int sockfd;
        // socket creation and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            fprintf(stderr, "Socket creation failed...\n");
            return EXIT_FAILURE;
        }

        thread_args args;
        args.key_size = key_size;
        args.servaddr = servaddr;
        args.sockfd = sockfd;


        clock_gettime(CLOCK_REALTIME, &current);
        // printf("ELAPSED : %lu\n",current.tv_sec - start.tv_sec);
        if(current.tv_sec - start.tv_sec >= (long)request_time) {
            join_and_free_threads_list(thread_list);
            verbose("Run out of time: %d s\n", request_time);
            verbose("Total threads number: %d\n", n);
            break;
        }
        pthread_t *id = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(id, NULL, (void *) thread_job, &args);
        // add threads to linked list
        list_push(thread_list, id);
        n++;
        // handle request rate by sleeping interval
        #if Q == 1
            uint32_t rate = ran_expo(request_rate);
            verbose("EXP: %ld\n", rate);
            usleep(rate/1000);
        #else
            usleep((((float)1/(float)request_rate)*1000000));
        #endif
    }
    return EXIT_SUCCESS;
}

