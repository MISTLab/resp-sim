#include "packetNoc.hpp"

packetNoc::packetNoc(sc_module_name module_name, string fileName, sc_time cycleLatency): sc_module(module_name), clock("clock", cycleLatency),
		intInitSocket((boost::lexical_cast<std::string>(module_name) + "_IntInitSock").c_str()),
		intTargetSocket((boost::lexical_cast<std::string>(module_name) + "_IntTargSock").c_str()),
		targetSocket((boost::lexical_cast<std::string>(module_name) + "_ExtTargSock").c_str()),
		initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_ExtInitSock").c_str()) {

	this->targetSocket.register_b_transport(this, &packetNoc::b_t1);
	this->intTargetSocket.register_b_transport(this, &packetNoc::b_t2);
	this->targetSocket.register_get_direct_mem_ptr(this, &packetNoc::get_direct_mem_ptr);
	this->intTargetSocket.register_get_direct_mem_ptr(this, &packetNoc::get_direct_mem_ptr);
	this->targetSocket.register_transport_dbg(this, &packetNoc::t_dbg1);
	this->intTargetSocket.register_transport_dbg(this, &packetNoc::t_dbg2);
	numAccesses = 0;
	numWords = 0;
	end_module();

	TiXmlDocument doc(fileName.c_str());
	if (!doc.LoadFile()) THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Error opening " << fileName << "." << endl);

	TiXmlHandle docH(&doc);
	TiXmlHandle rootH(0);

	// root
	rootH =	docH.FirstChildElement();
	if ( strcmp(rootH.Element()->Value(),"noc")!=0 )
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << "Root element of " << fileName << "not recognized as 'noc'." << endl);

	this->loadElementsFromXml(rootH);
	this->loadBindingsFromXml(rootH);
	this->completeSCBindings();

	#ifdef DEBUGMODE
	this->printGRT();
	#endif

	this->makeLRTs();

	#ifdef DEBUGMODE
	this->printGRT();
	#endif
}

packetNoc::~packetNoc() {}

void packetNoc::b_t1(int tag, tlm_generic_payload& trans, sc_time& delay){
	#ifdef DEBUGMODE
	cerr << "Request entering NOC" << endl;
	#endif

	unsigned int len = trans.get_data_length();
	unsigned int words = len / sizeof(unsigned int);
	if (len%sizeof(unsigned int) != 0) words++;
	this->numAccesses++;
	this->numWords+=words;
	
	intInitSocket[tag]->b_transport(trans, delay);
	trans.set_dmi_allowed(false);			// Disables DMI in order to insert the NOC latency for each transaction
	#ifdef DEBUGMODE
	cerr << "Request exiting NOC" << endl;
	#endif
}

void packetNoc::b_t2(int tag, tlm_generic_payload& trans, sc_time& delay){
	sc_dt::uint64 addr = trans.get_address();
	sc_dt::uint64 addr_old = trans.get_address();
	forwardMap_t::iterator fwIter;
	for (fwIter = forwardMap.begin(); fwIter != forwardMap.end(); fwIter++) {
		if ( fwIter->first.first <= addr && addr <= fwIter->first.second ) {
			addr = addr - fwIter->first.first;
			break;
		}
	}
	trans.set_address(addr);
	initiatorSocket[tag]->b_transport(trans, delay);
	trans.set_address(addr_old); //DO NOTE: it is necessary to restore original address otherwise in case of timeout the retransmission of the request will contain a wrong address
}

bool packetNoc::get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data){
	cout << "DMI not supported for NOC transactions! DMI requested by " << tag << endl;
	return false;
}

