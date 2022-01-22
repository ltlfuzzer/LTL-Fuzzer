#include <iostream>
#include <pathstore.h>
#include <targetstore.h>
#include <string>
#include <set>
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <utils.h>
#include <automata.h>
#include <automata_handler.h>
#include <target_location.h>
#include <boost/process.hpp>

namespace ltlfuzz{

    
class LTLFuzzer{
    public:
        std::string aflgo_paras;
        std::string aflgo_fuzz_path;
        int total_time_budget;
        int time_budget_one_target;
        int time_to_exploitation;
        std::string ltl_dir;
        std::string build_dir; 
        std::string program_paras;
        std::string network_link;
        std::string protocol_name;
        std::string dictionary;
        std::string targets_file;
        std::string exec_name;
        std::string error_message;

        INPUT_TYPE* input;
        std::string input_folder;
        std::string output_folder;
        std::string events_mapping_file;
        std::string all_events_file;
        std::string prefixDir;
        int size=0;

        path::PathsStore* path_store;
        AutomataHandler* automata_handler;
        TargetsStore* targets_store;
        
        
        LTLFuzzer(path::PathsStore* path_store);
        ~LTLFuzzer();

        void init(int flag);
        void fuzz(int flag);

    private:
        void save_input(std::string input_file, std::string folder);
        bool is_counterexample(std::string output);

        void replace_prefix_run_program(std::string prefix);
        std::string assemble_cmd(std::string target, int flag);

        int prefix_shmid;
        
    };
}
