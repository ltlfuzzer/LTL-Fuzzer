#include "aflgo_ext.h"
#include <stdlib.h>
#include <stdio.h>

void replace_with_prefix(void* mem, uint32_t len, PREFIX_SMEM* shm){

    uint8_t prefix_size = shm -> arr_size; 
    memcpy((INPUT_TYPE*)mem, shm -> array, prefix_size*sizeof(INPUT_TYPE));
    
    char number_str[10];
    sprintf(number_str,"%d",prefix_size);
    setenv("PREFIX_LENGTH", number_str, 1);
}

void add_prefix_common(void** mem, uint32_t* len, PREFIX_SMEM* shm){
  
    uint32_t prefix_size = shm->arr_size;
    if(prefix_size == 0){
        return;
    }
    else{
        void *new_mem = (void*)malloc((*len + prefix_size)*sizeof(INPUT_TYPE));
        memcpy((INPUT_TYPE*)new_mem, shm->array, (prefix_size)*sizeof(INPUT_TYPE));
        memcpy((INPUT_TYPE*)(new_mem + prefix_size*sizeof(INPUT_TYPE)), (INPUT_TYPE*)(*mem), (*len)*sizeof(INPUT_TYPE));
        *len += prefix_size;
        *mem = new_mem;
        
        char number_str[10];
        sprintf(number_str,"%u",prefix_size);
        setenv("PREFIX_LENGTH", number_str, 1);
        return;
    }
}




