#include "crypt_tools.h"

void encrypt_file(int key_size, uint32_t* key, int file_size, uint32_t* file, uint32_t* encrypted_file){
    #if OPTIM == 0
        // First version, non opti at all, score 30
//        printf("OPTIM 0\n");
        int nb_steps_per_dim = file_size / key_size;

        // Multiple matrix computations
        for(int i=0 ; i<nb_steps_per_dim ; i++){
           for(int j=0 ; j<nb_steps_per_dim ; j++){
               // Single matrix computation
               for(int k=0 ; k<key_size ; k++){
                   for(int l=0 ; l<key_size ; l++){
                       for(int m=0 ; m<key_size ; m++){
                           encrypted_file[k*file_size + l + j*key_size + i*key_size*file_size] += key[k*key_size + m] * file[m*file_size + l + j*key_size + i*key_size*file_size];
                           // printf("result[%u] += key[%u] * file[%u]\n",k*file_size + l + j*key_size + i*key_size*file_size,k*key_size + m,m*file_size + l + j*key_size + i*key_size*file_size);
                       }
                   }
               }
            }
        }
    #elif OPTIM == 3
        // Second version, same as first but with prof version
        // to compute indexes only once, score 60
//        printf("OPTIM 1\n");
        int nb_steps_per_dim = file_size / key_size;
        //Compute sub-matrices
        for (int i = 0; i < nb_steps_per_dim ; i ++) {
            int vstart = i * key_size;
            for (int j = 0; j < nb_steps_per_dim; j++) {
                int hstart = j * key_size;
                //Do the sub-matrix multiplication
                for (int ln = 0; ln < key_size; ln++) {
                    int aline = (vstart + ln) * file_size + hstart;
                    for (int col = 0; col < key_size; col++) {
                        int tot = 0;
                        for (int k = 0; k < key_size; k++) {
                            int vline = (vstart + k) * file_size + hstart;
                            tot += key[ln * key_size + k] * file[vline + col];
                        }
                        encrypted_file[aline + col] = tot;
                    }
                }
            }
        }
    #elif OPTIM == 2
        // Thrid version, line*line matrix multiplication, score 200
//        printf("OPTIM 2\n");
        int nb_steps_per_dim = file_size / key_size;
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
                            int iline = (vstart + i)*file_size + hstart;
                            int kline = (vstart + k)*file_size + hstart;
                            encrypted_file[iline + j] += r * file[kline + j];
                        }
                    }
                }
            }
        }
    #elif OPTIM == 1
        // Thrid version, line*line matrix multiplication, unrolling score 208
//        printf("OPTIM 2\n");
        int nb_steps_per_dim = file_size / key_size;
        int vstart, hstart, iline, kline, r, j;
        //Compute sub-matrices
        for (int l=0; l<nb_steps_per_dim ; l++) {
            vstart = l * key_size;
            for (int m=0; m<nb_steps_per_dim; m++) {
                hstart = m*key_size;
                //Do the sub-matrix multiplication
                for (int i=0; i<key_size; i++) {
                    iline = (vstart + i)*file_size + hstart;
                    for (int k=0 ; k<key_size; k++) {
                        kline = (vstart + k)*file_size + hstart;
                        r = key[i*key_size + k];
                        j = 0;
                        while (j<key_size) {
                            encrypted_file[iline + j] += r * file[kline + j];
                            encrypted_file[iline + j + 1] += r * file[kline + j];
                            encrypted_file[iline + j + 2] += r * file[kline + j];
                            encrypted_file[iline + j + 3] += r * file[kline + j];
                            encrypted_file[iline + j + 4] += r * file[kline + j];
                            encrypted_file[iline + j + 5] += r * file[kline + j];
                            encrypted_file[iline + j + 6] += r * file[kline + j];
                            encrypted_file[iline + j + 7] += r * file[kline + j];
                            j+=8;
                        }
                    }
                }
            }
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
