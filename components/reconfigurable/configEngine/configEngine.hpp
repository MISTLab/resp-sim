/*
 *	Configuration Engine, performs reconfiguraion on the eFpgas attached using TLM ports.
 *	Also used to catch calls to reconfigured functions. Can be also done using Python.
 *	See /architectures/test/test_reconfig.py for an example
 */
#ifndef  _CONFENG_H
#define  _CONFENG_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "reconfEmulator.hpp"

#include "../payloadData.hpp"
#include "cEAllocationTable.hpp"

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <boost/lexical_cast.hpp>
#include <trap_utils.hpp>

using namespace std;
using namespace trap;
using namespace tlm;
using namespace tlm_utils;
using namespace reconfEmu;

class configEngine: public sc_module {

private:
	unsigned long long bitstream_address;
	sc_time requestDelay, execDelay, configDelay, removeDelay;

	// The centralized map of the configured functions
	cEAllocationTable tab;

	// Dealing with multiple configuration accesses
	sc_event configFree;
	bool busy;

//	int status;

public:
//	void printStatus() {cout << "Configuration Engine Status: " << status << endl;}
//	void changeStatus(int nw) {status = nw;}

	// This is the Emulator used to perform the calls to hardware methods
	reconfEmulator<unsigned int> recEmu;
	
	// This is the port communicating with the devices
	multi_passthrough_initiator_socket< configEngine, 32 > initiatorSocket;

	// This is the port attached to the system main bus
//	tlm_initiator_port <	tlm_transport_if <	tlm_request<unsigned int, unsigned int>,
//							tlm_response<unsigned int>	> 	> bus_init_port;

	/*
	 * Constructor:
	 * - 'delAlg' specifies the type of deletion algorithm to be used if a
	 * configuration is required and there is no more space on the fabrics.
	 / 
	 */
	configEngine(sc_module_name name, ABIIf<unsigned int> &processorInstance, unsigned long long configAddress,
		deletionAlgorithm delAlg = LRU);


	/* 
	 * Configures a fabric with function 'funcName', which has the given 'latency' and occupies 'width' X 'height' logic cells;
	 * furthermore, latency and bus occupation required by the (eventual) configuration are managed by this method.
	 * Returns the address at which the function is mapped. If some error happens, reserved address 0 is returned.
	 * If 'reUse' is activated, the address of a previously allocated function with the same name (if exists) is
	 * returned and no new functions are configured.
	 */
	unsigned int configure(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse = true);

	/*
	 * Executes the function at the given 'address' on the fabric and waits for the appropriate latency.
	 * Returns FALSE if the address is empty or some error happens. TRUE elsewhere.
	 */
	bool execute(unsigned int funcAddr);

	/* 
	 * Configures and executes function 'funcName', which has the given 'latency' and occupies 'width' X 'height' logic cells;
	 * furthermore, latency and bus occupation required by the (eventual) configuration are managed by this method.
	 * Returns FALSE if some error happens. TRUE elsewhere.
	 * If 'reUse' is activated, a previously allocated function with the same name (if exists) is executed
	 * and no new functions are configured.
	 */
	bool confexec(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse = true);

	/*
	 * Removes the function at the address 'funcAddr' (with the given 'funcName') on the fabric.
	 * Returns FALSE if the address (name) is empty or some error happens. TRUE elsewhere.
	 */
	bool manualRemove(unsigned int funcAddr);
	bool manualRemove(string funcName);

	/*
	 * Prints on the standard output the internal status of the entire system.
	 */
	void printSystemStatus();

	/*
	 * Setters for the internal fixed latencies (initialized to 0)
	 */
	void setRequestDelay(sc_time new_time);
	void setExecDelay(sc_time new_time);
	void setConfigDelay(sc_time new_time);
	void setRemoveDelay(sc_time new_time);

	void registerPythonCall(string funName, reconfCB<unsigned int> &callBack);
	void registerCppCall(string funName, sc_time latency = SC_ZERO_TIME, unsigned int w=1, unsigned int h=1);

};

#endif  //_CONFENG_H
