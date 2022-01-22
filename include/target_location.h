#include <string.h>
#include <iostream>

#ifndef TARGET_LOCATION_H
#define TARGET_LOCATION_H

namespace ltlfuzz{
    enum TargetType{INPUT, OUTPUT};

    class TargetLocation{
        
        public:
            TargetType targetType;
            //fileName:lineNum
            std::string targetName;
            //std::string distance_path;

            TargetLocation(ltlfuzz::TargetType targetType, std::string targetName){

                this->targetType = targetType;
                this->targetName = targetName;
            }
            ~TargetLocation(){}
            
            void dump(){
                std::cout << "Target type: " << this->targetType <<std::endl; 
                std::cout << "Target name: " << this->targetName << std::endl;
            }
    };
}

#endif 
