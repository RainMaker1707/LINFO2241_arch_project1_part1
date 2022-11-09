#include "server.h"

int thread_job(thread_args* args){
    int socket_fd = args->socket_fd;
    int file_size = args->file_size;
    ARRAY_TYPE **files = args->files;

    // Utils variables
    int max_request_size = 8+(file_size*file_size*sizeof(ARRAY_TYPE));
    int response_size = 5+(file_size*file_size*sizeof(ARRAY_TYPE));

    // Client structure and socket
    int client_sock_fd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    char *buff_request = (char*)malloc(sizeof(char)*max_request_size);
    if(!buff_request) return EXIT_FAILURE;
    char *buff_response = (char*)malloc(sizeof(char)*response_size);
    if(!buff_response) return EXIT_FAILURE;
    ARRAY_TYPE *encrypted_file = (ARRAY_TYPE*)malloc(sizeof(ARRAY_TYPE)*file_size*file_size);
    if(!encrypted_file) return EXIT_FAILURE;
    ARRAY_TYPE *key = (ARRAY_TYPE*)malloc(sizeof(ARRAY_TYPE)*file_size*file_size);
    if(!key) return EXIT_FAILURE;

    char err_code;
    int file_index, key_size;
    int total_file_size = file_size*file_size*sizeof(ARRAY_TYPE);
    int total_file_size_network = htonl(total_file_size);
    ssize_t written, read_in;

    while(true){
//        struct timeval t0, t1, t2, t3, t4, t5;
        // Handle client connexion
        client_sock_fd = accept(socket_fd, (struct sockaddr*)&client, &len);
        if (client_sock_fd < 0){
            fprintf(stderr, "Socket Error : unable to accept client.\n");
            return EXIT_FAILURE;
        }
//        gettimeofday(&t0, NULL);

        // Get file index and key size from socket
        read_in = 0;
        do {
            read_in += read(client_sock_fd, &file_index+read_in, (sizeof(char)*4)-read_in);
        } while(read_in != 4 && read_in !=-1);
        if(read_in == -1) {
	        fprintf(stderr, "Error on file index extracting\n");
	        return EXIT_FAILURE;
	    }
        // Network byte order
        file_index = ntohl(file_index);

        read_in = 0;
        do {
            read_in += read(client_sock_fd, &key_size+read_in, (sizeof(char)*4)-read_in);
        } while(read_in != 4 && read_in !=-1);
        if(read_in == -1) {
            fprintf(stderr, "Error on key size extracting\n");
            return EXIT_FAILURE;
        }
        // Network byte order
        key_size = ntohl(key_size);
//        gettimeofday(&t1, NULL);


        // Verification on key_size
        if (file_size % key_size != 0 || key_size > file_size){
            fprintf(stderr, "Key Error : the key size must divide the file size and can not exceed it.");
            err_code = 1;
        } else {
            // Get key from socket
            read_in = 0;
            do {
                read_in += read(client_sock_fd, key+read_in, (sizeof(ARRAY_TYPE)*key_size*key_size)-read_in);
            } while(read_in != key_size*key_size*sizeof(ARRAY_TYPE) && read_in != -1);
            if(read_in == -1) {
                fprintf(stderr, "Error on file extracting: %ld\n", read_in);
                return EXIT_FAILURE;
            }
//            gettimeofday(&t2, NULL);
            // Encrypt file
            encrypt_file(key_size, key, file_size, files[file_index], encrypted_file);
            err_code = 0;
//            gettimeofday(&t3, NULL);
        }

        // Prepare response to client
        memcpy(buff_response, &err_code, sizeof(char));
        memcpy(buff_response+sizeof(char), &total_file_size_network, sizeof(char)*4);
        if (err_code == 0){
            memcpy(buff_response+(sizeof(char)*5), encrypted_file, sizeof(char)*total_file_size);
        }
//        gettimeofday(&t4, NULL);
        // Send response
        written = write(client_sock_fd, buff_response, sizeof(char)*response_size);
//        write(client_sock_fd, buff_response, sizeof(char)*response_size);
        if (written != response_size){
            verbose("%d WRITTEN : %zd, ERROR : %s\n",client_sock_fd,written,strerror(errno));
        }
//        gettimeofday(&t5, NULL);
//        printf("%2d | %6d | %6d | %6d | %6d | %6d | %6d\n",client_sock_fd,t1.tv_usec - t0.tv_usec, t2.tv_usec - t1.tv_usec, t3.tv_usec - t2.tv_usec, t4.tv_usec - t3.tv_usec, t5.tv_usec - t4.tv_usec, t5.tv_usec - t0.tv_usec);
        // printf("Answer sent\n");

        // Close connection
        close(client_sock_fd);
    }
}

int main(int argc, char **argv){
    verbose("Server starting...\n");

    // Arguments parsing
    int thread_n = 0;
    int file_size = 0;
    int port = 0;

    int opt;
    while((opt = getopt(argc, argv, "j:s:p:v")) != -1) {
        switch (opt) {
            case 'j':
                thread_n = atoi(optarg);
                break;
            case 's':
                file_size = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                setVerbose(true);
                break;
            default:
                fprintf(stderr, "Usage: %s [-j threads] [-s file_size] [-p port]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    verbose("Arguments:\n\tThread number: \t\t%i\n\tFile size: \t\t%i\n\tPort: \t\t\t%i\n",thread_n, file_size, port);

    // Generation of 1000 files
    ARRAY_TYPE **files = (ARRAY_TYPE**)malloc(sizeof(void*)*1000);
    for(int i=0 ; i<1000 ; i++){
        files[i] = (ARRAY_TYPE*)malloc(sizeof(ARRAY_TYPE)*file_size*file_size);
        if(i == 0){
            for(int j=0 ; j<file_size*file_size ; j++){
                files[0][j] = j;
            }
        }
    }

    // Socket creation
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr,"Socket Error : socket creation failed.\n");
        return EXIT_FAILURE;
    }
    verbose("Socket successfully created\n");

    // Socket option for multi-thread sharing
    int enable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));

    // IP and PORT assignation
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // IP and PORT binding
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        fprintf(stderr, "Socket Error : socket binding failed.\n");
        return EXIT_FAILURE;
    }
    verbose("Socket binded\n");

    // FIFO listening queue starting
    if (listen(socket_fd, 65535) != 0) {
        fprintf(stderr, "Socket Error : unable to listen to socket.\n");
        return EXIT_FAILURE;
    }
    verbose("Server listening\n");

    // Launch threads
    thread_args args;
    args.socket_fd = socket_fd;
    args.file_size = file_size;
    args.files = files;
    pthread_t *ids = (pthread_t*)malloc(sizeof(pthread_t)*thread_n);
    for(int i=0 ; i<thread_n ; i++){
        pthread_create(&ids[i], NULL, (void*)thread_job, &args);
    }

    for(int i=0 ; i<thread_n ; i++){
        pthread_join(ids[i], NULL);
    }

    // Free files
    for (int i=0 ; i<1000 ; i++){
        free(files[i]);
    }
    free(files);


    // Close main socket
    close(socket_fd);

    return EXIT_FAILURE;
}
