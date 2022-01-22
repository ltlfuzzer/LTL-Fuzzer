#include <targetstore.h>


ltlfuzz::TargetsStore* ltlfuzz::TargetsStore::s_instance;

ltlfuzz::TargetsStore* ltlfuzz::TargetsStore::instance(){

    if(!s_instance){
        s_instance = new ltlfuzz::TargetsStore;
    }
    return s_instance;
}

ltlfuzz::TargetsStore::TargetsStore(){

}

//flag: 0 for commong programs; 1 for protocols
void ltlfuzz::TargetsStore::load_targets(std::string filePath, int flag){

    std::ifstream fileReader(filePath);
    if(!fileReader.is_open()) throw std::runtime_error("could not open the file");

    std::string line;
    std::string delimiter =":";

    while(std::getline(fileReader, line)){

        std::string location = line.substr(0, line.find_last_of(delimiter));
        std::string event_s = line.substr(line.find_last_of(delimiter)+delimiter.length(), line.length());

    
        std::string event = event_s;
        if(!flag){
            event = "o" + coding(event_s);
        }
         
        auto it = s_instance->event_target_map.find(event);
        
        if(it == s_instance->event_target_map.end()){
            s_instance->event_target_map.insert({event, std::set<std::string>{location}});
        }
        else{
            it->second.insert(location);
        }
    }
    
    fileReader.close();
    std::cout << "The number of loading events: " << ((s_instance->event_target_map).size()) << std::endl;
}

void ltlfuzz::TargetsStore::load_events(std::string fileName){
    std::ifstream fileReader(fileName);
    if(!fileReader.is_open()) throw std::runtime_error("could not open the event_file");

    std::string line;
    while(std::getline(fileReader, line)){
        std::stringstream input(line);
        std::string result;
        input >> result;
        std::string key = result;
        input >> result;
        std::string value = result;
        s_instance->event_map[key] = value;
    }
    fileReader.close();
    
    //test information
    std::cout << "------------------------------Test Event---------------------" << std::endl;
    std::map<std::string, std::string>::iterator it;
    for(it = s_instance->event_map.begin(); it != s_instance->event_map.end(); it++){
        std::cout << (it->first) << "\t" << (it->second) << std::endl;
    }
    std::cout << "Event_load success" << std::endl;
}

//flag: 0 for RERS; 1 for protocols
ltlfuzz::TargetLocation ltlfuzz::TargetsStore::getTarget(std::string event, int flag){
    return s_instance->get_target_from_event(event, flag);
}

ltlfuzz::TargetLocation ltlfuzz::TargetsStore::get_target_from_event(std::string event, int flag){

    if(get_target_type(event, flag)==ltlfuzz::TargetType::INPUT){
        return ltlfuzz::TargetLocation(ltlfuzz::TargetType::INPUT, event);
    }

    auto iter=s_instance->event_target_map.find(event);

    if(iter == s_instance->event_target_map.end()){
        std::cout << "Corresponding target does not exist" <<std::endl;
        exit (EXIT_FAILURE);
    }
    std::vector<std::pair<std::any, double>> vector_;
        
    for(auto& e : iter->second){
        vector_.push_back(std::make_pair(e, 1.0));
    }
    std::any selected = strategy::RandomStrategy::instance()->select(vector_);
    std::string target = std::any_cast<std::string> (selected);
    

    return ltlfuzz::TargetLocation(get_target_type(target, flag), target);
}

ltlfuzz::TargetType ltlfuzz::TargetsStore::get_target_type(std::string target, int flag){
    if(flag){
        return ltlfuzz::TargetType::OUTPUT;
    }

    if(target.front()=='i')
        return ltlfuzz::TargetType::INPUT;
    else
        return ltlfuzz::TargetType::OUTPUT;
}

void ltlfuzz::TargetsStore::dump_event_target(){

    for(auto& e_t : s_instance->event_target_map){

        std::cout <<"Event: "<< e_t.first<<std::endl;

        for(auto t : e_t.second){
            std::cout << "target:" << t <<std::endl;
        }
    }

}


std::string ltlfuzz::TargetsStore::coding(std::string value){
    std::map<std::string, std::string>::iterator it;
    for(it = s_instance->event_map.begin(); it != s_instance->event_map.end(); it++){
        if(it->second == value){
            return it->first;
        }
    }
    if(it == s_instance->event_map.end()){
        throw std::runtime_error("could not find the event");
    }
}

std::string ltlfuzz::TargetsStore::decode(std::string input){
    std::string str = input.substr(1);
    std::string value = s_instance->event_map[str];
    return value;
}
