#include "packetNocGovernor.hpp"

packetNocGovernor::packetNocGovernor(sc_module_name module_name, packetNoc& pnObj, sc_time cycleLatency):
		sc_module(module_name), intClock("clock", cycleLatency), clock(intClock) {
	noc = &pnObj;
	SC_HAS_PROCESS(packetNocGovernor);
	SC_METHOD(activity);
	sensitive << clock.pos();
	
	std::vector<unsigned int> ids = noc->getSwitchIds();
	for(int i = 0; i < ids.size(); i++)
	  this->currDropped[ids[i]] = 0;
	
	end_module();
}

packetNocGovernor::~packetNocGovernor() {}

void packetNocGovernor::activity() {
	//noc->printStats();
	for(std::map<unsigned int, unsigned int>::iterator mapIt = this->currDropped.begin(); mapIt != this->currDropped.end(); mapIt++){
	   if(mapIt->second < noc->getDropped(mapIt->first)){
	     mapIt->second = noc->getDropped(mapIt->first);
	     noc->changeBufferSize(mapIt->first,1+noc->getBufferSize(mapIt->first));
	     std::cout << "changed buffer size of switch " << mapIt->first << ": " << (noc->getBufferSize(mapIt->first) - 1) << " -> " << noc->getBufferSize(mapIt->first) << std::endl;
	   }
	}
}

