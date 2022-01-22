#include "shared_table.h"
#include <string.h>

#ifndef PATHWRITER_H
#define PATHWRITER_H
using namespace boost::interprocess;

namespace inst{

class PathWriter{
    public:
        PathWriter();
        ~PathWriter();
        
        static void write_to_shared_table(std::string automata_path, std::string prefix, std::string metric);
        
};
} //namespace
#endif
