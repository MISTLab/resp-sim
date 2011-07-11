#include "masterPE.hpp"

masterPE::masterPE(unsigned int id, forwardMap_t *fM): sc_module(("me_" + boost::lexical_cast<std::string>(id)).c_str()), local_id(id),
		forwardMap(fM), targetSocket(("meTargetSock_" + boost::lexical_cast<std::string>(id)).c_str()) {
	last_session_id = (unsigned int)-1;
	flitsIn = 0;
	flitsOut = 0;
	packetsIn = 0;
	packetsOut = 0;
	timedOutSessions = 0;
	timeoutVal = sc_time(TIMEOUT,SC_NS);
	connectedIn = false;
	connectedOut = false;
	sendNull = true;
	this->targetSocket.register_b_transport(this, &masterPE::b_transport);
	SC_HAS_PROCESS(masterPE);
	SC_METHOD(txProcess);
		sensitive << clock.pos();
	SC_METHOD(rxProcess);
		sensitive << clock.pos();
	end_module();
}

masterPE::~masterPE() {
	map<unsigned int, Session*>::iterator openSessionsIt;
	for (openSessionsIt = openSessions.begin(); openSessionsIt != openSessions.end(); openSessionsIt++) {
		delete openSessionsIt->second;
	}
	map<unsigned int, sc_event*>::iterator endTransmIt;
	for (endTransmIt = endTransm.begin(); endTransmIt != endTransm.end(); endTransmIt++) {
		delete endTransmIt->second;
	}
	list< sc_signal<Packet>* >::iterator sigIter;
	for (sigIter = dummy_signals.begin(); sigIter != dummy_signals.end(); sigIter++) {
		delete *sigIter;
	}
}

