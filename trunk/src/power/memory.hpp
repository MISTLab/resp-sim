#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <power.hpp>
#include <string>
#include <vector>
#include <map>

class InterpolatedMemory : public PowerModel {
    protected:
        double renergy;
        double wenergy;
        double leakage;

        double c_read[5], c_write[5], c_leakage[5];

    public:
        InterpolatedMemory();
        double get_energy();
        void update_parameters();
};


#endif /* CACHE_HPP */
