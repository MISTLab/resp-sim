#ifndef __PACKET_NOC_GOVERNOR_H__
#define __PACKET_NOC_GOVERNOR_H__

#include "../packetNoc.hpp"

class packetNocGovernor: public sc_module {
private:
	packetNoc*			noc;
	sc_clock			intClock;
	sc_in_clk			clock;

	void				activity();

public:
					packetNocGovernor(sc_module_name module_name, packetNoc& pnObj, sc_time cycleLatency);
					~packetNocGovernor();
};

#endif