void masterPE::b_transport(tlm_generic_payload& trans, sc_time& delay) {

	tlm_generic_payload* tlmPacket = &trans;

	#ifdef DEBUGMODE
	cerr	<< sc_time_stamp().to_double()/1000 \
		<< ": ME[" << local_id << "] \tRECEIVED TLM" << " \t\t" \
		<< tlmPacket->get_command() << " to " << tlmPacket->get_address() << "." << endl;
	#endif

	Packet p;
	do {
		p = genPacket(tlmPacket);
		
		#ifdef DEBUGMODE
		cerr	<< sc_time_stamp().to_double()/1000 \
			<< ": ME[" << local_id << "] \tGENERATED" << " \t\t" \
			<< p << "." << endl;
		#endif
		packet_queue.push(p);
		if (endTransm[p.session_id] != NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Session number " << p.session_id << " already in use" << endl);
		timedOut[p.session_id] = false;
		endTransm[p.session_id] = new sc_event();
		sc_spawn(sc_bind(&masterPE::timeout,this,sc_ref(p.session_id)));
		wait(*endTransm[p.session_id]);
	} while(timedOut[p.session_id]);
	delete endTransm[p.session_id];
	endTransm[p.session_id] = NULL;
	endTransm.erase(p.session_id);
	timedOut.erase(p.session_id);
	#ifdef DEBUGMODE
	cerr	<< sc_time_stamp().to_double()/1000 \
		<< ": ME[" << local_id << "] \tFINISHED TLM" << " \t\t" \
		<< tlmPacket->get_command() << " to " << tlmPacket->get_address() << "." << endl;
	#endif
	}

void masterPE::setTimeout(sc_time tO) {
	timeoutVal = tO;
}

void masterPE::timeout(unsigned int session_id) {
	wait(timeoutVal);
	if (endTransm.count(session_id)/*endTransm[session_id] != NULL*/) {
		timedOut[session_id] = true;
		timedOutSessions++;
		#ifdef DEBUGMODE
		cerr	<< sc_time_stamp().to_double()/1000 \
			<< ": ME[" << local_id << "] \tSESSION " \
			<< session_id << " TIMED OUT." << endl;
		#endif
		endTransm[session_id]->notify();
	}
}

void masterPE::txProcess() {
	bool newSendNull = false;
	if(!packet_queue.empty()){	// consumes the first packet in queue (more than one flit means more packets to send)
		Packet flit = nextFlit();
		// send the packet
		#ifdef DEBUGMODE
		cerr 	<< sc_time_stamp().to_double()/1000 \
			<< ": ME[" << local_id << "] \tSENDING" << " \t\t" \
			<< flit << "." << endl;
		#endif
		pack_out.write(flit);
		flitsOut++;
		newSendNull = true;
	}
	else if (sendNull) {		// writes a "NULL" packet to clear the signal of the old packets
		Packet empty;
		#if defined(DEBUGMODE) && defined(PRINTNULLGEN)
		cerr	<< sc_time_stamp().to_double()/1000 \
			<< ": ME[" << local_id << "] \tNULLGEN" << " \t\t" \
			<< empty << "." << endl;
		#endif
		pack_out.write(empty);
	}
	sendNull = newSendNull;
}

void masterPE::rxProcess() {
	Packet p = pack_in.read();
	if(p != -1){					// null packet
		switch(p.type) {
		case ACK:				// ACK management
			#ifdef DEBUGMODE
			cerr 	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tRECEIVED ACK" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (endTransm[p.session_id] == NULL)
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Received ACK for unknown session" << endl);
			endTransm[p.session_id]->notify();
			flitsIn++;
			packetsIn++;
			break;
		case HEAD:				// HEAD management
			#ifdef DEBUGMODE
			cerr 	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tRECEIVED HEAD" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (openSessions.count(p.session_id) == 0/*[p.session_id] == NULL*/) openSessions[p.session_id] = new Session(p.size);
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete() && !timedOut[p.session_id]) {
				// send ACK to source PE and awake TLM transport
				packetsIn++;
				Packet a = genAck(p);
				packet_queue.push(a);
				#ifdef DEBUGMODE
				cerr 	<< sc_time_stamp().to_double()/1000 \
					<< ": ME[" << local_id << "] \tENQUEUED ACK" << " \t\t" \
					<< a << "." << endl;
				cerr	<< "\t\tCycles: " << (sc_time_stamp().to_double()/1000 - p.gen_time) \
					<< "." << endl;
				#endif
				if (endTransm[p.session_id] == NULL)
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Received ACK for unknown session" << endl);
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL;
				openSessions.erase(p.session_id);
				endTransm[p.session_id]->notify();
			}
			break;
		case BODY:				// BODY management
			#ifdef DEBUGMODE
			cerr 	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tRECEIVED BODY" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (openSessions.count(p.session_id) == 0/*[p.session_id] == NULL*/) openSessions[p.session_id] = new Session(p.size);
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete() && !timedOut[p.session_id]) {
				// send ACK to source PE and awake TLM transport
				packetsIn++;
				Packet a = genAck(p);
				packet_queue.push(a);
				#ifdef DEBUGMODE
				cerr 	<< sc_time_stamp().to_double()/1000 \
					<< ": ME[" << local_id << "] \tENQUEUED ACK" << " \t\t" \
					<< a << "." << endl;
				cerr	<< "\t\tCycles: " << (sc_time_stamp().to_double()/1000 - p.gen_time) \
					<< "." << endl;
				#endif
				if (endTransm[p.session_id] == NULL)
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Received ACK for unknown session" << endl);
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL;
				openSessions.erase(p.session_id);
				endTransm[p.session_id]->notify();
			}
			break;
		case TAIL:				// TAIL management
			#ifdef DEBUGMODE
			cerr 	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tRECEIVED TAIL" << " \t\t" \
				<< p << "." << endl;
			#endif
			if (openSessions.count(p.session_id) == 0/*[p.session_id] == NULL*/) openSessions[p.session_id] = new Session(p.size);
			openSessions[p.session_id]->newPacket(p.flit_left);
			flitsIn++;
			if (openSessions[p.session_id]->isComplete() && !timedOut[p.session_id]) {
				// send ACK to source PE and awake TLM transport
				packetsIn++;
				Packet a = genAck(p);
				packet_queue.push(a);
				#ifdef DEBUGMODE
				cerr 	<< sc_time_stamp().to_double()/1000 \
					<< ": ME[" << local_id << "] \tENQUEUED ACK" << " \t\t" \
					<< a << "." << endl;
				cerr	<< "\t\tCycles: " << (sc_time_stamp().to_double()/1000 - p.gen_time) \
					<< "." << endl;
				#endif
				if (endTransm[p.session_id] == NULL)
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Received ACK for unknown session" << endl);
				delete openSessions[p.session_id];
				openSessions[p.session_id] = NULL;
				openSessions.erase(p.session_id);
				endTransm[p.session_id]->notify();
			}
			break;
		default:				// Other packets management such as DATA_PACKET
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tRECEIVING" << " \t\t" \
				<< p << "." << endl;
			#endif

			// send ACK to source PE and awake TLM transport
			flitsIn++;
			packetsIn++;
			Packet a = genAck(p);
			packet_queue.push(a);
			#ifdef DEBUGMODE
			cerr	<< sc_time_stamp().to_double()/1000 \
				<< ": ME[" << local_id << "] \tENQUEUED ACK" << " \t\t" \
				<< a << "." << endl;
			cerr	<< "\t\tCycles: " << (sc_time_stamp().to_double()/1000 - p.gen_time) \
				<< "." << endl;
			#endif
			if (endTransm[p.session_id] == NULL)
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Received ACK for unknown session" << endl);
			endTransm[p.session_id]->notify();
		}
	}
}

