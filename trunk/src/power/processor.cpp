#include <processor.hpp>
#include <iostream>

SimpleProcessor::SimpleProcessor() {
    this->parameters["idlePower"] = this->idlePower = 0.01;     // mW/MHz
    this->parameters["activePower"] = this->activePower = 0.03; // mW/MHz

    this->probes["idleTime"] = 0.0;                             // ns
    this->probes["frequency"] = 250;                            // MHz
    this->probes["activeTime"] = 0.0;                           // ns


}

void SimpleProcessor::update_parameters() {
        this->idlePower = parameters["idlePower"];
        this->activePower = parameters["activePower"];
}

double SimpleProcessor::get_energy() {
    double energy = (this->probes["idleTime"]*this->idlePower*this->probes["frequency"]
                   + this->probes["activeTime"]*this->activePower*this->probes["frequency"])*1e-9; // ns * mW = nJ = 1e9 mJ

    return energy;
}
