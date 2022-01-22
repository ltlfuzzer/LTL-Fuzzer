#include <iostream>
#include <string>
#include <set>

#ifndef EVENT_H
#define EVENT_H

namespace ltlfuzz{

enum EventType {ACCEPT,REJECT};

class EventSet{
    
    public:
        std::set<std::string> events;
        ltlfuzz::EventType eventType;
        EventSet(std::set<std::string>  events, const ltlfuzz::EventType eventType){
            this->events = events;
            this->eventType = eventType;
        }
        ~EventSet(){}

        void dump(){
            std::cout << "event name: " << "event Type" << this->eventType <<std::endl;
        }
};
}//namespace
 


#endif
