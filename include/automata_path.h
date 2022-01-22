#include <string>
#include <iostream>

#ifndef AUTOMATA_PATH_H
#define AUTOMATA_PATH_H

namespace ltlfuzz{
    class AutomataPath{
        public:
            std::string path;
            
            AutomataPath(std::string path){
                this->path = path;
            }
            ~AutomataPath(){}

            void dump(){
                std::cout << "path: " << this->path << std::endl;
            }
    };
}

#endif
