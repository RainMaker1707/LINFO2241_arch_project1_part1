//
// Created by RainMaker on 08/12/2022.
//

#ifndef LINFO2241_ARCH_PROJECT1_PART1_FLOAT_CRYPT_TOOLS_H
#define LINFO2241_ARCH_PROJECT1_PART1_FLOAT_CRYPT_TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <x86intrin.h>
#include "verbose.h"

extern void encrypt_file(int key_size, float* key, float* file, float* encrypted_file, int FILE_SIZE);

#endif //LINFO2241_ARCH_PROJECT1_PART1_FLOAT_CRYPT_TOOLS_H
