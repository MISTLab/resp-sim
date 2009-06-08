
#include <math.h>
#include <memory.hpp>

InterpolatedMemory::InterpolatedMemory()  {
    this->parameters["size"] = 16384;          // Bytes
    this->parameters["line_width"] = 4;        // Bytes
    this->parameters["word_size"] = 32;        // Bits

    this->probes["current_time"] = 0;
    this->probes["num_reads"] = 0;
    this->probes["num_writes"] = 0;

    this->renergy = 23.21;
    this->wenergy = 27.34;
    this->leakage = 2001.07;
 
    this->c_read[0] =  -1.09167887e-05; this->c_read[1] = 6.05436716e-01; this->c_read[2] = 2.04028667e-02 ; this->c_read[3] =  2.76256799e-06 ; this->c_read[4] =  3.49943207e+00;
    this->c_write[0] =  -1.79541119e-05 ; this->c_write[1] = 7.16287132e-01 ; this->c_write[2] =  3.25601331e-02 ; this->c_write[3] = 3.91476756e-06 ; this->c_write[4] = 3.94571979e+00;
    this->c_leakage[0] = 2.92556486e-02 ; this->c_leakage[1] =  1.05218644e+01 ; this->c_leakage[2] =  2.02997888e+00 ; this->c_leakage[3] =  1.86243641e-03 ; this->c_leakage[4] =  1.27200413e+03;
}

void InterpolatedMemory::update_parameters() {
    unsigned int words = (this->parameters["size"]/this->parameters["line_width"]);
    unsigned int bits = this->parameters["word_size"];
    unsigned int area = words * bits;
    unsigned int mux = log2(words) + log2(this->parameters["line_width"]);

    this->renergy = words*this->c_read[0] + bits*this->c_read[1] + mux*this->c_read[2] + area*this->c_read[3] + this->c_read[4];
    this->wenergy = words*this->c_write[0] + bits*this->c_write[1] + mux*this->c_write[2] + area*this->c_write[3] + this->c_write[4];
    this->leakage = words*this->c_leakage[0] + bits*this->c_leakage[1] + mux*this->c_leakage[2] + area*this->c_leakage[3] + this->c_leakage[4];
}

double InterpolatedMemory::get_energy() {
    // No need to interpolate every time, use the stored values
    double current_time = this->probes["current_time"];

    // leakage (uW) * current_time (ns) => fJ => pJ * 1e-3
    double energy =  (this->probes["num_reads"]*this->renergy
                    + this->probes["num_writes"]*this->wenergy
                    + this->leakage*current_time*1e-3)*1e-9;     // pJ -> mJ

    return energy;
}