unsigned int packetNoc::t_dbg1(int tag, tlm::tlm_generic_payload& trans) {
	#ifdef DEBUGMODE
	cerr << "DBG Request entering NOC" << endl;
	#endif

	sc_dt::uint64 adr = trans.get_address();
	unsigned int destId;
	bool found = false;
	forwardMap_t::iterator fwIter;
	for (fwIter = forwardMap.begin(); !found && fwIter != forwardMap.end(); fwIter++) {
		if ( fwIter->first.first <= adr && adr <= fwIter->first.second ) {
			destId = fwIter->second;
			found = true;
		}
	}
	if (!found) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Address " << adr << " not bound to a slave device" << endl);

	slavePE* outSlave = getSlaveWithId(destId);
	if (outSlave == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Slave device with ID " << destId << " not found" << endl);
	unsigned int retVal = outSlave->initSocket->transport_dbg(trans);
	#ifdef DEBUGMODE
	cerr << "DBG Request exiting NOC" << endl;
	#endif
	return retVal;
}

unsigned int packetNoc::t_dbg2(int tag, tlm::tlm_generic_payload& trans) {
	sc_dt::uint64 addr = trans.get_address();
	forwardMap_t::iterator fwIter;
	for (fwIter = forwardMap.begin(); fwIter != forwardMap.end(); fwIter++) {
		if ( fwIter->first.first <= addr && addr <= fwIter->first.second ) {
			addr = addr - fwIter->first.first;
			break;
		}
	}
	trans.set_address(addr);
	unsigned int retVal = initiatorSocket[tag]->transport_dbg(trans);
	return retVal;
}

masterPE* packetNoc::getMasterWithId(unsigned int dst_id) {
	vector<masterPE*>::iterator masterIter;
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		if ((*masterIter)->getId() == dst_id) return *masterIter;
	}
	return NULL;
}

slavePE* packetNoc::getSlaveWithId(unsigned int dst_id) {
	vector<slavePE*>::iterator slaveIter;
	for (slaveIter = sl_list.begin(); slaveIter != sl_list.end(); slaveIter++) {
		if ((*slaveIter)->getId() == dst_id) return *slaveIter;
	}
	return NULL;
}

Switch* packetNoc::getSwitchWithId(unsigned int dst_id) {
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		if ((*switchIter)->getId() == dst_id) return *switchIter;
	}
	return NULL;
}

void packetNoc::loadElementsFromXml(TiXmlHandle rootH) {
	map<unsigned int, bool> existingElement;
	masterPE *tmpMs;
	slavePE *tmpSl;
	Switch* tmpSw;

	TiXmlElement* peEl;
	const char* idAttr;
	unsigned int id;
	// Get master processing elements information
	unsigned int numMasters = 0;
	for(peEl=rootH.FirstChild("mpe").Element(); peEl; peEl=peEl->NextSiblingElement("mpe")) {
		idAttr = peEl->Attribute("id");
		if (idAttr == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Required ID attribute missing for MPE element." << endl);
		id = (unsigned int) atoi(idAttr);
		if (existingElement[id])
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": ID " << id << " specified for an MPE has already been assigned to another element." << endl);
		tmpMs = new masterPE(id,&forwardMap);
		tmpMs->clock(this->clock);
		intInitSocket.bind(tmpMs->targetSocket);
		ms_list.push_back(tmpMs);
		existingElement[id] = true;
		numMasters++;
	}

	// Get slave processing elements information
	unsigned int numSlaves = 0;
	for(peEl=rootH.FirstChild("spe").Element(); peEl; peEl=peEl->NextSiblingElement("spe")) {
		idAttr = peEl->Attribute("id");
		if (idAttr == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Required ID attribute missing for SPE element." << endl);
		id = (unsigned int) atoi(idAttr);
		if (existingElement[id])
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": ID " << id << " specified for an SPE has already been assigned to another element." << endl);
		tmpSl = new slavePE(id);
		tmpSl->clock(this->clock);
		tmpSl->initSocket.bind(intTargetSocket);
		sl_list.push_back(tmpSl);
		existingElement[id] = true;
		numSlaves++;
	}

	// Get switches elements information
	unsigned int numSwitches = 0;
	for(peEl=rootH.FirstChild("switch").Element(); peEl; peEl=peEl->NextSiblingElement("switch")) {
		idAttr = peEl->Attribute("id");
		if (idAttr == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Required ID attribute missing for Switch element." << endl);
		id = (unsigned int) atoi(idAttr);
		if (existingElement[id])
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": ID " << id << " specified for an Switch has already been assigned to another element." << endl);
		tmpSw = new Switch(id);
		tmpSw->clock(this->clock);
		sw_list.push_back(tmpSw);
		existingElement[id] = true;
		numSwitches++;
	}

	// Assign number of elements in the NOC
	masters = numMasters;
	slaves = numSlaves;
	switches = numSwitches;
}

