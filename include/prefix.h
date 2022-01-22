#include <string.h>
#include <iostream>

#ifndef PREFIX_H
#define PREFIX_H

namespace ltlfuzz{
    class Prefix{
        public:
            std::string prefix;
            std::string feedback;

            Prefix(std::string prefix, std::string feedback){
                this->prefix = prefix;
                this->feedback = feedback;
            }
            void dump(){
                std::cout << "prefix: " << this->prefix << std::endl;
                std::cout << "feedback: " << this->feedback << std::endl;
            }
    };
}

#endif
