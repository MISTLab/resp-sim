#include "configEngine.hpp"

configEngine::configEngine(sc_module_name name, ABIIf<unsigned int> &processorInstance, unsigned long long configAddress, deletionAlgorithm delAlg):
				initiatorSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()),
				sc_module(name), tab(delAlg), bitstream_address(configAddress), recEmu(processorInstance) {
	requestDelay = SC_ZERO_TIME;
	execDelay = SC_ZERO_TIME;
	configDelay = SC_ZERO_TIME;
	removeDelay = SC_ZERO_TIME;
	busy=false;

	end_module();

	//status=0;
	//cerr << "Configuration Engine created with name " << name << endl;
}

unsigned int configEngine::configure(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse) {

	sc_time delay = SC_ZERO_TIME;
	tlm_generic_payload message;
	payloadData messageData;

	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);

	unsigned int i, responseValue, min, address, whatDelete, whereDelete;
	int hasMin=-1;

	// If reUse is activated and the function is already mapped, the old address is returned
	address=tab.getAddress(funcName);
	if (reUse && address!=0) {
		//cerr << "Reusing previous function " << address << endl;
		return address;
	}

	while( busy ) {
		wait( configFree );
	}
	busy = true;

	// Inserting a fix delay in the simulated communication Processing Element -> Configuration Engine
	wait(requestDelay);

	// Searches for the device with the best (minimum) euristic value
	messageData.address = 0;
	messageData.latency = SC_ZERO_TIME;
	messageData.width = width;
	messageData.height = height;
	messageData.answer = 0;
        message.set_data_length(payload_length);
	message.set_data_ptr(payload_buffer);
	message.set_read();
	message.set_address(1);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	for (i = 0; i < initiatorSocket.size(); i++) {
		this->initiatorSocket[i]->b_transport(message,delay);
		if (message.get_response_status() != TLM_OK_RESPONSE) cerr << "Search: Response error!" << endl;
		else {
			responseValue = messageData.answer;
			if (hasMin==-1 || (responseValue < min) ) {min=responseValue; hasMin=i;}
		}
	}

	while (hasMin == -1) {
		// Every device answered -1...no space on any device!
		cEAllocationTable tmpTable = tab;
		whatDelete = tab.remove(&whereDelete);
		if (whatDelete == 0) {
			cerr << "Error signaled while removing from the central table: ";
			cerr << "probably it is empty and the function you're trying to map is too big!" << endl;
			THROW_EXCEPTION("ERROR: no more modules can be removed; function is too big for this device" << endl);
			return 0;
		}
		//cerr << "Not enough space, removing at address " << whatDelete << "..." << endl;

		// Sends the removing command to device whereDelete-1
		messageData.address = whatDelete;
		messageData.latency = SC_ZERO_TIME;
		messageData.width = 0;
		messageData.height = 0;
		message.set_data_ptr(payload_buffer);
		message.set_write();
		message.set_address(1);
		message.set_response_status(TLM_INCOMPLETE_RESPONSE);
		this->initiatorSocket[whereDelete-1]->b_transport(message,delay);
		if (message.get_response_status() != TLM_OK_RESPONSE) {
			// Something wrong happened on the fabric...
			tab=tmpTable;
			THROW_EXCEPTION("ERROR: while removing from the device table!" << endl);
			return 0;
		}

		// Everything OK, function removed, inserting a fix delay
		wait(removeDelay);

		// Searches again for the device with the best (minimum) euristic value
		messageData.address = 0;
		messageData.latency = SC_ZERO_TIME;
		messageData.width = width;
		messageData.height = height;
		messageData.answer = 0;
	        message.set_data_length(payload_length);
		message.set_data_ptr(payload_buffer);
		message.set_read();
		message.set_address(1);
		message.set_response_status(TLM_INCOMPLETE_RESPONSE);
		for (i = 0; i < initiatorSocket.size(); i++) {
			this->initiatorSocket[i]->b_transport(message,delay);
			if (message.get_response_status() != TLM_OK_RESPONSE) cerr << "Search: Response error!" << endl;
			else {
				responseValue = messageData.answer;
				if (hasMin==-1 || (responseValue < min) ) {min=responseValue; hasMin=i;}
			}
		}
	}

	//cerr << "Best choice is # " << hasMin+1 << " with " << (int)min << endl;

	// Inserts the new function data in the general table
	address = tab.add(funcName,hasMin+1);
	if (address == 0) {
		THROW_EXCEPTION("ERROR: while inserting in the central table!" << endl);
		return 0;
	}

	// Inserts the new function data in the selected device table
	messageData.address = address;
	messageData.latency = latency;
	messageData.width = width;
	messageData.height = height;
	messageData.answer = 0;
	message.set_data_ptr(payload_buffer);
	message.set_write();
	message.set_address(0);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	this->initiatorSocket[hasMin]->b_transport(message,delay);
	// Gets back the number of words of the bitstream, to be transferred from memory to device
	if (message.get_response_status() != TLM_OK_RESPONSE) {
		// Something wrong happened on the fabric...
		THROW_EXCEPTION("ERROR: signaled while inserting in the device table!" << endl);
		if (tab.remove(address,&whereDelete)==0) THROW_EXCEPTION("ERROR: This shouldn't be happening...didn't found just inserted function..." << endl);
		return 0;
	}
	responseValue = messageData.answer;

	// Function successfully configured, inserting a fix delay
	wait(configDelay);

	// READs on the memory the bitstream to be configured on the selected device
