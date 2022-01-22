#include <iostream>
#include <ltlfuzzer.h>
#include <chrono>
#include <thread>
#include <pathstore.h>
#include <string>
#include <shared_table.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "Please specify the type of fuzzed programs: " << std::endl;
        std::cout << "\t1 is for network protocols" << std::endl;
        std::cout << "\t0 is for regular subjects" << std::endl;
        return 0;
    } 

    int flag = std::stoi(argv[1]);
    if(flag){
        //1 for protocols
        path::PathsStore path_store;
        ltlfuzz::LTLFuzzer fuzzer(&path_store);
        fuzzer.init(1);
        fuzzer.fuzz(1); 
    }
    else{
        //0 for common programs
        shared_memory_object::remove(shmId.c_str());
        managed_shared_memory segment(open_or_create, shmId.c_str(), size);
        path::PathsStore path_store(&segment);
        ltlfuzz::LTLFuzzer fuzzer(&path_store);
        fuzzer.init(0);
        fuzzer.fuzz(0); 
    }

    return 0;
}


