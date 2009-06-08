#ifndef ECACTI_WRAPPER_H
#define ECACTI_WRAPPER_H

namespace resp {


class cache_power_data{
public:
    double read_hit;    // J
    double read_miss;   // J
    double write_hit;   // J
    double write_miss;  // J
    double leakage;     // W

    cache_power_data() {
        read_hit = read_miss = write_hit = write_miss = leakage = 0;
    }

    cache_power_data(const cache_power_data &d) {
        read_hit = d.read_hit;
        read_miss = d.read_miss;
        write_hit = d.write_hit;
        write_miss = d.write_miss;
        leakage = d.leakage;
    }

};

cache_power_data get_power_data( int size , int block_size , int associativity, double technology, double subbanks =1.0, int wports = 0, int rports = 0, int rwports =1 );

}

#endif /* ECACTI_WRAPPER_H */
