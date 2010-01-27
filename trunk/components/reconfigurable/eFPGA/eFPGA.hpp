/**
 * eFPGA component
 */
#ifndef  _EFPGA_H
#define  _EFPGA_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../payloadData.hpp"
#include "eFPGAAllocationTable.hpp"

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

class eFPGA: public sc_module {

private:
	//Parameters
	float wordarea, cellarea;
	sc_time latword;


public:
	//Component name
	string name;

	//The local allocation table
	eFPGAAllocationTable tab;

	//This is the port receiving the communication
	simple_target_socket < eFPGA, 32 > targetSocket;

	//Constructor
	eFPGA(sc_module_name name, unsigned int w, unsigned int h, sc_time lw = SC_ZERO_TIME, float wa = 1, float ca = 1);

	//Transport method
	void b_transport(tlm_generic_payload& message, sc_time& delay);

	// Prints on the standard output the internal status of the eFPGA.
	void printStatus();
};

#endif  //_EFPGA_H
