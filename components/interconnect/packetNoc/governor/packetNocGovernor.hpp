#ifndef __PACKET_NOC_GOVERNOR_H__
#define __PACKET_NOC_GOVERNOR_H__

#include "../packetNoc.hpp"

class packetNocGovernor: public sc_module {
private:
  
  typedef struct {
    int activeBuffers;
    unsigned int currDropped[PORTS];
    unsigned int threshold[PORTS];
  } switch_data_t;
  
	packetNoc*			noc;
	sc_clock			intClock;
	sc_in_clk			clock;

  std::map <unsigned int, switch_data_t*> noc_data;

	void				activity();
	bool enabled;
	unsigned int init_threshold;
	unsigned int max_threshold;
	unsigned int min_threshold;
	unsigned int mult_factor;
	unsigned int red_factor;
	
	
public:
					packetNocGovernor(sc_module_name module_name, packetNoc& pnObj, sc_time cycleLatency);
					~packetNocGovernor();
					void enableGovernor(bool enabled);
					bool isEnabled();
					void printStatus();
					void initialize(unsigned int bufferSize, unsigned int init_threshold, unsigned int max_threshold, 
					                unsigned int min_threshold, unsigned int mult_factor, unsigned int red_factor);
};

#endif
