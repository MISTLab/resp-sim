#include "Switch.hpp"

Switch::Switch(unsigned int id): sc_module(("sw_" + boost::lexical_cast<std::string>(id)).c_str()), local_id(id) {
	flitsIn = 0;
	flitsOut = 0;
	dropped = 0;
	connectedOutgoingEdges = 0;
	connectedIncomingEdges = 0;
	for(unsigned int i = 0; i < PORTS; i++){sendNull[i]=true;}
	SC_HAS_PROCESS(Switch);
	SC_METHOD(rxProcess)
		sensitive << clock.pos();
	SC_METHOD(txProcess)
		sensitive << clock.pos();
	end_module();
}

Switch::~Switch() {
	list< sc_signal<Packet>* >::iterator sigIter;
	for (sigIter = dummy_signals.begin(); sigIter != dummy_signals.end(); sigIter++) {
		delete *sigIter;
	}
}

void Switch::rxProcess(){
	for(unsigned int i = 0; i < PORTS; i++){
		Packet p = pack_in[i].read();
		if(p != -1){
			#ifdef DEBUGMODE
			cerr << sc_time_stamp().to_double()/1000 \
				<< ": SW[" << local_id << "] \t" \
				<< "RECEIVING on [" << i << "]" << " \t" \
				<< p << "." << endl;
			#endif
			flitsIn++;
			if (!buffer[i].push(p)) {
				#ifdef DEBUGMODE
				cerr 	<< sc_time_stamp().to_double()/1000 \
					<< ": SW[" << local_id << "] \tDROPPING" << " \t\t" \
					<< p << "." << endl;
				#endif
				dropped++;
			}
		}
	}
}

void Switch::txProcess(){
	bool written[PORTS];
	for(unsigned int i = 0; i < PORTS; i++){written[i]=false;}

	unsigned int out_port;
	for(unsigned int i = 0; i < PORTS; i++){
		if(!buffer[i].isEmpty()){
			out_port = (unsigned int)-1;
			Packet p = buffer[i].front();
			if(p.force_route && !p.update_route && !p.dyn_slave_deletion){
				// manage FORCE ROUTE
				out_port = p.get_next_hop();
				#ifdef DEBUGMODE
				cerr << "\t\tDEBUG, force route" << endl;
				#endif
			} else if(!p.force_route && p.update_route && !p.dyn_slave_deletion){
				// manage UPDATE ROUTE
				// TBD
				#ifdef DEBUGMODE
				cerr << "\t\tDEBUG, update route" << endl;
				#endif
			} else if(!p.force_route && !p.update_route && p.dyn_slave_deletion){
				// manage DYNAMIC SLAVE DELETION
				// TBD
				#ifdef DEBUGMODE
				cerr << "\t\tDEBUG, dyn slave deletion" << endl;
				#endif
			} else {
				// DEFAULT operation: read destination, look up outport from Local Routing Table, forward
				if (destActive[p.dst_addr]) out_port = portTo[p.dst_addr];
				#ifdef DEBUGMODE
				cerr	<< "\t\tDEBUG, default: dst_addr = " << p.dst_addr << ", out_port = " \
					<< out_port << ", packet = " << p.src_addr << "-->" << p.dst_addr << endl;
				#endif
			}

			if(out_port == (unsigned int)-1)
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error in SW[" << local_id \
					<< "]: out_port not initialized for packet with DST address " \
					<< p.dst_addr << " and SRC address " << p.src_addr << endl);
			if (!written[out_port]) {
				#ifdef DEBUGMODE
				cerr << sc_time_stamp().to_double()/1000 \
					<< ": SW[" << local_id << "] \t" \
					<< "FORWARDING on [" << out_port << "]" << " \t" \
					<< p << "." << endl;
				#endif
				flitsOut++;
				pack_out[out_port].write(p);
				written[out_port] = true;
				buffer[i].pop();
			}
		}
	}
	for(unsigned int i = 0; i < PORTS; i++){
		// clear the port: pack_out so that the other switches do not read old packets
		if (sendNull[i] && !written[i]) {
			Packet empty;
			#if defined(DEBUGMODE) && defined(PRINTNULLGEN)
			cerr << sc_time_stamp().to_double()/1000 \
				<< ": SW[" << local_id << "] \t" \
				<< "NULLGEN on [" << i << "]" << " \t\t" \
				<< empty << "." << endl;
			#endif
			pack_out[i].write(empty);
		}
	}
	for(unsigned int i = 0; i < PORTS; i++){sendNull[i]=written[i];}
}

bool Switch::bindIn(sc_signal<Packet>* sig){
	if (connectedIncomingEdges >= PORTS) return false;
	pack_in[connectedIncomingEdges](*sig);
	connectedIncomingEdges++;
	return true;
}

bool Switch::bindOut(sc_signal<Packet>* sig, unsigned int dst_id){
	if (connectedOutgoingEdges >= PORTS) return false;
	pack_out[connectedOutgoingEdges](*sig);
	portTo[dst_id] = connectedOutgoingEdges;
	destActive[dst_id] = true;
	connectedOutgoingEdges++;
	return true;
}

void Switch::completeBindings() {
	sc_signal<Packet>* tmpSignal;
	while (connectedIncomingEdges < PORTS) {
		tmpSignal = new sc_signal<Packet>();
		pack_in[connectedIncomingEdges](*tmpSignal);
		dummy_signals.push_back(tmpSignal);
		connectedIncomingEdges++;
	}
	while (connectedOutgoingEdges < PORTS) {
		tmpSignal = new sc_signal<Packet>();
		pack_out[connectedOutgoingEdges](*tmpSignal);
		dummy_signals.push_back(tmpSignal);
		connectedOutgoingEdges++;
	}
}

bool Switch::addDestination(unsigned int dst_id, unsigned int nextHop_id) {
	if (destActive[dst_id] || !destActive[nextHop_id]) return false;
	destActive[dst_id] = true;
	portTo[dst_id] = portTo[nextHop_id];
	return true;
}

bool Switch::modifyDestination(unsigned int dst_id, unsigned int nextHop_id) {
	if (!destActive[dst_id] || !destActive[nextHop_id]) return false;
	portTo[dst_id] = portTo[nextHop_id];
	return true;
}

void Switch::setBufferSize(unsigned int size) {
	for (unsigned int i = 0; i < PORTS; i++) buffer[i].setMaxBufferSize(size);
}

void Switch::printLRT() {
	destActiveMap_t::iterator destIter;
	cout << "Switch #" << local_id << endl;
	for (destIter = destActive.begin(); destIter != destActive.end(); destIter++) {
		if (destIter->second) cout << "\tRequests with destination " << destIter->first << " are forwarded on " << portTo[destIter->first] << endl;
	}
}

void Switch::printStats() {
	cout << "Switch #" << local_id << endl;
	cout << "\tRECEIVED\t" << flitsIn << " flits" << endl;
	cout << "\tSENT\t\t" << flitsOut << " flits" << endl;
	cout << "\tDROPPED\t\t" << dropped << " flits" << endl;
}

