#include "slavePE.hpp"

slavePE::slavePE(unsigned int id): sc_module(("se_" + boost::lexical_cast<std::string>(id)).c_str()),
		local_id(id), initSocket(("seInitSock_" + boost::lexical_cast<std::string>(id)).c_str()) {
	busyTLM = false;
	curTLMEvent = 0;
	flitsIn = 0;
	flitsOut = 0;
	packetsIn = 0;
	packetsOut = 0;
	connectedIn = false;
	connectedOut = false;
	sendNull = true;
	SC_HAS_PROCESS(slavePE);
	SC_METHOD(rxProcess);
		sensitive << clock.pos();
	SC_METHOD(txProcess);
		sensitive << clock.pos();
	end_module();
}

slavePE::~slavePE() {
	map<unsigned int, Session*>::iterator openSessionsIt;
	for (openSessionsIt = openSessions.begin(); openSessionsIt != openSessions.end(); openSessionsIt++) {
		delete openSessionsIt->second;
	}
	list< sc_signal<Packet>* >::iterator sigIter;
	for (sigIter = dummy_signals.begin(); sigIter != dummy_signals.end(); sigIter++) {
		delete *sigIter;
	}
}

void slavePE::txProcess() {
	bool newSendNull = false;
	if(!packet_queue.empty()){	// consumes the first packet in queue (more than one flit means more packets to send)
		Packet flit = nextFlit();
		// send the packet
		#ifdef DEBUGMODE
		cerr	<< sc_time_stamp().to_double()/1000 \
			<< ": SE[" << local_id << "] \tSENDING" << " \t\t" \
			<< flit << "." << endl;
		#endif
		pack_out.write(flit);
		flitsOut++;
		flitsOutPerDest[flit.dst_addr]++;
		newSendNull = true;
	}
	else if (sendNull) {		// writes a "NULL" packet to clear the signal of the old packets
		Packet empty;
		#if defined(DEBUGMODE) && defined(PRINTNULLGEN)
		cerr	<< sc_time_stamp().to_double()/1000 \
			<< ": SE[" << local_id << "] \tNULLGEN" << " \t\t" \
			<< empty << "." << endl;
		#endif
		pack_out.write(empty);
	}
	sendNull = newSendNull;
}

void slavePE::rxProcess() { //TODO in case of retransmission, the request is reissued to the slave. It should not be reissued, only the aswer should be sent back. to do this, a completedSessions map should be used
	Packet p = pack_in.read();
	if(p != -1){					// null packet
		switch(p.type) {
		case ACK:				// ACK management
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": SE[" << local_id << "] \tRECEIVED ACK" << " \t\t" \
				<< p << "." << endl;
			#endif
			flitsIn++;
			packetsIn++;
			break;
		case HEAD:				// HEAD management
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": SE[" << local_id << "] \tRECEIVED HEAD" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (openSessions[p.session_id] == NULL) openSessions[p.session_id] = new Session(p.size); //TODO: accoding to the below TODO, this test should be changed in openSessions.count(p.session_id)==0
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete()) {
				// do WRITE transaction
				packetsIn++;
				Packet *persistant = new Packet(p);
				sc_spawn(sc_bind(&slavePE::launchTLMWrite,this,sc_ref(*persistant)));
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL; //TODO: when a session is closed, it should be removed from openSessions
			}
			break;
		case BODY:				// BODY management
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": SE[" << local_id << "] \tRECEIVED BODY" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (openSessions[p.session_id] == NULL) openSessions[p.session_id] = new Session(p.size); //TODO: accoding to the below TODO, this test should be changed in openSessions.count(p.session_id)==0
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete()) {
				// do WRITE transaction
				packetsIn++;
				Packet *persistant = new Packet(p);
				sc_spawn(sc_bind(&slavePE::launchTLMWrite,this,sc_ref(*persistant)));
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL; //TODO: when a session is closed, it should be removed from openSessions
			}
			break;
		case TAIL:				// TAIL management
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": SE[" << local_id << "] \tRECEIVED TAIL" << " \t\t" \
				<< p << "." << endl;
			#endif
			p.session_id = p.session_id;
			if (openSessions[p.session_id] == NULL) openSessions[p.session_id] = new Session(p.size); //TODO: accoding to the below TODO, this test should be changed in openSessions.count(p.session_id)==0
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete()) {
				// do WRITE transaction
				packetsIn++;
				Packet *persistant = new Packet(p);
				sc_spawn(sc_bind(&slavePE::launchTLMWrite,this,sc_ref(*persistant)));
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL; //TODO: when a session is closed, it should be removed from openSessions
			}
			break;
		default:				// Other packets management such as DATA_PACKET
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": SE[" << local_id << "] \tRECEIVING" << " \t\t" \
				<< p << "." << endl;
			#endif

			flitsIn++;
			packetsIn++;
			p.session_id = p.session_id;
			if ((p.payload)->get_command() == TLM_WRITE_COMMAND) {
				// do WRITE transaction
				Packet *persistant = new Packet(p);
				sc_spawn(sc_bind(&slavePE::launchTLMWrite,this,sc_ref(*persistant)));
			}
			else if ((p.payload)->get_command() == TLM_READ_COMMAND) {
				// do READ transaction
				Packet *persistant = new Packet(p);
				sc_spawn(sc_bind(&slavePE::launchTLMRead,this,sc_ref(*persistant)));
			}
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Unknown TLM command" << endl);
		}
	}
}

