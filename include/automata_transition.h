#include <string.h>
#include <iostream>
#include <set>
#include <vector>

#ifndef TRANSITION_H
#define TRANSITION_H

namespace ltlfuzz{
    class AutomataTransition{
        public:
            std::vector<std::set<std::string>> propositions;
            std::string destState;

            AutomataTransition(std::vector<std::set<std::string>> props, std::string destState){
                this->propositions = props;
                this-> destState = destState;
            }

            void dump(){
                std::cout << "next state: " << this->destState << std::endl;
                for(auto& it: this->propositions)
                    for(auto& e : it)
                        std::cout << "proposition: " << e << std::endl;
            }
    };
}

#endif
