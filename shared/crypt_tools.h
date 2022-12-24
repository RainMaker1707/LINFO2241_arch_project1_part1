#ifndef LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H
#define LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#if OPTIM == 5
struct data {
        uint32_t r;
        uint8_t nb_steps;
        uint8_t l;
        uint8_t m;
        uint8_t i;
        uint8_t k;
        uint8_t j;
        uint16_t vstart;
        uint16_t hstart;
        uint32_t kline;
        uint32_t iline;
        uint16_t iline_key;
        uint8_t key_size;
    }__attribute__((aligned (1))); // power of 2 > 200
#else
#pragma pack(push, 16)
typedef struct var{
    int vs, i_line, k_line, r;
}var;
#pragma pack(pop)
#endif

extern void encrypt_file(int key_size, uint32_t* key, uint32_t* file, uint32_t* encrypted_file);
void print_array(uint32_t* array, int size);


#endif //LINFO2241_ARCH_PROJECT1_PART1_CRYPT_TOOLS_H
