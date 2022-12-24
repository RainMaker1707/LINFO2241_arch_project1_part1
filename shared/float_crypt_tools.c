//
// Created by RainMaker on 08/12/2022.
//

#include "float_crypt_tools.h"
//#define FILE_SIZE 1024


void print_array(float* array, int size){
    printf("\n");
    for(int i=0 ; i<size ; i++){
        printf("[ ");
        for(int j=0 ; j<size ; j++) {
            printf("%f ", array[j+(i*size)]);
        }
        printf(" ]\n");
    }
}


inline void encrypt_file(int key_size, float* key, float* file, float* encrypted_file, int FILE_SIZE)__attribute__((always_inline));
inline void encrypt_file(int key_size, float* key, float* file, float* encrypted_file, int FILE_SIZE){
#if OPTIM == 2 /// Not optimised version
    // Float version line*line wit unrolling and inliner
    int nb_steps_per_dim = FILE_SIZE / key_size;
    int vstart, hstart;
    // Compute sub-matrices
    for (int l=0; l < nb_steps_per_dim ; l++) {
        vstart = l * key_size;
        for (int m=0; m < nb_steps_per_dim; m++) {
            hstart = m * key_size;
            //Do the sub-matrix multiplication
            for (int i=0; i < key_size; i++) {
                for (int k=0 ; k < key_size; k++) {
                    int iline = (vstart + i) * FILE_SIZE + hstart;
                    int kline = (vstart + k) * FILE_SIZE + hstart;
                    int r = key[i * key_size + k];
                    int j = 0;
                    while (j < key_size) {
                        encrypted_file[iline + j] += r * file[kline + j];
                        encrypted_file[iline + j + 1] += r * file[kline + j + 1];
                        encrypted_file[iline + j + 2] += r * file[kline + j + 2];
                        encrypted_file[iline + j + 3] += r * file[kline + j + 3];
                        encrypted_file[iline + j + 4] += r * file[kline + j + 4];
                        encrypted_file[iline + j + 5] += r * file[kline + j + 5];
                        encrypted_file[iline + j + 6] += r * file[kline + j + 6];
                        encrypted_file[iline + j + 7] += r * file[kline + j + 7];
                        j += 8;
                    }
                }
            }
        }
    }
#elif OPTIM == 3 /// Optimised version
    // Float version line*line wit unrolling and inliner
    int nb_steps_per_dim = FILE_SIZE / key_size;
    int vstart, hstart;
    //Compute sub-matrices
    for (int l=0; l < nb_steps_per_dim ; l++) {
        vstart = l * key_size;
        for (int m=0; m < nb_steps_per_dim; m++) {
            hstart = m * key_size;
            //Do the sub-matrix multiplication
            for (int i=0; i < key_size; i++) {
                for (int k=0 ; k < key_size; k++) {
                    int iline = (vstart + i) * FILE_SIZE + hstart;
                    int kline = (vstart + k) * FILE_SIZE + hstart;
                    int r = key[i * key_size + k];
                    int j = 0;
                    while (j < key_size) {
                        // load values needed to compute the final result
                        __m256 encrypted_vect = _mm256_loadu_ps(encrypted_file + iline + j);
                        __m256 file_vect = _mm256_loadu_ps(file + kline + j);
                        // set a vector of 8 times the current key value used
                        __m256 key_vect = _mm256_set1_ps(r);
                        // multiply key value and file line
                        __m256 multiplied_vect = _mm256_mul_ps(file_vect, key_vect);
                        // add multiplied values to encrypted values
                        encrypted_vect = _mm256_add_ps(encrypted_vect, multiplied_vect);
                        // store the encrypted vect in the right place
                        _mm256_storeu_ps(encrypted_file + iline + j, encrypted_vect);
                        j += 8;
                    }
                }
            }
        }
    }
#endif
}
