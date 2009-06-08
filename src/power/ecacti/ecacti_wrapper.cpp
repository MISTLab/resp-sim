#include "def.h"
#include "areadef.h"

#include "ecacti_wrapper.h"
#include <iostream>
/*
typedef struct {
    int cache_size;
    int number_of_sets;
    int associativity;
    int block_size;
    int num_write_ports;
    int num_readwrite_ports;
    int num_read_ports;
    int num_single_ended_read_ports;
    char fully_assoc;
    float fudgefactor;
    float tech_size;
    float VddPow;
} parameter_type;

*/

extern "C" int input_data (int argc, char *argv[], parameter_type *parameters, double *NSubbanks);

extern "C" void evaluate_config(result_type *result, arearesult_type *arearesult,
                            area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks, int *configArgs);
extern "C" void explore_configs(result_type *result, arearesult_type *arearesult,
                            area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks);

extern "C" void init_tech_params(double tech);
extern "C" void output_init_data(parameter_type *parm, double *NSubbanks);

extern "C"void output_data(result_type *result, arearesult_type *arearesult,
			area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks);

resp::cache_power_data resp::get_power_data( int size , int block_size , int associativity, double technology, double subbanks, int wports, int rports, int rwports ) {
    parameter_type parms;
    result_type result;
    arearesult_type arearesult;
    area_type arearesult_subbanked;
    cache_power_data data;

    parms.cache_size = size/((int) subbanks);
    parms.block_size = block_size;
    parms.associativity = associativity;
    //if( associativity == size/block_size ) parms.fully_assoc = 1;
    //else
    parms.fully_assoc = 0;
    parms.number_of_sets = size/(block_size*associativity);

    parms.num_write_ports = wports;
    parms.num_read_ports = parms.num_single_ended_read_ports = rports;
    parms.num_readwrite_ports = rwports;

    parms.fudgefactor = .8/technology;
    parms.tech_size = technology;

    dualVt = 1;
    explore = TRUE;

    init_tech_params(technology);

    //verbose = 1;
    explore_configs(&result,&arearesult,&arearesult_subbanked,&parms,&subbanks);
    data.read_hit = (result.total_power_allbanks.readOp.dynamic+result.total_power_allbanks.readOp.leakage)/FREQ;  // J
    data.write_hit = (result.total_power_allbanks.writeOp.dynamic+result.total_power_allbanks.writeOp.leakage)/FREQ;  // J
    data.read_miss = data.read_hit + (result.tarray_write_power+result.darray_write_power)/FREQ;  // J
    data.write_miss = result.tarray_read_power/FREQ + data.write_hit;  // J
    data.leakage = 0.0;         // W?

    return data;
}

