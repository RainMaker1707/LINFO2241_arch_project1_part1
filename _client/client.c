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

    return EXIT_SUCCESS;
}

