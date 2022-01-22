#ifndef _SHMDATA_H_HEADER
#define _SHMDATA_H_HEADER

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef char INPUT_TYPE;

typedef struct Prefix_SMEM{
	INPUT_TYPE array[32][32768];
	uint32_t arr_size;  
} PREFIX_SMEM;     // From LTL-Fuzzer to AFLGo

static int set_prefix_smem(){
	/**
		From LTL-Fuzzer to AFLGo
	**/
	key_t key = 2222;
	int shmid = shmget(key, sizeof(PREFIX_SMEM), 0666|IPC_CREAT);
	if(shmid == -1){
		printf(">>> failed to set prefix_shm.\n");
		return -1;
	}
	else{
		printf(">>> succeeded to set prefix_shm.\n");
		return shmid;
	}
}

static int get_prefix_smem(){
	key_t key = 2222;
	int shmid = shmget(key, 0, 0);
	if(shmid == -1){
		printf(">>> sigmget prefix_shm failed.\n");
		return -1;
	}
	else{
		//printf("sigmget succeeded.\n");
		return shmid;
	}
}

static PREFIX_SMEM* bind_prefix_smem(int shmid){
	void *shm = NULL;
	shm = shmat(shmid, NULL, 0);
	if(shm == (void*)-1){
		fprintf(stderr, "failed to get prefix_shm. \n"); 
		return (PREFIX_SMEM*)-1;
	}
	else{
		//printf("shmget succeed. \n"); 
		return (PREFIX_SMEM*)shm;
	}
}

static void detach_prefix_shmem(PREFIX_SMEM* shm){
	int status = shmdt(shm);
	if(status){
		printf(">>> prefix_shm detach failed. \n"); 
	}
}

static void release_prefix_shmem(int shmid){
	if(shmid == -1){
		return;
	}
	int status = shmctl(shmid, IPC_RMID, NULL);
	if(status == -1){
		printf(">>> prefix_shm release failed. \n"); 
	}
}

#endif

