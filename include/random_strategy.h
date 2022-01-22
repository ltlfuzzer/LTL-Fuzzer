#include <strategy.h>
#include <vector>

#ifndef RANDOM_STRATEGY_H
#define RANDOM_STRATEGY_H
namespace strategy{

class RandomStrategy: public Strategy {
    public:
        std::any select(std::vector<std::pair<std::any, double>> v);
        static RandomStrategy* instance();

    private:
        static RandomStrategy* s_instance;
        RandomStrategy();
};
}//namespace

#endif
