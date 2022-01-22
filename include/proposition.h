#include <set>

#ifndef PROPOSITION_H
#define PROPOSITION_H
namespace ltlfuzz{
class Proposition{
    public:
        std::set<std::string> events;

        Proposition(std::set<std::string> events){
            this->events = events;
        }
        
};//
}

#endif
