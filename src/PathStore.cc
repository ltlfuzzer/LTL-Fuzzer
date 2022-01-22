#include "pathstore.h"
#include <targetstore.h>
#include <string.h>
#include <utility>

using std::cout;
using std::endl;

using namespace boost::interprocess;

int path::PathsStore::init_run = 1;

//managed_shared_memory  segment(open_or_create, shmId.c_str(), size);
//void_allocator alloc_inst(segment.get_segment_manager());

//1:protocols; 0: RERS
path::PathsStore::PathsStore(managed_shared_memory* segment){
    this->segment=segment;
    void_allocator alloc_inst(segment->get_segment_manager());
    this->table = this->segment->find_or_construct<string_string_string_map>(tableName.c_str())(std::less<char_string>(), alloc_inst);
}

path::PathsStore::PathsStore(){}

void path::PathsStore::clean_up(){
    if(shared_memory_object::remove(shmId.c_str()))
        std::cout<< "remove shared table successfully!" <<std::endl;
    else
        std::cout << "failed to remove shared table!" << std::endl;
}

path::PathsStore::~PathsStore(){}

void path::PathsStore::insert_init_automata_path(std::string automata_path, std::string prefix, std::string metric){

    void_allocator alloc_inst(this->segment->get_segment_manager());

    char_string path_automata_s(automata_path.c_str(), alloc_inst);
    char_string prefix_s(prefix.c_str(), alloc_inst);
    char_string metric_s(metric.c_str(), alloc_inst);

    string_string_value_type  ss_map_value(prefix_s, metric_s);
    string_string_map *col_map = segment->construct<string_string_map>((automata_path+prefix).c_str())(std::less<char_string>(), alloc_inst);
         col_map->insert(ss_map_value);

    string_string_string_value_type sss_map_value(path_automata_s, *col_map);
    this->table->insert(sss_map_value);

}

std::pair<std::string, std::string> path::PathsStore::select_automataPath_and_prefix(std::string prefixDir){
    std::vector<std::string> fileList;
    std::string inputDirectory = prefixDir;
    const char* str = inputDirectory.c_str();
	DIR *p_dir;
	p_dir = opendir(str);
	if(p_dir == NULL){
		std::cout << "cannot open directory: " << inputDirectory << std::endl;
        return std::make_pair("0,", "");
	}
	struct dirent *p_dirent;
	while ( p_dirent = readdir(p_dir))
    {
        std::string tmpFileName = p_dirent->d_name;
        if( tmpFileName == "." || tmpFileName == "..")
        {
            continue;
        }
        else
        {
            fileList.push_back(tmpFileName);
        }
    }
    closedir(p_dir);
    if(fileList.empty()){
        return std::make_pair("0,", "");
    }

    std::string selected_path = get_automataPath(fileList);
    std::string prefix = get_prefix(selected_path, prefixDir);

    return std::make_pair(selected_path, prefix);
}

std::string path::PathsStore::get_automataPath(std::vector<std::string> &fileList){
    srand (time(NULL)); 
    auto it = fileList.begin();
    std::advance(it, rand() % fileList.size());
    std::string aPath = *it;

    return aPath;
}

std::string path::PathsStore::get_prefix(std::string aPath, std::string prefixDir){
    std::string fileName = prefixDir + aPath;
	std::ifstream in;
	in.open(fileName, std::ios::in);
	std::string single_token = "";
	if(in.is_open()){
		std::ostringstream tmp; 
		tmp << in.rdbuf();
		std::string total_prefix = tmp.str();
        in.close();
        if(total_prefix == ""){
            return single_token;
        }

		std::string delimiter = "@@";
        size_t pos = 0;
        std::vector<std::string> tokens_vec;
        std::string token;
        while ((pos = total_prefix.find(delimiter)) != std::string::npos) {
            token = total_prefix.substr(0, pos);
            total_prefix.erase(0, pos + delimiter.length());
            tokens_vec.push_back(token);
        }

        srand (time(NULL)); 
        auto it = tokens_vec.begin();
        std::advance(it, rand() % tokens_vec.size());
        std::string prefix = *it;
		
        return prefix;
	}
	else{
		return single_token;
	}
}


