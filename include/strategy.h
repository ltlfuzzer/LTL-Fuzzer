/*
 * strategy takes a set of objects for each of which has a fitness value 
 * (double type) and returns an object according to implemented strategies
 */
#include <vector>
#include <any>

#ifndef STRATEGY_H
#define STRATEGY_H
namespace strategy{

class Strategy{
    public:
        Strategy(){};
        ~Strategy(){};
        virtual std::any select(std::vector<std::pair<std::any, double>> m) = 0;
};
} //namepsace
#endif
