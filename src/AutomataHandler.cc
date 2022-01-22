#include <automata_handler.h>
 
std::set<std::string> ltlfuzz::ALL_EVENTS;


ltlfuzz::AutomataHandler::AutomataHandler(lfz::automata::Automata* atm){
    this->atm = atm;
}

void ltlfuzz::load_ALL_EVENTS(std::string fileName){
    std::ifstream fileReader(fileName);
    std::cout << "filename is: " << fileName << std::endl;
    if(!fileReader.is_open()) throw std::runtime_error("could not open the event_file at Load_all_events");
    std::cout << "------------Loding ALL Events--------- " << std::endl;
    std::string line;
    while(std::getline(fileReader, line)){
        std::stringstream input(line);
        std::string result;
        input >> result;
        std::cout << "event: " << result << std::endl;
        ltlfuzz::ALL_EVENTS.insert(result);
    }
    std::cout << "-----------------loading Done: " << ALL_EVENTS.size() << "--------------" << std::endl;
    fileReader.close();
}

std::string ltlfuzz::AutomataHandler::select_event(std::string curState, std::string aPath){
    ltlfuzz::AutomataTransition tran = select_tran(curState, aPath);
    ltlfuzz::Proposition prop = select_proposition(tran);
    ltlfuzz::EventSet eSet = extract_proposition_events(prop);
    return select_event(eSet);
}

ltlfuzz::AutomataTransition ltlfuzz::AutomataHandler::select_tran(std::string curState, std::string aPath){
    std::set<int> sPath;
    std::vector<std::string> vPath;
    aPath = aPath.substr(0, aPath.size()-1);
    boost::split(vPath, aPath, [](char c){return c == ',';});
    for(auto& e : vPath){
        sPath.insert(std::stoi(e));
    }

    for(auto&e : sPath){
        std::cout << ">>> test existing path: " << e << std::endl;
    }
    
    int state = std::stoi(curState);
    lfz::automata::transitions_t trans;

    this->atm->get_state_transitions(state, trans);


    std::cout << "debug: " << curState << std::endl;
    std::vector<std::pair<std::any, double>> vector_;

    for(auto& e : trans){
        std::set<int>::iterator it;
        it = sPath.find(e.second);
        std::cout << "next state: " << e.second << std::endl;
        if(it == sPath.end()){
            ltlfuzz::AutomataTransition tran(e.first, std::to_string(e.second));
            vector_.push_back(std::make_pair(tran,1.0));
        }
    }

    if(vector_.size() == 0){
        for(auto& e : trans){
            ltlfuzz::AutomataTransition tran(e.first, std::to_string(e.second));
            vector_.push_back(std::make_pair(tran,1.0));
        }
    }

    std::any selected = strategy::RandomStrategy::instance()->select(vector_);
    ltlfuzz::AutomataTransition trans_selected = std::any_cast<ltlfuzz::AutomataTransition> (selected);


    std::cout << "Selected transition: "<< std::endl;
    trans_selected.dump();

    return trans_selected;


}

ltlfuzz::Proposition ltlfuzz::AutomataHandler::select_proposition(ltlfuzz::AutomataTransition tran){
    std::vector<std::pair<std::any, double>> vector_;

    for(auto& e : tran.propositions){
        ltlfuzz::Proposition prop(e);
        vector_.push_back(std::make_pair(prop,1.0));
    }

    std::any selected = strategy::RandomStrategy::instance()->select(vector_);
    ltlfuzz::Proposition prop = std::any_cast<ltlfuzz::Proposition> (selected);

    return prop;
}

ltlfuzz::EventSet ltlfuzz::AutomataHandler::extract_proposition_events(ltlfuzz::Proposition prop){
    std::set<std::string> eSet;
    for(auto e : prop.events){

        if(e.front() == '!'){
            eSet.insert(e.erase(0));
        }
        else{
            eSet.clear();
            eSet.insert(e);
            return ltlfuzz::EventSet(eSet, ltlfuzz::EventType::ACCEPT);
        }

    }
    return ltlfuzz::EventSet(eSet, ltlfuzz::EventType::REJECT);
}

std::string ltlfuzz::AutomataHandler::select_event(ltlfuzz::EventSet eSet){

     std::vector<std::pair<std::any, double>> vector_;

     switch (eSet.eventType){
         case ltlfuzz::EventType::ACCEPT :
             for( auto& e : eSet.events ){
                 vector_.push_back(std::make_pair(e, 1.0));
             }
             break;

         case ltlfuzz::EventType::REJECT :
             std::set<std::string> accept_events;
             std::set_difference(ltlfuzz::ALL_EVENTS.begin(), ltlfuzz::ALL_EVENTS.end(), eSet.events.begin(), eSet.events.end(), std::inserter(accept_events, accept_events.end()));

             for(auto& e : accept_events){
                 vector_.push_back(std::make_pair(e, 1.0));
             }
             break;
     }

     std::any selected = strategy::RandomStrategy::instance()->select(vector_);
     std::string event = std::any_cast<std::string> (selected);

     return event;
}
