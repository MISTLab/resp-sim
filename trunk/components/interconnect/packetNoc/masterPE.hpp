#ifndef __MASTER_PROCESSING_ELEMENT_H__
#define __MASTER_PROCESSING_ELEMENT_H__

#include "nocCommon.hpp"

typedef pair<sc_dt::uint64,sc_dt::uint64> addressSet;
typedef map<addressSet, unsigned int> forwardMap_t;

class masterPE: public sc_module {
private:
	// Variables
	unsigned int			local_id;
	unsigned int			last_session_id;
	queue<Packet>			packet_queue;
	map<unsigned int, Session*>	openSessions;
	map<unsigned int, sc_event*>	endTransm;
	map<unsigned int, bool>		timedOut;
	bool				sendNull;
	sc_time				timeoutVal;

	// Functions
	void				rxProcess();
	void				txProcess();
	void				timeout(unsigned int session_id);
	Packet				genPacket(tlm_generic_payload* tlmPacket);
	Packet				genAck(Packet &p_in);
//	Packet				genNack(unsigned int dst_addr);
	Packet				nextFlit();

	// I/O Ports
	sc_in<Packet>			pack_in;
	sc_out<Packet>			pack_out;
	// Used to check if the element is already connected
	bool				connectedIn;
	bool				connectedOut;
	// List of signals used to complete the bindings for unused ports
	list< sc_signal<Packet>* >	dummy_signals;
	// Pointer to a shared map assigning a destination slave element given a TLM address
	forwardMap_t			*forwardMap;

public:
	// I/O Ports
	sc_in_clk			clock;
	bool				bindIn(sc_signal<Packet>* sig);
	bool				bindOut(sc_signal<Packet>* sig);
	void				completeBindings();

	simple_target_socket<masterPE, 32> targetSocket;

	// Stats
	unsigned int			flitsIn;
	unsigned int			flitsOut;
	unsigned int			packetsIn;
	unsigned int			packetsOut;
	unsigned int			timedOutSessions;

	// Functions
					masterPE(unsigned int id, forwardMap_t *fM);
					~masterPE();
	void				b_transport(tlm_generic_payload& trans, sc_time& delay);
	unsigned int			getId() {return local_id;}
	void				setTimeout(sc_time tO);
	void				printStats();
};

#endif
