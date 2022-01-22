#include <stdint.h>
#include <string>
#include <vector>
#include <automata.h>
#include <shmdata.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <sys/shm.h>
#include <sys/types.h>
#include <pathwriter.h>
#include <utility> 
#include <map>
#include <fstream>
#include <sstream>
#include <boost/functional/hash.hpp>

#ifndef CODEBEAN_H
#define CODEBEAN_H

namespace inst{

    class CodeBean{
        public:
            static std::string SHM_ENV_VAR; 
            static int MAP_SIZE; 
            
            static std::vector<int> trace_common;
            static std::vector<std::string> trace_protocol;
            static std::vector<std::string> input_protocol;
            static std::map<std::string, int32_t> distance_map;
            static std::vector<size_t> state_vector; 
            static int offset;
            static std::string distance_file_path; 
            static std::string delimiter;
            static std::string delimiter_prefix;
            static std::string delimiter_ltl;
            static std::string ltl_formula_path;
            static std::string DRY_RUN_ENV;
            static void init_distance_map();
            static void collect_proposition(std::string prop);
            static void collect_input(std::string input);
            static void collect_trace(int input, int output);
            static void collect_state(long *ptr, int *size, int num);
            static void evaluate_trace(int flag);
            static int32_t get_distance_to_target(char* block_id);
            static void init_shared_memory();
        
        private:
            CodeBean(){}
            static std::map<int, std::string> event_reverse_map; 
            static std::map<std::string, int> prop_to_loc;
            static std::vector< std::pair<std::string, int> > prop_loc_vec;
            static std::vector<std::string> trace_string;
            static void saved_prefix_path(std::string aPath, std::string prefix);
            static void load_event_reverse_map();
            static void normalize_trace();
            static int get_last_state(std::string aPath);
            static void check_conditions(std::vector<std::set<std::string>> events, unsigned int begin_loc, unsigned int end_loc, int flag);
            static void check_acceptance(std::vector<lfz::automata::MCState> states, std::string aPath, int flag);
            static void extract_prefix_automata_path(std::vector<lfz::automata::MCState> states, std::string& aPath, std::string& prefix, int flag);

    };
}

#endif
