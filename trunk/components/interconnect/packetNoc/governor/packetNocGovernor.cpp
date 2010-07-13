#include "packetNocGovernor.hpp"

packetNocGovernor::packetNocGovernor(sc_module_name module_name, packetNoc& pnObj, sc_time cycleLatency):
		sc_module(module_name), intClock("clock", cycleLatency), clock(intClock) {
	noc = &pnObj;
	SC_HAS_PROCESS(packetNocGovernor);
	SC_METHOD(activity);
		sensitive << clock.pos();
	end_module();
}

packetNocGovernor::~packetNocGovernor() {}

void packetNocGovernor::activity() {
	noc->printStats();
}

