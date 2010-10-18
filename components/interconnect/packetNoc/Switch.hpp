#ifndef __SWITCH_H__
#define __SWITCH_H__

#include "nocCommon.hpp"
#include "Buffer.hpp"

typedef map<unsigned int, unsigned int> portToSwitchMap_t;
typedef map<unsigned int, bool> destActiveMap_t;

class Switch: public sc_module {
private:
	// Variables
	unsigned int			local_id;
	Buffer				buffer[PORTS];
	bool				sendNull[PORTS];

	// Functions
	void				rxProcess();
	void				txProcess();

	// I/O Ports
	sc_in<Packet>			pack_in[PORTS];
	sc_out<Packet>			pack_out[PORTS];
	// These maps are used to store the number of the port on which a switch (with a unique ID) is connected (if the port is active)
	portToSwitchMap_t		portTo;
	portToSwitchMap_t		invPortTo;
	destActiveMap_t			destActive;
	// ...while these counters are used to keep trace of the next free slots (pack_in&out ports)
	unsigned int			connectedOutgoingEdges;
	unsigned int			connectedIncomingEdges;
	// List of signals used to complete the bindings for unused ports
	list< sc_signal<Packet>* >	dummy_signals;

	// Stats
	unsigned int			flitsIn[PORTS];
	unsigned int			flitsOut[PORTS];
	unsigned int			dropped[PORTS];

public:
	// I/O Ports
	sc_in_clk			clock;
	bool				bindIn(sc_signal<Packet>* sig);
	bool				bindOut(sc_signal<Packet>* sig, unsigned int dst_id);
	void				completeBindings();

	// Functions
					Switch(unsigned int id);
					~Switch();
	unsigned int			getId() {return local_id;}
	bool				addDestination(unsigned int dst_id, unsigned int nextHop_id);
	bool				modifyDestination(unsigned int dst_id, unsigned int nextHop_id);
	bool        isPortActive(unsigned int id);
	void				setBufferSize(unsigned int id, unsigned int size);
	unsigned int	getBufferSize(unsigned int id);
	unsigned int	getBufferCurrentFreeSlots(unsigned int id);
	unsigned int	getDropped(unsigned int id);
	unsigned int	getFlitsIn(unsigned int id);
	unsigned int	getFlitsOut(unsigned int id);
	void				printLRT();
	void				printStats();
};

#endif
