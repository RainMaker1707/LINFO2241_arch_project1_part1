#include "server.h"
//#define TOTAL_FILE_SIZE 4194304
#define FILE_NUMBER 1000
#define ALI 8
//#define TOTAL_FILE_SIZE 256

void intHandler(int sig){
    sig = sig+1; //avoid warning
    fflush(stdout);
    exit(0);
}

int main(int argc, char **argv){
    signal(SIGINT, intHandler);

    // Arguments parsing
    int file_size = 0;
    int port = 0;
    int opt;
    while((opt = getopt(argc, argv, "j:s:p:v")) != -1) {
        switch (opt) {
            case 'j':
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
    int TOTAL_FILE_SIZE = file_size*file_size;
    verbose("Arguments:\n\tFile size: \t\t%i\n\tPort: \t\t\t%i\n", file_size, port);

    // Generation of 1000 files
    ARRAY_TYPE **files = (ARRAY_TYPE**)malloc(sizeof(void*)*FILE_NUMBER);
    for(int i=0 ; i<FILE_NUMBER ; i++){
        files[i] = (ARRAY_TYPE*) malloc(sizeof(ARRAY_TYPE)*file_size*file_size);
    }
    for(int i=0 ; i<file_size*file_size ; i++){
        files[0][i] = i;
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
    verbose("Socket bind\n");

    // FIFO listening queue starting
    if (listen(socket_fd, 65535) != 0) {
        fprintf(stderr, "Socket Error : unable to listen to socket.\n");
        return EXIT_FAILURE;
    }
    verbose("Server listening\n");

    // Client structure and socket
    int client_sock_fd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    ARRAY_TYPE *key = (ARRAY_TYPE*)malloc(sizeof(ARRAY_TYPE)*file_size*file_size);
    if(!key) return EXIT_FAILURE;
    ARRAY_TYPE *encrypted_file = (ARRAY_TYPE*)calloc(file_size*file_size, sizeof(ARRAY_TYPE));
    if(!encrypted_file) return EXIT_FAILURE;

    char err_code;
    int file_index, key_size;
    int total_file_size = file_size*file_size*sizeof(ARRAY_TYPE);
    int total_file_size_network = htonl(total_file_size);
    int total_key_size;
    ssize_t read_in;

    while((client_sock_fd = accept(socket_fd, (struct sockaddr*)&client, &len))) {
        // Handle client connexion
        if (client_sock_fd < 0) {
            fprintf(stderr, "Socket Error : unable to accept client.\n");
            return EXIT_FAILURE;
        }
        #if Q == 1
            struct timeval start, end;
            gettimeofday(&start, NULL);
        #endif
        // Get file index and key size from socket
        read_in = read(client_sock_fd, &file_index, sizeof(char) * 4);
        if (read_in == -1) {
            fprintf(stderr, "Error on file index extracting\n");
            return EXIT_FAILURE;
        }
        // Network byte order
        file_index = ntohl(file_index);
        read_in = read(client_sock_fd, &key_size, sizeof(char) * 4);
        if (read_in == -1) {
            fprintf(stderr, "Error on key size extracting\n");
            return EXIT_FAILURE;
        }
        // Network byte order
        key_size = ntohl(key_size);
        total_key_size = key_size * key_size * sizeof(ARRAY_TYPE);
        // Get key from socket
        read_in = 0;
        do {
            read_in += read(client_sock_fd, key + read_in, total_key_size - read_in);
        } while (read_in != total_key_size && read_in != -1);
        if (read_in == -1) {
            fprintf(stderr, "Error on file extracting: %ld\n", read_in);
            return EXIT_FAILURE;
        }
        // Encrypt file
        #if OPTIM == 2 || OPTIM == 3
            encrypt_file(key_size, key, files[file_index], encrypted_file, file_size); // server float
        #else
            encrypt_file(key_size, key, files[file_index], encrypted_file); // server uint
        #endif
//        print_array(encrypted_file, file_size);
        // print_array(key, key_size);
        err_code = 0;
        // Send response to client
        write(client_sock_fd, &err_code, sizeof(char));
        write(client_sock_fd, &total_file_size_network, sizeof(char)*4);
        ssize_t written = 0;
        if (err_code == 0) {
            while ((unsigned long)written < (unsigned long)TOTAL_FILE_SIZE && written != -1) {
                written += write(client_sock_fd, encrypted_file+written, (sizeof(ARRAY_TYPE)*TOTAL_FILE_SIZE)-written);
            }
        }

        #if Q == 1
            gettimeofday(&end, NULL);
            verbose("%ld\n",(((end.tv_sec - start.tv_sec)*1000000)+(end.tv_usec - start.tv_usec)));
        #endif

        // Close connection
        close(client_sock_fd);
        // Re-initialize memory of encrypted file
        memset(encrypted_file,0,TOTAL_FILE_SIZE);
    }

    // Free files
    for (int i=0 ; i<FILE_NUMBER ; i++){
        free(files[i]);
    }
    free(files);

    // Close main socket
    close(socket_fd);

    return EXIT_FAILURE;
}
