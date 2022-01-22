#include <instrument.h>
#include <codebean.h>

//For RERS
extern "C" void automata_handler(int input, int output){  
    inst::CodeBean::collect_trace(input, output); 
}

//For protocols
extern "C" void proposition_handler(const char* prop){
    inst::CodeBean::collect_proposition(prop);
}

extern "C" void input_handler(const char* input){
    inst::CodeBean::collect_input(input);
}

//Common 
extern "C" void state_handler(long *ptr, int *size, int num){
    inst::CodeBean::collect_state(ptr, size, num); 
}

extern "C" void evaluate_trace(int flag){
    inst::CodeBean::evaluate_trace(flag); 
}

extern "C" long begin_time(){
    struct timeval star;
    gettimeofday(&star, NULL);
    long b = star.tv_sec * 1000000 + star.tv_usec;
    return b;
}

extern "C" void end_time(long btime){
    struct timeval end;
    gettimeofday(&end, NULL);
    long etime = end.tv_sec*1000000 + end.tv_usec;
    long tm = etime - btime;
    printf("Time concumption: %ld us\n", tm);
}


extern "C" int32_t get_distance_to_target(char* block_id){
    return inst::CodeBean::get_distance_to_target(block_id);
}

extern "C" void init_distance_map(){
    inst::CodeBean::init_distance_map();
}

extern "C" void init_shared_memory(){
    inst::CodeBean::init_shared_memory();
}
