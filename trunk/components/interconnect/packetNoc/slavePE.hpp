#ifndef __SLAVE_PROCESSING_ELEMENT_H__
#define __SLAVE_PROCESSING_ELEMENT_H__

#include "nocCommon.hpp"

class slavePE: public sc_module {
private:
	// Variables
	unsigned int			local_id;
	queue<Packet>			packet_queue;
	map<unsigned int, Session*>	openSessions;
	bool				sendNull;

	// Functions
	void				rxProcess();
	void				txProcess();
	void				launchTLMRead(Packet &p);
	void				launchTLMWrite(Packet &p);
	Packet				genResponsePacket(Packet &p_in);
	Packet				genAck(Packet &p_in);
	Packet				nextFlit();

	// Dealing with multiple parallel TLM accesses
	unsigned int curTLMEvent;
	map<unsigned int,sc_event *> wakeTLMEvents;
	queue<unsigned int> nextTLMWake;
	bool busyTLM;
	void lockTLM();
	void unlockTLM();

	// I/O Ports
	sc_in<Packet>			pack_in;
	sc_out<Packet>			pack_out;
	// Used to check if the element is already connected
	bool				connectedIn;
	bool				connectedOut;
	// List of signals used to complete the bindings for unused ports
	list< sc_signal<Packet>* >	dummy_signals;

public:
	// I/O Ports
	sc_in_clk			clock;
	bool				bindIn(sc_signal<Packet>* sig);
	bool				bindOut(sc_signal<Packet>* sig);
	void				completeBindings();

	simple_initiator_socket<slavePE, 32> initSocket;

	// Stats
	unsigned int			flitsIn;
	unsigned int			flitsOut;
	unsigned int			packetsIn;
	unsigned int			packetsOut;

	// Functions
					slavePE(unsigned int id);
					~slavePE();
	unsigned int			getId() {return local_id;}
	void				printStats();
};

#endif
