#include <codebean.h>

std::string inst::CodeBean::SHM_ENV_VAR = std::string("__AFL_SHM_ID");
int inst::CodeBean::MAP_SIZE = 65536 + 16; //shm for AFL+AFLGO
int inst::CodeBean::offset=64;
std::string inst::CodeBean::delimiter=std::string(",");
std::string inst::CodeBean::delimiter_prefix=std::string(";;");
std::string inst::CodeBean::delimiter_ltl=std::string(":");
std::string inst::CodeBean::distance_file_path="distance.cfg.txt";
std::map<std::string, int32_t> inst::CodeBean::distance_map;
std::vector<int> inst::CodeBean::trace_common;
std::vector<std::string> inst::CodeBean::trace_protocol;
std::vector<std::string> inst::CodeBean::input_protocol;
std::vector<size_t> inst::CodeBean::state_vector; 
std::string inst::CodeBean::DRY_RUN_ENV="DRY_RUN";
std::map<int, std::string> inst::CodeBean::event_reverse_map; 
std::vector< std::pair<std::string, int> > inst::CodeBean::prop_loc_vec;
std::vector<std::string> inst::CodeBean::trace_string;

lfz::automata::Automata automata;


void inst::CodeBean::init_distance_map(){
    
    std::fstream readFile;
    readFile.open(distance_file_path, std::ios::in);
    if(readFile.is_open()){
        std::string line;
        while(std::getline(readFile, line)){
            std::string block_id = line.substr(0, line.find(delimiter));
            std::string distance_s = line.substr(line.find(delimiter)+delimiter.length(), line.length());

            int32_t distance = (int32_t) atoi(distance_s.c_str());
            
            distance_map.insert({block_id, distance});
        }
    }
}

int32_t inst::CodeBean::get_distance_to_target(char* block_id){

    if(distance_map.size()==0)
    {
        //std::cout <<"initialize the distance table ..." << std::endl;
        init_distance_map();
    }
    auto it = distance_map.find(std::string(block_id));

    if(it == distance_map.end()){
		int32_t var = 100;        
		return var;
    }
        
    return distance_map[std::string(block_id)];
}

//For RERS
void inst::CodeBean::collect_trace(int input, int output){
    trace_common.push_back(input);
    trace_common.push_back(output);
    std::cout << "input: " << input << "; output: " << output << std::endl;
}

void inst::CodeBean::collect_state(long *ptr, int *size, int num){

    int total_len = 0;
	for(int i = 0; i < num; i++){
		total_len += size[i];
	}

	unsigned char state_value[total_len];
    int cur_len = 0;
	for(int i = 0; i < num; i++){
		if(i == 0){
			memcpy(state_value, (void*)ptr[i], size[i]);
		}
		else{
			memcpy(state_value + cur_len, (void*)ptr[i], size[i]);
		}
        cur_len += size[i];
	}

    size_t hash_value = boost::hash_range(state_value, state_value + total_len);
    state_vector.push_back(hash_value);
}

//For protocol
void inst::CodeBean::collect_proposition(std::string prop){
    std::cout << "prop: " << prop << std::endl;
    trace_protocol.push_back(prop);
    std::pair<std::string, int> node;
    node = std::make_pair(prop, input_protocol.size());
    prop_loc_vec.push_back(node);
}

void inst::CodeBean::collect_input(std::string input){
    input_protocol.push_back(input);
}

void inst::CodeBean::load_event_reverse_map(){
    char* curDir = getenv("SUBJECT");
    if(curDir == NULL){
        std::cout << "Please specify the SUBJECT direcotry" << std::endl;
        return;
    }
    std::string str(curDir);
    std::string fileName = str + std::string("event_map_dir/event_mapping.txt");
    std::ifstream fileReader(fileName);
    if(!fileReader.is_open()) throw std::runtime_error("could not open the event_file at evaluate_trace");

    std::string line;
    while(std::getline(fileReader, line)){  //int to string 
        std::stringstream input(line);
        std::string result;
        input >> result;
        std::string value = result;
        input >> result;
        int key = std::stoi(result);
        event_reverse_map[key] = value;
    }
    fileReader.close();
}

void inst::CodeBean::normalize_trace(){
    
    for(long unsigned int i=0; i<trace_common.size(); i++){
        std::string prefix="";
        if(i%2==0) 
            prefix="i";
        else
            prefix="o";

        int key = trace_common[i];
		//map output-0 to the event oinvalid
        trace_string.push_back(std::string(prefix+event_reverse_map[key]));
    }

}

void inst::CodeBean::init_shared_memory(){

    char* id_str = getenv(SHM_ENV_VAR.c_str());
    if (id_str) {

        uint32_t shm_id = atoi(id_str);
        uint8_t*  __afl_area_ptr = (uint8_t *) shmat(shm_id, NULL, 0);
       
        if (__afl_area_ptr == (void *)-1) {

            exit(1);
        }

        memset(__afl_area_ptr, 0, MAP_SIZE);
        __afl_area_ptr[0] = 1;

    }
    else{
        std::cout << "failed to get shmd id" <<std::endl;
    }
}

void inst::CodeBean::saved_prefix_path(std::string aPath, std::string prefix){
    char* curDir = getenv("SUBJECT");
    if(curDir == NULL){
        std::cout << "Please speficy the SUBJECT direcotry" << std::endl;
        return;
    }
    std::string str(curDir);

    std::string dirName = str + std::string("prefix/");
    std::string fileName = dirName + aPath;
    std::ofstream out;
	out.open(fileName, std::ios::app);
	if(out.is_open()){
		if(prefix != ""){
		    out << prefix << "@@";
        }
		out.close();
	}
	else{
		std::cout << "failed to create" << std::endl;
	}
}

