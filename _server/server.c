#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../shared/crypt_tools.h"

int main(int argc, char **argv){
    printf("Server running...\n");


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
                    printf("Argument Error: Thread_number (-j) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                file_size = strtol(argv[index++], &err, 10);
                if(*err != '\0' || file_size <= 0){
                    printf("Argument Error: File Size (-s) should be a positive number.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                port = strtol(argv[index++], &err, 10);
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