std::pair<ltlfuzz::Prefix, ltlfuzz::AutomataPath> path::PathsStore::select_prefix_aPath(){

    this->selected_item = select_automata_path();

    ltlfuzz::AutomataPath aPath = get_selected_automata_path();
    ltlfuzz::Prefix prefix = select_prefix();
    return std::make_pair(prefix, aPath);
}

int path::PathsStore::getSize(){

    return this->table->size();
}

void path::PathsStore::dump(){
    cout << ">>> print shared memory ...." << endl;
    for(string_string_string_map::iterator sss_ite = this->table->begin(); sss_ite != this->table->end(); ++sss_ite)
    {
        cout << "-----The automata path: " << sss_ite->first << endl;

        for(string_string_map::iterator ss_ite = (sss_ite->second).begin(); ss_ite != (sss_ite->second).end(); ++ss_ite)
        {
            cout<< "prefix: " << ss_ite->first << " metric: " << ss_ite->second <<endl;
        }
    }

    cout << ">>> End printfing shm ...." << endl;
}

/* Only in init run, AFL can select the init path "0," */
string_string_string_map::iterator path::PathsStore::select_automata_path(){

    std::vector<std::pair<std::any, double>> vector;
    std::cout << "table size: " << getSize() << std::endl;
    for(string_string_string_map::iterator sss_ite = this->table->begin(); sss_ite != this->table->end(); sss_ite++)
    {
        char_string path = sss_ite->first;
        std::cout << "path: " << sss_ite->first << std::endl;
        if(path != "0," && !init_run){
            ltlfuzz::AutomataPath aPath(std::string(path.begin(), path.end()));
            vector.push_back(std::make_pair(sss_ite, 1.0));
        }
    }
    std::cout << "vector size: " << vector.size() << std::endl;
    if(vector.size() == 0){
        init_run = 0;
        string_string_string_map::iterator it = this->table->begin();
        return it;
    }
    else{
        std::any pointer=strategy::RandomStrategy::instance()->select(vector);
        return std::any_cast<string_string_string_map::iterator>(pointer);
    }
   
}

ltlfuzz::AutomataPath path::PathsStore::get_selected_automata_path(){

    char_string path = this->selected_item->first;
    ltlfuzz::AutomataPath aPath(std::string(path.begin(), path.end()));
    return aPath;
}

double path::PathsStore::compute_prefix_fitness(ltlfuzz::Prefix prefix){
    
    return 1.0;
}

ltlfuzz::Prefix path::PathsStore::select_prefix(){

    string_string_map map_m = this->selected_item->second;

    std::vector<std::pair<std::any, double>> vector_;


   for(string_string_map::iterator ss_ite = map_m.begin(); ss_ite != map_m.end(); ++ss_ite){

       char_string prefix = ss_ite->first;
       char_string feedback = ss_ite->second;

       std::string prefix_s(prefix.begin(), prefix.end());
       std::string feedback_s(feedback.begin(), feedback.end());

       ltlfuzz::Prefix prefix_o(prefix_s, feedback_s);
       double f = compute_prefix_fitness(prefix_o);
       vector_.push_back(std::make_pair(prefix_o, f));
   }

   std::any selected = strategy::RandomStrategy::instance()->select(vector_);
   ltlfuzz::Prefix prefix_selected = std::any_cast<ltlfuzz::Prefix> (selected);

   return prefix_selected;
}

ltlfuzz::AutomataTransition path::PathsStore::select_transition(const lfz::automata::Automata &atm) {

    ltlfuzz::AutomataPath aPath = get_selected_automata_path();
    std::string last_state = utils::get_last_state(aPath.path);
    int state = utils::str2int(last_state);

    lfz::automata::transitions_t trans_v;
    atm.get_state_transitions(state, trans_v);

    std::vector<std::pair<std::any, double>> vector_;

    for(auto& e : trans_v){
        ltlfuzz::AutomataTransition tran(e.first, std::to_string(e.second));
        vector_.push_back(std::make_pair(tran,1.0));
    }

    std::any selected = strategy::RandomStrategy::instance()->select(vector_);
    ltlfuzz::AutomataTransition trans_selected = std::any_cast<ltlfuzz::AutomataTransition> (selected);

    cout << "Selected transition: "<< endl;
    trans_selected.dump();

    return trans_selected;
}