Packet masterPE::genPacket(tlm_generic_payload* tlmPacket){
	tlm_command cmd = tlmPacket->get_command();
	sc_dt::uint64 adr = tlmPacket->get_address();
	unsigned int len = tlmPacket->get_data_length();

	unsigned int numFlits;
	if (cmd == TLM_READ_COMMAND) numFlits = 1;
	else if (cmd == TLM_WRITE_COMMAND) {
		numFlits = len / BYTESPERPACKET;
		if (len%BYTESPERPACKET != 0) numFlits++;
	}
	else THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Unknown TLM command" << endl);

	unsigned int destId;
	bool found = false;
	forwardMap_t::iterator fwIter;
	if (forwardMap == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Shared forwarding map not found" << endl);
	for (fwIter = forwardMap->begin(); !found && fwIter != forwardMap->end(); fwIter++) {
		if ( fwIter->first.first <= adr && adr <= fwIter->first.second ) {
			destId = fwIter->second;
			found = true;
		}
	}
	if (!found) THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Address " << adr << " not bound to a slave device" << endl);

	Packet p;
	if(numFlits != 1){
		p.size = p.flit_left = numFlits + 2;		// N flits plus HEAD and TAIL
		p.type = NO_TYPE;
	} else {
		p.size = p.flit_left = 1;	// number of flits, so in this case only this packet
		p.type = DATA_PACKET;		// data packet
	}

	p.session_id = (++last_session_id + SESSION_ID_OFFSET*local_id); //TODO does it works when session_id overcomes (SESSION_ID_OFFSET*(local_id+1)). maybe yes since we remove closed sessions
	p.gen_time = sc_time_stamp().to_double();
	p.src_addr = local_id;
	p.dst_addr = destId;
	p.payload = tlmPacket;

	packetsOut++;

	//	p.next_hop.push_back(2); // sw0 -> sw2
	// 	p.next_hop.push_back(1); // sw2 -> sw3
	// 	p.next_hop.push_back(3); // sw3 -> sw5
	// 	p.next_hop.push_back(0); // sw5 -> s2

	return p;
}

Packet masterPE::genAck(Packet &p_in){
	Packet p;
	p.size = p.flit_left = 1;	// number of flits, so in this case only this packet
	p.type = ACK;			// ack packet

	p.session_id = p_in.session_id;
	p.src_addr = local_id;
	p.dst_addr = p_in.src_addr;
	p.payload = NULL;
	p.gen_time = sc_time_stamp().to_double();

	packetsOut++;

	//	p.next_hop.push_back(1); // sw5 -> sw4
	//	p.next_hop.push_back(2); // sw4 -> sw2
	//	p.next_hop.push_back(2); // sw2 -> sw0
	//	p.next_hop.push_back(0); // sw0 -> m0

	return p;
}

Packet masterPE::nextFlit(){
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

bool masterPE::bindIn(sc_signal<Packet>* sig) {
	if (connectedIn) return false;
	pack_in(*sig);
	connectedIn = true;
	return true;
}

bool masterPE::bindOut(sc_signal<Packet>* sig) {
	if (connectedOut) return false;
	pack_out(*sig);
	connectedOut = true;
	return true;
}

void masterPE::completeBindings() {
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

void masterPE::printStats(){
	cout << "Master Processing Element #" << local_id << endl;
	cout << "\tRECEIVED\t" << flitsIn << " flits for a total of " << packetsIn << " packets" << endl;
	cout << "\tSENT\t\t" << flitsOut << " flits for a total of " << packetsOut << " packets" << endl;
	cout << "\tTIMED OUT\t" << timedOutSessions << " times" << endl;
}

