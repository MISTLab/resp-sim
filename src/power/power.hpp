#ifndef POWER_HPP
#define POWER_HPP

#include <string>
#include <map>


class PowerModel {
    protected:
        std::map<std::string, double> parameters;
        std::map<std::string, double> probes;

    public:

        PowerModel();
        PowerModel( std::map<std::string, double> &params );

        virtual double get_energy() = 0;
        virtual void update_parameters() = 0;

        virtual void set_parameter( std::string name , double value );
        virtual void set_probe( std::string name , double value );

    
};

#endif /* POWER_HPP */
