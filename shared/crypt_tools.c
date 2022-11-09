#include "crypt_tools.h"

void encrypt_file(int key_size, uint32_t* key, int file_size, uint32_t* file, uint32_t* encrypted_file){
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
}