//	configrequest.set_command(READ);
//	configrequest.set_address(0);			// We start to read at address 0 (usually attached to main memory)...
//	configrequest.set_block_address_incr(0);	// and for each block we do not increment the address!
//	configrequest.set_block_size(responseValue);	// Remeber: each block introduces a bus latency + memory latency
//	this->bus_init_port->transport(configrequest, response);

	// 'WRITEs' the bitstream on the selected device
//	configrequest.set_command(WRITE);
//	configrequest.set_address(bitstream_address);	// We send the message to the bitstream sink
//	this->bus_init_port->transport(configrequest, response);

	//cerr << tab.getName(address) << " configured at address " << address << " on device # " << tab.getDevice(address) << ";";
	//cerr << " configuration required " << responseValue << " words." << endl;
	//cerr << " --> it uses " << width << " X " << height << " cells and its execution will take " << latency << endl;
	//cerr << " --> the reuse option is " << reUse << endl;

	busy = false;
	configFree.notify();

	return address;
}

bool configEngine::execute(unsigned int funcAddr) {

	string name = tab.getName(funcAddr);
	unsigned int device = tab.getDevice(funcAddr);
	if ( device == 0 || name.length()==0) {
		THROW_EXCEPTION(funcAddr << " not in the Configuration Engine table" << endl);
		return false;
	}

	//cerr << "Launching " << name << " on device " << device << endl;
	// Inserting a fix delay in the simulated communication Processing Element -> eFPGA
	wait(execDelay);

	sc_time delay = sc_time(18,SC_PS);
	tlm_generic_payload message;
	payloadData messageData;
	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);
	messageData.address = funcAddr;
	messageData.latency = SC_ZERO_TIME;
	messageData.width = 0;
	messageData.height = 0;
        message.set_data_length(payload_length);
	message.set_data_ptr(payload_buffer);
	message.set_read();
	message.set_address(0);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	this->initiatorSocket[device-1]->b_transport(message,delay);

	if ( message.get_response_status() != TLM_OK_RESPONSE ) {
		THROW_EXCEPTION("ERROR: while executing from the device!" << endl);
		return false;
	}
	else {
		if (!tab.exec(funcAddr)) {
			THROW_EXCEPTION("ERROR: This shouldn't be happening...the function existed at the beginning of the execution..." << endl);
			return false;
		}
		return true;
	}
}

