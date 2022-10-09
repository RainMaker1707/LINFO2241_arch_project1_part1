#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../shared/crypt_tools.h"

int main(int argc, char **argv){
    printf("Client running...\n");
    int opt;
    int key_size = 0;
    int request_rate = 0;
    int request_time = 0;
    int port;
    char target_ip[15] = "255.255.255.255";

    char* err;
    while((opt = getopt(argc, argv, "k:r:t")) != -1){
        switch(opt){
            case 'k':
                key_size = strtol(argv[optind-1], &err, 10);
                if (*err != '\0'){
                    printf("Arguments error: Key size (-k) is not a number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'r':
                request_rate = strtol(argv[optind-1], &err, 10);
                if (*err != '\0'){
                    printf("Arguments error: Request rate (-r) is not a number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                request_time = strtol(argv[optind], &err,10);
                if (*err != '\0'){
                    printf("Arguments error: Request time (-t) is not a number\n");
                    return EXIT_FAILURE;
                }
                break;
        }
    }
    char *ip_and_port = argv[optind+1];
    char delim = ':';
    int index = 0;
    int end = strlen(ip_and_port);
    bool before = true;
    char port_char[5];
    int sub = 0;
    while(index < end){
        if(ip_and_port[index] == delim){
            before = false;
            target_ip[index] = '\0';
            sub = index+1;

        }else{
            if(before){
                target_ip[index] = ip_and_port[index];
            }else{
                port_char[index-sub] = ip_and_port[index];
            }
        }
        index++;
    }
    port = strtol(port_char, &err, 10);
    printf("Arguments:\n\tTarget IP: \t%s\n\tPort: \t\t%i\n\tKey size: \t%i\n"
           "\tRequest rate:   %i\n\tRequest time:   %i\n",
           target_ip, port, key_size, request_rate, request_time);
    return EXIT_SUCCESS;
}

