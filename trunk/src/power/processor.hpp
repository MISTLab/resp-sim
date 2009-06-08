#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <power.hpp>

class SimpleProcessor : public PowerModel {
    protected:
        double idlePower;   // W
        double activePower; // W

    public:
        SimpleProcessor();
        double get_energy();
        void update_parameters();
};


#endif /* PROCESSOR_HPP */