void packetNoc::loadBindingsFromXml(TiXmlHandle rootH) {
	TiXmlHandle elH(0);
	TiXmlElement* peEl;
	const char* idAttr;
	unsigned int id;
	TiXmlElement* entryEl;
	const char* dst_idAttr;
	unsigned int dst_id;

	unsigned int maxEdges = (switches*PORTS) + masters + slaves;
	unsigned int numEdges = 0;
	Edge_desc* edges = (Edge_desc*) malloc( maxEdges * sizeof(Edge_desc) );
	int* weights = (int*) malloc( maxEdges * sizeof(int) );

	for(peEl=rootH.FirstChild().Element(); peEl; peEl=peEl->NextSiblingElement()) {
		idAttr = peEl->Attribute("id");
		if (idAttr == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Required ID attribute missing for an element requiring connections." << endl);
		id = (unsigned int) atoi(idAttr);

		elH = TiXmlHandle(peEl);
		for(entryEl=elH.FirstChild("connect").Element(); entryEl; entryEl=entryEl->NextSiblingElement("connect")) {
			dst_idAttr = entryEl->Attribute("dst_id");
			if (dst_idAttr == NULL)
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Required ID attribute missing for a connection in " << id << "." << endl);
			dst_id = (unsigned int) atoi(dst_idAttr);
			edges[numEdges] = Edge_desc(id,dst_id);
			weights[numEdges] = 1;
			numEdges++;

			masterPE *initMaster, *targetMaster;
			slavePE *initSlave, *targetSlave;
			Switch *initSwitch, *targetSwitch;
			sc_signal<Packet>* tmpSignal = new sc_signal<Packet>();
			initMaster = getMasterWithId(id);
			if (initMaster != NULL) {
				targetMaster = getMasterWithId(dst_id);
				if (targetMaster != NULL) {
					if (!initMaster->bindOut(tmpSignal))
						THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
					if (!targetMaster->bindIn(tmpSignal))
						THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
				}
				else {
					targetSlave = getSlaveWithId(dst_id);
					if (targetSlave != NULL) {
						if (!initMaster->bindOut(tmpSignal))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						if (!targetSlave->bindIn(tmpSignal))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
					}
					else {
						targetSwitch = getSwitchWithId(dst_id);
						if (targetSwitch == NULL) 
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No target element with ID " \
								 << dst_id << " has been found." << endl);
						if (!initMaster->bindOut(tmpSignal))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						if (!targetSwitch->bindIn(tmpSignal))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
					}
				}
			}
			else {
				initSlave = getSlaveWithId(id);
				if (initSlave != NULL) {
					targetMaster = getMasterWithId(dst_id);
					if (targetMaster != NULL) {
						if (!initSlave->bindOut(tmpSignal)) 
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						if (!targetMaster->bindIn(tmpSignal)) 
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
					}
					else {
						targetSlave = getSlaveWithId(dst_id);
						if (targetSlave != NULL) {
							if (!initSlave->bindOut(tmpSignal))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
							if (!targetSlave->bindIn(tmpSignal)) 
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						}
						else {
							targetSwitch = getSwitchWithId(dst_id);
							if (targetSwitch == NULL) 
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No target element with ID " \
									<< dst_id << " has been found." << endl);
							if (!initSlave->bindOut(tmpSignal))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
							if (!targetSwitch->bindIn(tmpSignal))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						}
					}
				}
				else {
					initSwitch = getSwitchWithId(id);
					if (initSwitch == NULL) 
						THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No target element with ID " << \
							dst_id << " has been found." << endl);
					targetMaster = getMasterWithId(dst_id);
					if (targetMaster != NULL) {
						if (!initSwitch->bindOut(tmpSignal,dst_id))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						if (!targetMaster->bindIn(tmpSignal))
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
					}
					else {
						targetSlave = getSlaveWithId(dst_id);
						if (targetSlave != NULL) {
							if (!initSwitch->bindOut(tmpSignal,dst_id))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
							if (!targetSlave->bindIn(tmpSignal))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						}
						else {
							targetSwitch = getSwitchWithId(dst_id);
							if (targetSwitch == NULL)
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No target element with ID " \
									<< dst_id << " has been found." << endl);
							if (!initSwitch->bindOut(tmpSignal,dst_id))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
							if (!targetSwitch->bindIn(tmpSignal))
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Port binding error." << endl);
						}
					}
				}
			}
			signal_list.push_back(tmpSignal);
		}
	}

	networkGraph = Graph_t(edges, edges + numEdges, weights, masters+slaves+switches);
	graphWeightMap = get(edge_weight, networkGraph);
	free(edges);
	free(weights);
}

void packetNoc::completeSCBindings() {
	vector<masterPE*>::iterator masterIter;
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		(*masterIter)->completeBindings();
	}
	vector<slavePE*>::iterator slaveIter;
	for (slaveIter = sl_list.begin(); slaveIter != sl_list.end(); slaveIter++) {
		(*slaveIter)->completeBindings();
	}
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		(*switchIter)->completeBindings();
	}
}

