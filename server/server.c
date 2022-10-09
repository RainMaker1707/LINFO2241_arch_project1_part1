#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../shared/crypt_tools.h"

int main(int argc, char **argv){
    printf("Server running...\n");


    int thread_n;
    int file_size;
    int port;

    int opt;
    char* err;
    while((opt = getopt(argc, argv, "j:s:p")) != -1){
        switch(opt){
            case 'j':
                thread_n = strtol(argv[optind-1], &err, 10);
                if(*err != '\0' || thread_n <= 0){
                    printf("Argument Error: Thread_number (-j) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                file_size = strtol(argv[optind-1], &err, 10);
                if(*err != '\0' || file_size <= 0){
                    printf("Argument Error: File Size (-s) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                port = strtol(argv[optind], &err, 10);
                if(*err != '\0' || port <= 0){
                    printf("Argument Error: Port (-p) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
        }
    }
    printf("Arguments:\n\tThread number: \t%i\n\tFile size: \t\t%i\n\tPort: \t\t\t%i\n",
           thread_n, file_size, port);

    /// Code the real server here
    return EXIT_SUCCESS;
}

