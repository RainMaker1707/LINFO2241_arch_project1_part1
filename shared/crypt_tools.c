#include "crypt_tools.h"
#define FILE_SIZE 1024
//#define FILE_SIZE 8

#if OPTIM == 5
    #pragma pack(1)
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
    };
#endif

inline void encrypt_file(int key_size, uint32_t* key, uint32_t* file, uint32_t* encrypted_file)__attribute__((always_inline));
void inline encrypt_file(int key_size, uint32_t* key, uint32_t* file, uint32_t* encrypted_file){
    #if OPTIM == 2
        // First version, non opti at all, score 30
//        printf("OPTIM 0\n");
        int nb_steps_per_dim = FILE_SIZE / key_size;

        // Multiple matrix computations
        for(int i=0 ; i<nb_steps_per_dim ; i++){
           for(int j=0 ; j<nb_steps_per_dim ; j++){
               // Single matrix computation
               for(int k=0 ; k<key_size ; k++){
                   for(int l=0 ; l<key_size ; l++){
                       for(int m=0 ; m<key_size ; m++){
                           encrypted_file[k*FILE_SIZE + l + j*key_size + i*key_size*FILE_SIZE] += key[k*key_size + m] * file[m*FILE_SIZE + l + j*key_size + i*key_size*FILE_SIZE];
                           // printf("result[%u] += key[%u] * file[%u]\n",k*file_size + l + j*key_size + i*key_size*file_size,k*key_size + m,m*file_size + l + j*key_size + i*key_size*file_size);
                       }
                   }
               }
            }
        }
    #elif OPTIM == 3
        // line * column, no unrolling
//        printf("OPTIM 1\n");
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
    #elif OPTIM == 4
        // line * line, no unrolling
//        printf("OPTIM 2\n");
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
    #elif OPTIM == 1
        // line * line, unrolling
//        printf("OPTIM 2\n");
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
    #elif OPTIM == 5
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
    #else
        ///TODO: optimised method to compute sub matrix
        uint32_t loopN = FILE_SIZE / key_size;
        uint32_t keyLineIndex = 0;
        for(uint32_t line = 0; line < FILE_SIZE; line++){ // for all lines in the file
            for(uint32_t subI = 0; subI < loopN; subI++){ // for each sub matrix in a line
                uint32_t start = key_size * subI;
                uint32_t end = start + key_size - 1;
                // compute result for the whole line and sub matrix
                uint32_t from = keyLineIndex * key_size;
                for(uint32_t index = start; index < end; index++) { // for all number in the sub matrix line
                    encrypted_file[index] += key[from + (index % key_size)] * file[index];
                    if(line == 0) printf("%u ", encrypted_file[index]);
                }
            }
            keyLineIndex++;
            if(keyLineIndex == 8) keyLineIndex = 0; //faster than modulo
        }
    #endif
}

void print_array(uint32_t* array, int size){
    printf("\n");
    for(int i=0 ; i<size ; i++){
        printf("[ ");
        for(int j=0 ; j<size ; j++) {
            printf("%d ",array[j+(i*size)]);
        }
        printf(" ]\n");
    }
}