void packetNoc::makeLRTs() {
	vector<masterPE*>::iterator masterIter;
	vector<slavePE*>::iterator slaveIter;
	Switch* switchPtr;
	unsigned int dst_id;
	vector<Vertex_t> p(num_vertices(networkGraph));
	vector<int> d(num_vertices(networkGraph));
	Vertex_it vi, vend;
	Vertex_t s;
	
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		dst_id = (*masterIter)->getId();
		s = vertex(dst_id, networkGraph);
		dijkstra_shortest_paths(networkGraph, s, predecessor_map(&p[0]).distance_map(&d[0]));

		#ifdef DEBUGMODE
		cerr << "Calculating routing maps for destination element #" << dst_id << endl;
		#endif
		// For each switch we have to update the internal routing map
		for (tie(vi, vend) = vertices(networkGraph); vi != vend; ++vi) {
			// Vertex 0 is not used in our graph...
			if (*vi == 0) continue;

			switchPtr = getSwitchWithId(*vi);
			if (switchPtr == NULL) continue;

			#ifdef DEBUGMODE
			cerr << "\tDistance(" << *vi << ") = " << d[*vi] << ", ";
			cerr << "\tParent(" << *vi << ") = " << p[*vi] << endl;
			#endif
			if (d[*vi]>1)
				if ( !switchPtr->addDestination(dst_id,p[*vi]) ) 
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Cannot add destination " << dst_id \
						<< " through " << p[*vi] << " to switch " << switchPtr->getId() << endl);
		}
	}
	for (slaveIter = sl_list.begin(); slaveIter != sl_list.end(); slaveIter++) {
		dst_id = (*slaveIter)->getId();
		s = vertex(dst_id, networkGraph);
		dijkstra_shortest_paths(networkGraph, s, predecessor_map(&p[0]).distance_map(&d[0]));

		#ifdef DEBUGMODE
		cerr << "Calculating routing maps for destination element #" << dst_id << endl;
		#endif
		// For each switch we have to update the internal routing map
		for (tie(vi, vend) = vertices(networkGraph); vi != vend; ++vi) {
			// Vertex 0 is not used in our graph...
			if (*vi == 0) continue;

			switchPtr = getSwitchWithId(*vi);
			if (switchPtr == NULL) continue;

			#ifdef DEBUGMODE
			cerr << "\tDistance(" << *vi << ") = " << d[*vi] << ", ";
			cerr << "\tParent(" << *vi << ") = " << p[*vi] << endl;
			#endif
			if (d[*vi]>1)
				if ( !switchPtr->addDestination(dst_id,p[*vi]) )
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Cannot add destination " << dst_id \
						<< " through " << p[*vi] << " to switch " << switchPtr->getId() << endl);
		}
	}
}

void packetNoc::addBinding(unsigned int spe, sc_dt::uint64 startAddress, sc_dt::uint64 endAddress) {
	slavePE* slaveToBind = getSlaveWithId(spe);
	if (startAddress > endAddress || slaveToBind == NULL) 
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while binding " << spe << ": check the addresses and the slave number" << endl);

	forwardMap_t::iterator fwIter;
	for (fwIter = forwardMap.begin(); fwIter != forwardMap.end(); fwIter++) {
		if (	( fwIter->first.first <= startAddress && startAddress <= fwIter->first.second ) ||
			( fwIter->first.first <= endAddress && endAddress <= fwIter->first.second ) )
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Addresses are conflicting with previous bindings!" << endl);
	}
	addressSet newBind(startAddress,endAddress);
	forwardMap[newBind] = spe;
}

void packetNoc::printBindings() {
	forwardMap_t::iterator fwIter;
	for (fwIter = forwardMap.begin(); fwIter != forwardMap.end(); fwIter++) {
		cout << "Slave Processing Element #" << fwIter->second << " bound to addresses from ";
		cout << fwIter->first.first << " to " << fwIter->first.second << endl;
	}
}

void packetNoc::printGRT() {
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		(*switchIter)->printLRT();
	}
}

void packetNoc::printStats() {
	cout << "The entire NOC had " << numAccesses << " accesses for a total of " << numWords << " words exchanged;" << endl;
	vector<masterPE*>::iterator masterIter;
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		(*masterIter)->printStats();
	}
	vector<slavePE*>::iterator slaveIter;
	for (slaveIter = sl_list.begin(); slaveIter != sl_list.end(); slaveIter++) {
		(*slaveIter)->printStats();
	}
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		(*switchIter)->printStats();
	}
}

