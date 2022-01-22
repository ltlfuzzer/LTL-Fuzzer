#include <shared_table.h>
#include <string>
#include <vector>
#include <automata_path.h>
#include <automata_transition.h>
#include <prefix.h>
#include <target_location.h>
#include "targetstore.h"
#include <any>
#include "strategy.h"
#include "random_strategy.h"
#include "utils.h"
#include "automata.h"

#ifndef PATHSTORE_H
#define PATHSTORE_H

using namespace boost::interprocess;

namespace path{

class PathsStore{
    
    public:
        PathsStore(managed_shared_memory* segment);
        PathsStore();
        ~PathsStore();
        
        void insert_init_automata_path(std::string automata_path, std::string prefix, std::string metric);
        std::pair<ltlfuzz::Prefix, ltlfuzz::AutomataPath> select_prefix_aPath();
        std::pair<std::string, std::string> select_automataPath_and_prefix(std::string prefixDir);

        
        //data checking
        int getSize();
        void dump();
        static void clean_up();


    private:
        static int init_run;
        managed_shared_memory* segment;
        string_string_string_map*                  table;        //For RERS

        string_string_string_map::iterator         selected_item;

        string_string_string_map::iterator         select_automata_path();
        ltlfuzz::AutomataPath                      get_selected_automata_path();
        ltlfuzz::Prefix                            select_prefix();
        ltlfuzz::AutomataTransition                select_transition(const lfz::automata::Automata &atm);
        double                                     compute_prefix_fitness(ltlfuzz::Prefix prefix);
        std::string                                get_automataPath(std::vector<std::string> &fileList);
        std::string                                get_prefix(std::string aPath, std::string prefixDir);

};
}//namespace

#endif