void slavePE::launchTLMRead(Packet &p) {
	lockTLM();
	sc_time delay = SC_ZERO_TIME;
	this->initSocket->b_transport(*(p.payload),delay);
	unlockTLM();

	// send all packets to source PE
	Packet a = genResponsePacket(p);
	packet_queue.push(a);
	
	#ifdef DEBUGMODE
	cerr 	<< sc_time_stamp().to_double()/1000 \
		<< ": SE[" << local_id << "] \tGENERATED" << " \t\t" \
		<< a << "." << endl;
	#endif
	delete &p;
}

void slavePE::launchTLMWrite(Packet &p) {
	lockTLM();
	sc_time delay = SC_ZERO_TIME;
	this->initSocket->b_transport(*(p.payload),delay);
	unlockTLM();

	// send ack to source PE
	Packet a = genAck(p);
	packet_queue.push(a);
	
	#ifdef DEBUGMODE
	cerr 	<< sc_time_stamp().to_double()/1000 \
		<< ": SE[" << local_id << "] \tENQUEUED ACK" << " \t\t" \
		<< a << "." << endl;
	cerr	<< "\t\tCycles: " << (sc_time_stamp().to_double()/1000 - p.gen_time) \
		<< "." << endl;
	#endif
	delete &p;
}

Packet slavePE::genResponsePacket(Packet &p_in) { 
  //DO NOTE: this function is used only to generate the response packet for read requests
	unsigned int adr = p_in.src_addr;
	unsigned int len = (p_in.payload)->get_data_length();

	unsigned int numFlits;
	numFlits = len / BYTESPERPACKET;
	if (len%BYTESPERPACKET != 0) numFlits++;

	unsigned int destId = adr;

	Packet p;
	if(numFlits != 1){
		p.size = p.flit_left = numFlits + 2;		// N flits plus HEAD and TAIL
		p.type = NO_TYPE;
	} else {
		p.size = p.flit_left = 1;	// number of flits, so in this case only this packet
		p.type = DATA_PACKET;		// data packet
	}

	p.session_id = (p_in.session_id);
	p.gen_time = sc_time_stamp().to_double();
	p.src_addr = local_id;
	p.dst_addr = destId;
	p.payload = NULL;

	packetsOut++;
	packetsOutPerDest[p.dst_addr]++;

	return p;
}

Packet slavePE::genAck(Packet &p_in){
	Packet p;
	p.size = p.flit_left = 1;	// number of flits, so in this case only this packet
	p.type = ACK;			// ack packet

	p.session_id = p_in.session_id;
	p.src_addr = local_id;
	p.dst_addr = p_in.src_addr;
	p.payload = NULL;
	p.gen_time = sc_time_stamp().to_double();

	packetsOut++;
	packetsOutPerDest[p.dst_addr]++;

	//	p.next_hop.push_back(1); // sw5 -> sw4
	//	p.next_hop.push_back(2); // sw4 -> sw2
	//	p.next_hop.push_back(2); // sw2 -> sw0
	//	p.next_hop.push_back(0); // sw0 -> m0

	return p;
}

