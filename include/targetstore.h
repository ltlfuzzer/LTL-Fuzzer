#include <string>
#include <strategy.h>
#include <map>
#include <automata_transition.h>
#include <iostream>
#include <target_location.h>
#include <event.h>
#include <set>
#include <random_strategy.h>
#include <algorithm>
#include <fstream>
#include <sstream>

#ifndef TARGET_STORE_H
#define TARGET_STORE_H

namespace ltlfuzz{

class TargetsStore{
        public:
            
            void load_targets(std::string filePath, int flag);
            void load_events(std::string fileName);
            static TargetsStore* instance();
            TargetLocation getTarget(std::string event, int flag);
            void dump_event_target();
            std::string decode(std::string input);
            std::map<std::string, std::string> event_map;

        private:
            static TargetsStore *s_instance;
            std::map<std::string, std::set<std::string>> event_target_map;

            TargetsStore();
            TargetLocation get_target_from_event(std::string e, int flag);
            TargetType get_target_type(std::string target, int flag);
            std::string coding(std::string value);

    };
}
#endif
