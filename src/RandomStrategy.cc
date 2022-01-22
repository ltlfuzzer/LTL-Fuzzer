#include <random_strategy.h>
#include <stdlib.h>
#include <time.h>

strategy::RandomStrategy* strategy::RandomStrategy::s_instance;
strategy::RandomStrategy* strategy::RandomStrategy::instance(){
    if(!s_instance)
        s_instance = new RandomStrategy;
    
    return s_instance;
}
strategy::RandomStrategy::RandomStrategy(){}

std::any strategy::RandomStrategy::select(std::vector<std::pair<std::any, double>> m){

    srand (time(NULL)); 

    auto it = m.begin();
    std::advance(it, rand() % m.size());
    return it->first;
}