bool configEngine::confexec(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse) {
	unsigned int addr = configure(funcName, latency, width, height, reUse);
	if (addr == 0) return false;
	if (!execute(addr)) return false;
	return true;
}

bool configEngine::manualRemove(unsigned int funcAddr) {

	while( busy ) {
		wait( configFree );
	}
	busy = true;

	cEAllocationTable tmpTable = tab;
	unsigned int whatDelete, whereDelete;
	whatDelete = tab.remove(funcAddr,&whereDelete);
	if (whatDelete == 0) {
		THROW_EXCEPTION("ERROR: while removing in the central table!" << endl);
		return false;
	}

	sc_time delay = SC_ZERO_TIME;
	tlm_generic_payload message;
	payloadData messageData;
	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);
	messageData.address = whatDelete;
	messageData.latency = SC_ZERO_TIME;
	messageData.width = 0;
	messageData.height = 0;
        message.set_data_length(payload_length);
	message.set_data_ptr(payload_buffer);
	message.set_write();
	message.set_address(1);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	this->initiatorSocket[whereDelete-1]->b_transport(message,delay);
	if ( message.get_response_status() != TLM_OK_RESPONSE ) {
		// Something wrong happened on the fabric...
		tab=tmpTable;
		THROW_EXCEPTION("ERROR: while removing from the device table!" << endl);
		return false;
	}

	// Everything OK, function removed, inserting a fix delay
	wait(removeDelay);

	busy = false;
	configFree.notify();

	return true;
}

bool configEngine::manualRemove(string funcName) {

	while( busy ) {
		wait( configFree );
	}
	busy = true;

	cEAllocationTable tmpTable = tab;
	unsigned int whatDelete, whereDelete;
	whatDelete = tab.remove(funcName,&whereDelete);
	if (whatDelete == 0) {
		THROW_EXCEPTION("ERROR: while removing in the central table!" << endl);
		return false;
	}

	sc_time delay = SC_ZERO_TIME;
	tlm_generic_payload message;
	payloadData messageData;
	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);
	messageData.address = whatDelete;
	messageData.latency = SC_ZERO_TIME;
	messageData.width = 0;
	messageData.height = 0;
        message.set_data_length(payload_length);
	message.set_data_ptr(payload_buffer);
	message.set_write();
	message.set_address(1);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	this->initiatorSocket[whereDelete-1]->b_transport(message,delay);
	if ( message.get_response_status() != TLM_OK_RESPONSE ) {
		// Something wrong happened on the fabric...
		tab=tmpTable;
		THROW_EXCEPTION("ERROR: while removing from the device table!" << endl);
		return false;
	}

	// Everything OK, function removed, inserting a fix delay
	wait(removeDelay);

	busy = false;
	configFree.notify();

	return true;
}

void configEngine::printSystemStatus() {

	tab.printStatus();

	sc_time delay = SC_ZERO_TIME;
	tlm_generic_payload message;
	payloadData messageData;
	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);
	messageData.address = 0;
	messageData.latency = SC_ZERO_TIME;
	messageData.width = 0;
	messageData.height = 0;
        message.set_data_length(payload_length);
	message.set_data_ptr(payload_buffer);
	message.set_read();
	message.set_address(2);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	for (int i = 0; i < initiatorSocket.size(); i++) {
		this->initiatorSocket[i]->b_transport(message,delay);
	}
}

void configEngine::setRequestDelay(sc_time new_time) {
	requestDelay = new_time;
}

void configEngine::setExecDelay(sc_time new_time) {
	execDelay = new_time;
}

void configEngine::setConfigDelay(sc_time new_time) {
	configDelay = new_time;
}

void configEngine::setRemoveDelay(sc_time new_time) {
	removeDelay = new_time;
}

void configEngine::registerPythonCall(string funName, reconfCB<unsigned int> &callBack){
	this->recEmu.register_call(funName, callBack);
}

