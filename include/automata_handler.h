#include <iostream>
#include <automata.h>
#include <automata_transition.h>
#include <event.h>
#include <random_strategy.h>
#include <proposition.h>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>

#ifndef AUTOMATA_HANDLER_H
#define AUTOMATA_HANDLER_H

namespace ltlfuzz{
    extern std::set<std::string> ALL_EVENTS; //all exclusive events
	extern void load_ALL_EVENTS(std::string fileName); 
    class AutomataHandler{
        public:
            AutomataHandler(lfz::automata::Automata* atm);
            std::string select_event(std::string curState, std::string aPath);
            
        private:
            lfz::automata::Automata* atm;
            
            AutomataTransition select_tran(std::string curState, std::string aPath);
            Proposition select_proposition(AutomataTransition tran);
            EventSet extract_proposition_events(Proposition prop);
            std::string select_event(EventSet eSet);

    };
}

#endif 

