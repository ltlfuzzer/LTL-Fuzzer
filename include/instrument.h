#include <stdint.h>
#include <sys/time.h>

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#if defined(__cplusplus)
extern "C" {
#endif

//For RERS
void automata_handler(int input, int output);

//For protocols
void proposition_handler(const char* prop);
void input_handler(const char* input);

void state_handler(long *ptr, int *size, int num);
void evaluate_trace(int flag); //0: RESR; 1: protocols

/**
    Record beginning time and ending time
**/
long begin_time();
void end_time(long btime);

int32_t get_distance_to_target(char* block_id);
void init_distance_map();
void init_shared_memory();

#if defined(__cplusplus)
}
#endif

#endif

