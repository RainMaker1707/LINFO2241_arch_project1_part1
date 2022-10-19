#include "server.h"

int thread_job(thread_args* args){
    int socket_fd = args->socket_fd;
    int file_size = args->file_size;
    char **files = args->files;

    // Utils variables
    int max_request_size = 8+(file_size*file_size);
    int response_size = 5+(file_size*file_size);

    // Client structure and socket
    int client_sock_fd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    char *buff_request = (char*)malloc(sizeof(char)*max_request_size);
    char *buff_response = (char*)malloc(sizeof(char)*response_size);
    char *encrypted_file = (char*)malloc(sizeof(char)*file_size*file_size);
    char *key = (char*)malloc(sizeof(char)*file_size*file_size);
    char err_code;
    int file_index, key_size;
    int total_file_size = file_size*file_size;

    while(true){
        // Handle client connexion
        client_sock_fd = accept(socket_fd, (struct sockaddr*)&client, &len);
        if (client_sock_fd < 0){
            printf("Socket Error : unable to accept client.\n");
            return EXIT_FAILURE;
        }
        // printf("\nClient accepted\n");

        // Get file index and key size from socket
        read(client_sock_fd, buff_request, sizeof(char)*max_request_size);
        memcpy(&file_index, buff_request, sizeof(char)*4);
        memcpy(&key_size, buff_request+(sizeof(char)*4), sizeof(char)*4);
        // printf("KEY SIZE : %d\n",key_size);


        // Verification on key_size
        if (file_size % key_size != 0 || key_size > file_size){
            printf("Key Error : the key size must divide the file size and can not exceed it.");
            err_code = 1;
        } else {
            // Get key from socket
            if(!key) return EXIT_FAILURE;
            memcpy(key, buff_request+(sizeof(char)*8), sizeof(char)*key_size*key_size);
            // Encrypt file
            encrypt_file(key_size, key, file_size, files[file_index], encrypted_file);
            err_code = 0;
        }

        // Prepare response to client
        memcpy(buff_response, &err_code, sizeof(char));
        memcpy(buff_response+sizeof(char), &total_file_size, sizeof(char)*4);
        if (err_code == 0){
            memcpy(buff_response+(sizeof(char)*5), encrypted_file, sizeof(char)*total_file_size);
        }

        // Send response
        write(client_sock_fd, buff_response, sizeof(char)*response_size);
        // printf("Answer sent\n");

        // Close connection
        close(client_sock_fd);
    }
}

int main(int argc, char **argv){
    printf("Server running...\n");

    // Arguments parsing
    int thread_n = 0;
    int file_size = 0;
    int port = 0;

    int index = 1;
    char* err;
    while(index < argc){
        char cmd = argv[index++][1];
        switch(cmd){
            case 'j':
                thread_n = strtol(argv[index++], &err, 10);
                if(*err != '\0' || thread_n <= 0){
                    printf("Argument Error: thread_number (-j) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                file_size = strtol(argv[index++], &err, 10);
                if(*err != '\0' || file_size <= 0){
                    printf("Argument Error: file size (-s) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                port = strtol(argv[index++], &err, 10);
                if(*err != '\0' || port <= 0){
                    printf("Argument Error: port (-p) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                printf("Argument Error: one argument is not supported.\n");
                return EXIT_FAILURE;
        }
    }
    // printf("Arguments:\n\tThread number: \t%i\n\tFile size: \t\t%i\n\tPort: \t\t\t%i\n",thread_n, file_size, port);

    // Generation of 1000 files
    char **files = (char**)malloc(sizeof(char*)*1000);
    for(int i=0 ; i<1000 ; i++){
        files[i] = (char*)malloc(sizeof(char)*file_size*file_size);
        // Fill files with letters A to test
        for(int j=0; j<file_size*file_size; j++){
            files[i][j] = 'a';
        }
    }

    // Socket creation
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        printf("Socket Error : socket creation failed.\n");
        return EXIT_FAILURE;
    }
    printf("Socket successfully created\n");

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
        printf("Socket Error : socket binding failed.\n");
        return EXIT_FAILURE;
    }
    printf("Socket binded\n");

    // FIFO listening queue starting
    if (listen(socket_fd, SOMAXCONN) != 0) {
        printf("Socket Error : unable to listen to socket.\n");
        return EXIT_FAILURE;
    }
    printf("Server listening\n");

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

    return EXIT_SUCCESS;
}