#ifndef LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H
#define LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


void encrypt_file(int key_size, uint32_t* key, int file_size, uint32_t* file, uint32_t* encrypted_file);
void print_array(uint32_t* array, int size);


#endif //LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H
