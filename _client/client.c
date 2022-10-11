#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../shared/crypt_tools.h"

int main(int argc, char **argv){
    printf("Client running...\n");

    /// Arguments parsing
    int key_size;
    int request_rate;
    int request_time;
    int port;
    char target_ip[15];

    int index = 1;
    char* err;
    while(index < argc-1){ // -1 for ip on the tail
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
        }
    }
    char *ip_and_port = argv[index];
    char delim = ':';
    index = 0;
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

    printf("Arguments:\n\tTarget IP: \t\t%s\n\tPort: \t\t\t%i\n\tKey size: \t\t%i\n"
           "\tRequest rate:   %i\n\tRequest time:   %i\n",
           target_ip, port, key_size, request_rate, request_time);

    /// CODE the real client here

    return EXIT_SUCCESS;
}