Packet slavePE::nextFlit(){
	Packet flit;
	flit.flit_left = packet_queue.front().flit_left;
	flit.size = packet_queue.front().size;

	flit.session_id = packet_queue.front().session_id;
	flit.src_addr = packet_queue.front().src_addr;
	flit.dst_addr = packet_queue.front().dst_addr;

	if(packet_queue.front().size == packet_queue.front().flit_left && packet_queue.front().size != 1)
		flit.type = HEAD;
	else if(packet_queue.front().flit_left == 1 && packet_queue.front().size != 1)
		flit.type = TAIL;
	else if(packet_queue.front().size != 1)
		flit.type = BODY;
	else if(packet_queue.front().type == ACK)
		flit.type = ACK;
	else if(packet_queue.front().type == NACK)
		flit.type = NACK;
	else if(packet_queue.front().size == 1)
		flit.type = DATA_PACKET;

	// Copy packet routing to flits
	list<unsigned int>::iterator it;
	for(it = packet_queue.front().next_hop.begin(); it != packet_queue.front().next_hop.end(); it++){
		flit.next_hop.push_back(*it);
	}
	flit.payload = packet_queue.front().payload;
	flit.gen_time = packet_queue.front().gen_time;

	packet_queue.front().flit_left--;
	if(packet_queue.front().flit_left == 0)
		packet_queue.pop();

	return flit;
}

void slavePE::lockTLM() {
	if( this->busyTLM ) {
		unsigned int myEvent = curTLMEvent++;
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Queuing TLM request on " << myEvent << endl;
		#endif
		nextTLMWake.push(myEvent);
		wakeTLMEvents[myEvent] = new sc_event;
		wait(*(this->wakeTLMEvents[myEvent]));
		delete wakeTLMEvents[myEvent];
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Starting TLM request " << myEvent << endl;
		#endif
	}
	this->busyTLM = true;
}

void slavePE::unlockTLM() {
	this->busyTLM = false;
	if( !nextTLMWake.empty() ) {
		unsigned int nextEvent = this->nextTLMWake.front();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Waking up queued TLM request " << nextEvent << endl;
		#endif
		this->nextTLMWake.pop();
		(this->wakeTLMEvents[nextEvent])->notify();
		this->busyTLM = true;
	}
}

bool slavePE::bindIn(sc_signal<Packet>* sig){
	if (connectedIn) return false;
	pack_in(*sig);
	connectedIn = true;
	return true;
}

bool slavePE::bindOut(sc_signal<Packet>* sig){
	if (connectedOut) return false;
	pack_out(*sig);
	connectedOut = true;
	return true;
}

void slavePE::completeBindings() {
	sc_signal<Packet>* tmpSignal;
	if (!connectedIn) {
		tmpSignal = new sc_signal<Packet>();
		pack_in(*tmpSignal);
		dummy_signals.push_back(tmpSignal);
		connectedIn = true;
	}
	if (!connectedOut) {
		tmpSignal = new sc_signal<Packet>();
		pack_out(*tmpSignal);
		dummy_signals.push_back(tmpSignal);
		connectedOut = true;
	}
}

void slavePE::printStats() {
	cout << "Slave Processing Element #" << local_id << endl;
	cout << "\tRECEIVED\t" << flitsIn << " flits for a total of " << packetsIn << " packets" << endl;
	cout << "\tSENT\t\t" << flitsOut << " flits for a total of " << packetsOut << " packets" << endl;
	unsigned int f_num = 0, f_den = 0, p_num = 0, p_den = 0;
	for(std::map<unsigned int,unsigned int>::iterator mapIt = mastersDist.begin(); mapIt != mastersDist.end(); mapIt++){
	  f_num += (flitsOutPerDest[mapIt->first]*mapIt->second);
	  f_den += flitsOutPerDest[mapIt->first];
	  p_num += (packetsOutPerDest[mapIt->first]*mapIt->second);
	  p_den += packetsOutPerDest[mapIt->first];
	}
	cout << "\tAVERAGE HOPS per packet \t" << (p_den!=0?(double)p_num/p_den:0) << " per packet, " << (f_den!=0?(double)f_num/f_den:0) << " per flit " << endl;

}