//flag: 0 for commong programs; 1 for protocols
void inst::CodeBean::evaluate_trace(int flag){
    char* curDir = getenv("SUBJECT");
    if(curDir == NULL){
        std::cout << "Please specify the SUBJECT direcotry" << std::endl;
        return;
    }
    std::string str(curDir);
    std::cout << "come to evaluating_trace...." << std::endl;
    std::string ltl_formula_file = str + std::string("ltl_dir/ltl.txt");
    std::ifstream ifs(ltl_formula_file);
    std::string content( 
            (std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>()));
     
    std::string formula = content.substr(0, content.find(delimiter_ltl));
    std::string exclusive_events = content.substr(content.find(delimiter_ltl)+delimiter_ltl.length(), content.length());
    std::cout << "formula is:  " << formula << std::endl;
    std::cout << "exclusive events:" << exclusive_events <<std::endl;

    automata.set_formula(formula, exclusive_events);
    std::vector<lfz::automata::MCState> states;
    
    if(flag){
        automata.model_check_events(trace_protocol, states);
    }
    else{
        load_event_reverse_map();
        normalize_trace();
        automata.model_check_events(trace_string, states);
    }

    std::string aPath = "";
    std::string prefix = "";
    extract_prefix_automata_path(states, aPath, prefix, flag);

    check_acceptance(states, aPath, flag);

    if(getenv(DRY_RUN_ENV.c_str()) ==nullptr || std::string(getenv(DRY_RUN_ENV.c_str()))=="1"){
        std::cout<< "using DRY RUN model " << std::endl;
        return;
    }
    if(!aPath.empty()){
        if(!flag){
            PathWriter::write_to_shared_table(aPath, prefix, "1");
        }
        else{
            saved_prefix_path(aPath, prefix);
        }
    }
	
}

int inst::CodeBean::get_last_state(std::string aPath){
    aPath=aPath.substr(0,aPath.size()-1);

    std::size_t found = aPath.find_last_of(delimiter);
    std::string state = aPath.substr(found+1);

    return std::stoi(state);
}

void inst::CodeBean::check_conditions(std::vector<std::set<std::string>> events, unsigned int begin_loc, unsigned int end_loc, int flag){
    
    for(auto&e : events){             //e: set; events: vector;
        for(auto&e2 : e){             //e2: event
            if(e2.front() == '!'){
                std::string c_evt = e2.substr(1);
                for(unsigned int i = begin_loc; i <= end_loc; i++){
                    if(flag){
                            if(c_evt == trace_protocol[i]){
                            goto end;
                            }
                        }
                        else{
                            if(c_evt == trace_string[i]){
                                goto end;
                            }
                        }

                }
            }
            else{
                std::string c_evt = e2;
                for(unsigned int i = begin_loc; i <= end_loc; i++){
                    if(flag){
                        if(c_evt != trace_protocol[i]){
                            goto end;
                        }
                    }
                    else{
                        if(c_evt != trace_string[i]){
                            goto end;
                        }
                    }
                }
            }
        }
        throw std::runtime_error("a counterexample!");
        end:
            continue;

    }
}

void inst::CodeBean::check_acceptance(std::vector<lfz::automata::MCState> states, std::string aPath, int flag){
    unsigned int loc = 0;
    for(auto& state : states){
        if(state.acceptance){
            int last_state = inst::CodeBean::get_last_state(aPath);
            std::cout << "last state: " << last_state << std::endl;
            
            lfz::automata::transitions_t trans; //std::vector<std::pair<std::vector<std::set<std::string>>, int>>
            automata.get_state_transitions(last_state, trans);
            std::vector<std::set<std::string>> events;
            for(auto&e : trans){
                if(e.second == last_state){
                    events = e.first;
                }
            }
            if(events.empty()){
                throw std::runtime_error("a counterexample!");
            }
            
            unsigned int sloc = 0;
            if(flag){
                sloc = loc;
            }
            else{
                sloc = loc/2;
            }

            size_t pstate = state_vector[sloc];
            for(unsigned int i = sloc + 1; i < state_vector.size(); i++){
                if(pstate == state_vector[i]){
                    if(flag){
                        check_conditions(events, loc, i, 1);
                    }
                    else{
                        check_conditions(events, loc, 2*i+1, 0);
                    }

                }
            }
        }
        ++loc;
    }
}

void inst::CodeBean::extract_prefix_automata_path(std::vector<lfz::automata::MCState> states, std::string& aPath, std::string& prefix, int flag){
    int loc=0;
    int curState=-2;
    for(size_t i=0; i<states.size(); i++){
        if(states[i].state==-1){
            break;
        }
        if(curState!=states[i].state){
            curState=states[i].state;
			if(!flag){
				for(size_t j = loc + 1; j < i; j++){
		            if(j%2 == 0 && trace_common[j+1] > 0){
		                prefix = prefix + std::to_string(trace_common[j]) + delimiter;
		            }
            	}
			}
            loc=i;
            aPath= aPath + std::to_string(states[i].state) + delimiter; 
			if(!flag){
				if(i%2 == 0){
		            prefix = prefix + std::to_string(trace_common[i]) + delimiter;
		        }
			}
        }
    }
    
    if(flag && !aPath.empty()){
        int location = prop_loc_vec[loc].second;
        for(int j = 0; j < location; j++){
            prefix = prefix + input_protocol[j] + delimiter_prefix;
        }
    }
   
    std::cout << "aPath: " << aPath << "; prefix: " << prefix << std::endl;
    
}

