#include "server.h"
#define TOTAL_FILE_SIZE 4194304
#define FILE_SIZE 1024
//#define TOTAL_FILE_SIZE 256
//#define FILE_SIZE 8

int main(int argc, char **argv){
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

//    verbose("Arguments:\n\tThread number: \t\t%i\n\tFile size: \t\t%i\n\tPort: \t\t\t%i\n",thread_n, file_size, port);

    // Generation of 1000 files
    ARRAY_TYPE **files = (ARRAY_TYPE**)malloc(sizeof(void*)*1000);
    for(int i=0 ; i<1000 ; i++){
        files[i] = (ARRAY_TYPE*)malloc(TOTAL_FILE_SIZE);
    }
    for(int i=0 ; i<FILE_SIZE*FILE_SIZE ; i++){
        files[0][i] = i;
    }

    // Socket creation
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr,"Socket Error : socket creation failed.\n");
        return EXIT_FAILURE;
    }
//    verbose("Socket successfully created\n");

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
//    verbose("Socket binded\n");

    // FIFO listening queue starting
    if (listen(socket_fd, 65535) != 0) {
        fprintf(stderr, "Socket Error : unable to listen to socket.\n");
        return EXIT_FAILURE;
    }
//    verbose("Server listening\n");

    // Client structure and socket
    int client_sock_fd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    ARRAY_TYPE *key = (ARRAY_TYPE*)malloc(TOTAL_FILE_SIZE);
    if(!key) return EXIT_FAILURE;
    ARRAY_TYPE *encrypted_file = (ARRAY_TYPE*)calloc(FILE_SIZE*FILE_SIZE, sizeof(ARRAY_TYPE));
    if(!encrypted_file) return EXIT_FAILURE;


    char err_code;
    int file_index, key_size;
    int total_file_size_network = htonl(TOTAL_FILE_SIZE);
    int total_key_size;
    ssize_t read_in;

    while((client_sock_fd = accept(socket_fd, (struct sockaddr*)&client, &len))) {
        // Handle client connexion
        if (client_sock_fd < 0) {
            fprintf(stderr, "Socket Error : unable to accept client.\n");
            return EXIT_FAILURE;
        }

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
        // START Encrypt file ======================================================================
        // print_array(files[file_index],file_size);
//        encrypt_file(key_size, key, files[file_index], encrypted_file);
//        print_array(encrypted_file, file_size);
        // print_array(key, key_size);

        uint32_t* file = files[file_index];
        #if OPTIM == 2
            encrypt_file(key_size, key, files[file_index], encrypted_file);
        #elif OPTIM == 2
            // First version, non opti at all, score 30
            int nb_steps_per_dim = FILE_SIZE / key_size;

            // Multiple matrix computations
            for(int i=0 ; i<nb_steps_per_dim ; i++){
               for(int j=0 ; j<nb_steps_per_dim ; j++){
                   // Single matrix computation
                   for(int k=0 ; k<key_size ; k++){
                       for(int l=0 ; l<key_size ; l++){
                           for(int m=0 ; m<key_size ; m++){
                               encrypted_file[k*FILE_SIZE + l + j*key_size + i*key_size*FILE_SIZE] += key[k*key_size + m] * file[m*FILE_SIZE + l + j*key_size + i*key_size*FILE_SIZE];
                           }
                       }
                   }
                }
            }
        #elif OPTIM == 2
            // line * column, no unrolling
            int nb_steps_per_dim = FILE_SIZE / key_size;
            //Compute sub-matrices
            for (int i = 0; i < nb_steps_per_dim ; i ++) {
                int vstart = i * key_size;
                for (int j = 0; j < nb_steps_per_dim; j++) {
                    int hstart = j * key_size;
                    //Do the sub-matrix multiplication
                    for (int ln = 0; ln < key_size; ln++) {
                        int aline = (vstart + ln) * FILE_SIZE + hstart;
                        for (int col = 0; col < key_size; col++) {
                            int tot = 0;
                            for (int k = 0; k < key_size; k++) {
                                int vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                            }
                            encrypted_file[aline + col] = tot;
                        }
                    }
                }
            }
        #elif OPTIM == 2
            // line * column, unrolling
            int nb_steps_per_dim = FILE_SIZE / key_size;
            //Compute sub-matrices
            for (int i = 0; i < nb_steps_per_dim ; i ++) {
                int vstart = i * key_size;
                for (int j = 0; j < nb_steps_per_dim; j++) {
                    int hstart = j * key_size;
                    //Do the sub-matrix multiplication
                    for (int ln = 0; ln < key_size; ln++) {
                        int aline = (vstart + ln) * FILE_SIZE + hstart;
                        for (int col = 0; col < key_size; col++) {
                            int tot = 0;
                            int k = 0;
                            while (k < key_size) {
                                int vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                                vline = (vstart + k) * FILE_SIZE + hstart;
                                tot += key[ln * key_size + k] * file[vline + col];
                                k++;
                            }
                            encrypted_file[aline + col] = tot;
                        }
                    }
                }
            }
        #elif OPTIM == 2
            // line * line, no unrolling
            int nb_steps_per_dim = FILE_SIZE / key_size;
            //Compute sub-matrices
            for (int l=0; l<nb_steps_per_dim ; l++) {
                int vstart = l * key_size;
                for (int m=0; m<nb_steps_per_dim; m++) {
                    int hstart = m*key_size;
                    //Do the sub-matrix multiplication
                    for (int i=0; i<key_size; i++) {
                        for (int k=0 ; k<key_size; k++) {
                            int r = key[i*key_size + k];
                            for (int j=0; j<key_size; j++) {
                                int iline = (vstart + i)*FILE_SIZE + hstart;
                                int kline = (vstart + k)*FILE_SIZE + hstart;
                                encrypted_file[iline + j] += r * file[kline + j];
                            }
                        }
                    }
                }
            }
        #elif OPTIM == 0
            // line * line, unrolling
            int nb_steps_per_dim = FILE_SIZE / key_size;
            int vstart, hstart, iline, kline, r, j;
            //Compute sub-matrices
            for (int l=0; l<nb_steps_per_dim ; l++) {
                vstart = l * key_size;
                for (int m=0; m<nb_steps_per_dim; m++) {
                    hstart = m * key_size;
                    //Do the sub-matrix multiplication
                    for (int i=0; i<key_size; i++) {
                        iline = (vstart + i)*FILE_SIZE + hstart;
                        for (int k=0 ; k<key_size; k++) {
                            kline = (vstart + k)*FILE_SIZE + hstart;
                            r = key[i*key_size + k];
                            j = 0;
                            while (j<key_size) {
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                                encrypted_file[iline + j] += r * file[kline + j];
                                j++;
                            }
                        }
                    }
                }
            }
        #elif OPTIM == 1
            // line * line, unrolling, full line matrix
            int nb_steps_per_dim = FILE_SIZE / key_size;
            int vstart, hstart, iline, kline, r, j;

            for(int l=0 ; __builtin_expect(l<nb_steps_per_dim,1) ; l++){
                vstart = l * key_size;
                //Do the sub-matrix multiplication
                for (int i=0; i<key_size; i++) {
                    iline = (vstart + i)*FILE_SIZE;
                    for (int k=0 ; k<key_size; k++) {
                        j = 0;
                        kline = (vstart + k)*FILE_SIZE;
                        r = key[i*key_size + k];
                        while (__builtin_expect(j<FILE_SIZE,1)) {
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                            encrypted_file[iline + j] += r * file[kline + j];
                            j++;
                        }
                    }
                }
            }
        #elif OPTIM == 2
            int nb_steps = FILE_SIZE / key_size;
            int vs;
            for(int l = 0; l < nb_steps; l++) {
                vs = l * key_size;
                for (int i = 0; i < key_size; i++) {
                    for (int k = 0; k < key_size; k++) {
                        int iline = (vs+i) * FILE_SIZE;
                        int kline = (vs+k) * FILE_SIZE;
                        int r = key[i * key_size + k];
                        int j = 0;
                        while(j < FILE_SIZE){
                            encrypted_file[iline + j] += r * file[kline + j];
                            encrypted_file[iline + j + 1] += r * file[kline + j + 1];
                            encrypted_file[iline + j + 2] += r * file[kline + j + 2];
                            encrypted_file[iline + j + 3] += r * file[kline + j + 3];
                            encrypted_file[iline + j + 4] += r * file[kline + j + 4];
                            encrypted_file[iline + j + 5] += r * file[kline + j + 5];
                            encrypted_file[iline + j + 6] += r * file[kline + j + 6];
                            encrypted_file[iline + j + 7] += r * file[kline + j + 7];
                            j+=8;
                        }
                    }
                }
            }
        #elif OPTIM == 2
            // line * line, unrolling, struct for data
    //        printf("OPTIM 2\n");
            struct data dt;
            dt.key_size = key_size;
            dt.nb_steps = FILE_SIZE / dt.key_size;

            //Compute sub-matrices
            dt.l = 0;
            while (dt.l<dt.nb_steps) {
                dt.vstart = dt.l * dt.key_size;
                dt.m = 0;
                while (dt.m<dt.nb_steps) {
                    dt.hstart = dt.m * dt.key_size;
                    //Do the sub-matrix multiplication
                    dt.i = 0;
                    while (dt.i<dt.key_size) {
                        dt.iline = (dt.vstart + dt.i)*FILE_SIZE + dt.hstart;
                        dt.iline_key = dt.i*dt.key_size;
                        dt.k = 0;
                        while (dt.k<dt.key_size) {
                            dt.kline = (dt.vstart + dt.k)*FILE_SIZE + dt.hstart;
                            dt.r = key[dt.iline_key + dt.k];
                            dt.j = 0;
                            while (dt.j<dt.key_size) {
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                                encrypted_file[dt.iline + dt.j] += dt.r * file[dt.kline + dt.j];
                                dt.j++;
                            }
                            dt.k++;
                        }
                        dt.i++;
                    }
                    dt.m++;
                }
                dt.l++;
            }
        #endif

        // END Encrypt file ======================================================================
        err_code = 0;

        // Send response to client
        write(client_sock_fd, &err_code, sizeof(char));
        write(client_sock_fd, &total_file_size_network, sizeof(char)*4);
        if (err_code == 0) {
            write(client_sock_fd, encrypted_file, sizeof(char) * TOTAL_FILE_SIZE);
        }

        // Close connection
        close(client_sock_fd);
        // Re-initialize memory of encrypted file
        memset(encrypted_file,0,TOTAL_FILE_SIZE);
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
