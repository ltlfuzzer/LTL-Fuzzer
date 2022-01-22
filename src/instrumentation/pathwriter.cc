#include "shared_table.h"
#include <string.h>
#include <stdlib.h>
#include "pathwriter.h"

using std::cout;
using std::endl;
using namespace boost::interprocess;

inst::PathWriter::PathWriter(){}
inst::PathWriter::~PathWriter(){}

managed_shared_memory segment_w(open_only, shmId.c_str());
void_allocator alloc_inst_w(segment_w.get_segment_manager());

void inst::PathWriter::write_to_shared_table(std::string automata_path, std::string prefix, std::string metric){
    
    //get the shared table
    string_string_string_map *table = segment_w.find<string_string_string_map>(tableName.c_str()).first;

    //create char_string
    char_string path_automata_s(automata_path.c_str(), alloc_inst_w);
    char_string prefix_s(prefix.c_str(), alloc_inst_w);
    char_string metric_s(metric.c_str(), alloc_inst_w);
    
        
    if(table==nullptr)
        std::cout<<"failed to find the shared table" << std::endl;

    //insert into the map
    string_string_string_map::iterator sss_ite = table->find(path_automata_s);
    
    if(sss_ite == table->end()){

        string_string_value_type  ss_map_value(prefix_s, metric_s);
        string_string_map *col_map = segment_w.construct<string_string_map>
        ((automata_path+prefix).c_str())(std::less<char_string>(), alloc_inst_w);
        col_map->insert(ss_map_value);

        string_string_string_value_type sss_map_value(path_automata_s, *col_map);
        table->insert(sss_map_value);
    }
    else{
        sss_ite->second.insert(string_string_value_type(prefix_s, metric_s));
    }
}




