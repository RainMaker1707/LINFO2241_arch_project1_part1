#include "client.h"

int thread_job(thread_args* args){
    struct sockaddr_in servaddr = args->servaddr;
    int key_size = args->key_size;
    int sockfd;
    int request_size = (key_size*key_size)+8;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else {
        printf("Socket successfully created..\n");
    }

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    }
    else {
        printf("Connected to the server..\n");
    }

    int index = 90;
    char *key = (char*)malloc(sizeof(char)*key_size*key_size);
    if(!key) return EXIT_FAILURE;
    // Fill key with "a" to test
    for(int i=0 ; i<key_size*key_size ; i++){
        key[i] = 'a';
    }

    char *buff = (char*)malloc(sizeof(char)*request_size);
    if(!buff) return EXIT_FAILURE;
    memcpy(buff, &index, sizeof(char)*4);
    memcpy(buff+(sizeof(char)*4), &key_size, sizeof(char)*4);
    memcpy(buff+(sizeof(char)*8), key, sizeof(char)*key_size*key_size);

    write(sockfd, buff, sizeof(char)*request_size);
    struct timeval start, end;
    gettimeofday(&start, NULL);

    uint8_t *error = malloc(sizeof(char));
    if(!error) return EXIT_FAILURE;
    read(sockfd, error, sizeof(char)*1);
    if(*error != 0){
        printf("Server send error code: %u\n", *error);
        return EXIT_FAILURE;
    }


    uint32_t *file_size = malloc(sizeof(char)*4);
    if(!file_size) return EXIT_FAILURE;
    read(sockfd, file_size, sizeof(char)*4);
    if(*file_size == 0){
        printf("No file received: file size = 0\n");
        return EXIT_FAILURE;
    }
    sleep(1);
    char *ans = malloc(sizeof(char)* (*file_size));
    if(!ans) return EXIT_FAILURE;
    read(sockfd, ans, sizeof(char)* (*file_size));
    gettimeofday(&end, NULL);
    printf("Elapsed time between send and receive: %ld µs",
           ((end.tv_sec - start.tv_sec)*1000000+( end.tv_usec - start.tv_usec)));


    // close the socket
    close(sockfd);

    //garbage
    free(key);
    free(buff);
    free(error);
    free(file_size);
    free(ans);

    return EXIT_SUCCESS;
}


int main(int argc, char **argv){
    printf("Client running...\n");

    /// Arguments parsing
    int key_size;
    int request_rate;
    int request_time;
    int port;
    char target_ip[15];
    char *ip_and_port = NULL;

    int index = 1;
    char* err;
    while(index < argc){ // -1 for ip on the tail
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

    printf("Arguments:\n\tTarget IP: \t\t%s\n\tPort: \t\t\t%i\n\tKey size: \t\t%i\n"
           "\tRequest rate:   %i\n\tRequest time:   %i\n",
           target_ip, port, key_size, request_rate, request_time);

    /// CODE the real client here
    struct sockaddr_in servaddr;

    // assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(target_ip);
    servaddr.sin_port = htons(port);

    // Launch threads
    thread_args args;
    args.key_size = key_size;
    args.servaddr = servaddr;
    pthread_t *ids = (pthread_t*)malloc(sizeof(pthread_t)*1);
    for(int i=0 ; i<1 ; i++){
        pthread_create(&ids[i], NULL, (void*)thread_job, &args);
    }

    for(int i=0 ; i<1 ; i++){
        pthread_join(ids[i], NULL);
    }

    return EXIT_SUCCESS;
}

