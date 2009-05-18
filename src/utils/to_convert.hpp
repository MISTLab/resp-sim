#include <map>
#include <string>
#include <vector>
#include <list>

#include <systemc.h>
#include <tlm.h>

using namespace tlm;

namespace converters{
    inline unsigned __instantiate_converters(){
        unsigned int dummy = 0;

        dummy += unsigned(sizeof(std::map<std::string, sc_time>));
        dummy += unsigned(sizeof(std::map<unsigned int, sc_time>));
        dummy += unsigned(sizeof(std::map<int, double>));
        dummy += unsigned(sizeof(std::vector<std::string>));

        return dummy;
    }
}