unsigned int packetNoc::getBufferCurrentFreeSlots(unsigned int switchId, unsigned int portId){
  Switch* sw = getSwitchWithId(switchId);
  return sw->getBufferCurrentFreeSlots(portId);
}

std::vector<unsigned int> packetNoc::getActivePorts(unsigned int switchId){
  std::vector<unsigned int> activePorts;
  Switch* sw = getSwitchWithId(switchId);
  for(unsigned int i = 0; i < PORTS; i++){
    if(sw->isPortActive(i))
      activePorts.push_back(i);
  }
  return activePorts;
}


unsigned int packetNoc::getFlitsIn(unsigned int switchId, unsigned int portId) {
	masterPE* selMaster = getMasterWithId(switchId);
	if (selMaster != NULL) return selMaster->flitsIn;
	slavePE* selSlave = getSlaveWithId(switchId);
	if (selSlave != NULL) return selSlave->flitsIn;
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch != NULL) return selSwitch->getFlitsIn(portId);
}

unsigned int packetNoc::getFlitsOut(unsigned int switchId, unsigned int portId) {
	masterPE* selMaster = getMasterWithId(switchId);
	if (selMaster != NULL) return selMaster->flitsOut;
	slavePE* selSlave = getSlaveWithId(switchId);
	if (selSlave != NULL) return selSlave->flitsOut;
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch != NULL) return selSwitch->getFlitsOut(portId);
}

unsigned int packetNoc::getTimeouts(unsigned int masterId) {
	masterPE* selMaster = getMasterWithId(masterId);
	if (selMaster == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Master #" << masterId << " doesn't exists" << endl);
	return selMaster->timedOutSessions;
}

unsigned int packetNoc::getAllTimeouts() {
	unsigned int timeouts = 0;
	vector<masterPE*>::iterator masterIter;
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		timeouts += (*masterIter)->timedOutSessions;
	}	
	return timeouts;
}

unsigned int packetNoc::getDropped(unsigned int switchId, unsigned int portId) {
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Switch #" << switchId << " doesn't exists" << endl);
	return selSwitch->getDropped(portId);
}

unsigned int packetNoc::getAllDropped() {
	unsigned int dropped = 0;
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		for(unsigned int i  = 0; i < PORTS; i++)
		  dropped += (*switchIter)->getDropped(i);
	}
	return dropped;
}

void packetNoc::changeTimeout(unsigned int masterId, sc_time tO) {
	masterPE* selMaster = getMasterWithId(masterId);
	if (selMaster == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Master #" << masterId << " doesn't exists" << endl);
	selMaster->setTimeout(tO);
}

void packetNoc::changeAllTimeouts(sc_time tO) {
	vector<masterPE*>::iterator masterIter;
	for (masterIter = ms_list.begin(); masterIter != ms_list.end(); masterIter++) {
		(*masterIter)->setTimeout(tO);
	}	
}

unsigned int packetNoc::getBufferSize(unsigned int switchId, unsigned int portId){
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Switch #" << switchId << " doesn't exists" << endl);
	return selSwitch->getBufferSize(portId); 
}

void packetNoc::changeBufferSize(unsigned int switchId, unsigned int portId, unsigned int size) {
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Switch #" << switchId << " doesn't exists" << endl);
	selSwitch->setBufferSize(portId, size);
}

void packetNoc::changeAllBufferSizes(unsigned int size) {
	vector<Switch*>::iterator switchIter;
	for (switchIter = sw_list.begin(); switchIter != sw_list.end(); switchIter++) {
		for (unsigned int i = 0; i < PORTS; i++)
  		(*switchIter)->setBufferSize(i,size);
	}
}

void packetNoc::changePath(unsigned int switchId, unsigned int destinationId, unsigned int nextHop) {
	Switch* selSwitch = getSwitchWithId(switchId);
	if (selSwitch == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Switch #" << switchId << " doesn't exists" << endl);
	if (!selSwitch->modifyDestination(destinationId,nextHop))
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Cannot use " << nextHop \
			<< " as next hop for destination " << destinationId << " on switch " << switchId << "." << endl);
}

std::vector<unsigned int> packetNoc::getSwitchIds() {
  std::vector<unsigned int> ids;
  for(int i = 0; i < sw_list.size(); i++)
    ids.push_back(sw_list[i]->getId());
  return ids;
}
