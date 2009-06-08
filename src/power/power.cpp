
#include <power.hpp>
#include <utils.hpp>

PowerModel::PowerModel() {
}

PowerModel::PowerModel( std::map<std::string, double> &params ) {
}


void PowerModel::set_parameter( std::string name , double value ) {
    if ( this->parameters.find(name) == this->parameters.end() )
        THROW_EXCEPTION( "Invalid Parameter "+name );

    this->parameters[name] = value;
}

void PowerModel::set_probe( std::string name , double value ) {
    if ( this->probes.find(name) == this->probes.end() )
        THROW_EXCEPTION( "Invalid Probe "+name );
    
    this->probes[name] = value;
}
