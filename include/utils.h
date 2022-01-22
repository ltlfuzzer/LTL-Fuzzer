#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <set>
#include <sstream> 
#include <iterator>
#include <shmdata.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream>
extern "C"
{
    #include <shmdata.h>
}

using std::cout;
using std::endl;


#ifndef UTILS_H
#define UTILS_H
namespace utils{
    extern std::string aDelimiter;

    std::string get_last_state(std::string aPath);

    int str2int(std::string s);
    
    std::string get_current_time();
    
    std::string set_to_string(std::set<std::string> set);

    void string_to_input_type(std::string p, std::vector<INPUT_TYPE>& results);  
    
    std::vector<std::string> string2vector(std::string s);
    void string_to_vector(std::string p, std::vector<std::string>& results); 

    void gen_ltl_files(std::string script, std::string build_dir, std::string formula);

    INPUT_TYPE* read_input(int* size, std::string input_file);

    void write_input(INPUT_TYPE* input, int size,  std::string output_file);
    
    void write_to_shmem_protocol(std::vector<std::string> arr, uint32_t arr_size);  //For protocols
    void write_to_shmem_common(INPUT_TYPE arr[], uint32_t arr_size);  //For RERS
    void add_path_and_loc_pair(std::string path);
    //std::string decode(std::string input);
            

}

#endif
