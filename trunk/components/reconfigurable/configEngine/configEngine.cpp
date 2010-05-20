/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
 *   Component developed by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#include "configEngine.hpp"

configEngine::configEngine(sc_module_name name, sc_dt::uint64 bitstreamSource, deletionAlgorithm delAlg):
				sc_module(name), initiatorSocket((boost::lexical_cast<std::string>(name) + "_initSock").c_str()),
				ramSocket((boost::lexical_cast<std::string>(name) + "_ramSock").c_str()),
				destSocket((boost::lexical_cast<std::string>(name) + "_destSock").c_str()),
				bitstream_source_address(bitstreamSource), tab(delAlg) {
	requestDelay = SC_ZERO_TIME;
	execDelay = SC_ZERO_TIME;
	configDelay = SC_ZERO_TIME;
	removeDelay = SC_ZERO_TIME;
	lastBinding=0;

	configBusy=false;
	curConfigEvent=0;
	curExecEvent=0;

	end_module();

	//status=0;
	#ifdef DEBUGMODE
	cerr << "Configuration Engine created with name " << name << endl;
	#endif
}

void configEngine::configLock() {
	if( this->configBusy ) {
		unsigned int myEvent = curConfigEvent++;
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Queuing config request on " << myEvent << endl;
		#endif
		nextConfigWake.push(myEvent);
		wakeConfigEvents[myEvent] = new sc_event;
		wait(*(this->wakeConfigEvents[myEvent]));
		delete wakeConfigEvents[myEvent];
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Starting config request " << myEvent << endl;
		#endif
	}
	this->configBusy = true;
}

void configEngine::configUnlock() {
	this->configBusy = false;
	if( !nextConfigWake.empty() ) {
		unsigned int nextEvent = this->nextConfigWake.front();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Waking up queued config request " << nextEvent << endl;
		#endif
		this->nextConfigWake.pop();
		(this->wakeConfigEvents[nextEvent])->notify();
		this->configBusy = true;
	}
}

void configEngine::execLock(unsigned int address) {
	list<unsigned int>::iterator execIter = executing.begin();
	bool execBusy = false;
	while (execIter!=executing.end()) {
		if(*execIter==address) execBusy = true;
		execIter++;
	}
	if ( execBusy ) {
		unsigned int myEvent = curExecEvent++;
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Queuing exe request on " << myEvent << " for address " << address << endl;
		#endif
		nextExecWake[address].push(myEvent);
		wakeExecEvents[myEvent] = new sc_event;
		wait(*(this->wakeExecEvents[myEvent]));
		delete wakeExecEvents[myEvent];
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Starting exe request " << myEvent << " for address " << address << endl;
		#endif
	}
	else {this->executing.push_back(address);}
}

void configEngine::execUnlock(unsigned int address) {
	list<unsigned int>::iterator execIter = executing.begin();
	while (execIter!=executing.end()) {
		if(*execIter==address) break;
		execIter++;
	}
	executing.erase(execIter);
	if( !nextExecWake[address].empty() ) {
		unsigned int nextEvent = this->nextExecWake[address].front();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Waking up queued exe request " << nextEvent << " for address " << address << endl;
		#endif
		this->nextExecWake[address].pop();
		(this->wakeExecEvents[nextEvent])->notify();
		this->executing.push_back(address);
	}
}

unsigned int configEngine::bindFPGA(sc_dt::uint64 dest_addr) {
	bitstream_dest_address[++lastBinding] = dest_addr;
	#ifdef DEBUGMODE
	cerr << "Bound eFPGA #" << lastBinding << " to destination address " << dest_addr << endl;
	#endif
	return lastBinding;
}

unsigned int configEngine::config(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse) {

	sc_time delay = SC_ZERO_TIME;
	tlm_generic_payload message;
	payloadData messageData;

	unsigned char* payload_buffer = (unsigned char*) &messageData;
	unsigned int payload_length = sizeof(payloadData);

	unsigned int i, responseValue, min, address;
	int hasMin=-1;

	// Inserting a fix delay in the simulated communication Processing Element -> Configuration Engine
	wait(requestDelay);

	// If reUse is activated and the function is already mapped, the old address is returned
	address=tab.getAddress(funcName);
	if (reUse && address!=0) {
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Reusing previous function " << address << endl;
		#endif
		return address;
	}

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
		delay = SC_ZERO_TIME;
		this->initiatorSocket[i]->b_transport(message,delay);
		if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid during search among devices!" << endl);
		else {
			responseValue = messageData.answer;
			if ( (int)responseValue!=-1 && (hasMin==-1 || ((int)responseValue < (int)min) ) ) {min=responseValue; hasMin=i;}
		}
	}

	while (hasMin == -1) {
		// Every device answered -1...no space on any device!
		unsigned int whatDelete,whereDelete,check;
		bool executed;
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Not enough space, searching for module to delete" << endl;
		#endif
		list<unsigned int> deleteList = tab.removeList();
		list<unsigned int>::iterator deleteIter = deleteList.begin();
		if ( deleteIter == deleteList.end() ) {
			ostringstream stream;
			stream << " - ERROR: while removing from the central allocation table; no functions are currently mapped;";
			stream << " probably the function you're trying to map is too big!" << endl;
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << stream.str());
		}
		list<unsigned int>::iterator execIter;
		do {
			whatDelete = *deleteIter;
			executed = false;
			for (execIter = executing.begin(); execIter!=executing.end(); execIter++) {
				if (whatDelete == *execIter) executed = true;
			}
			#ifdef DEBUGMODE
			if (executed)
				cerr << sc_time_stamp() << ": " << name() << " - " << whatDelete << " is being executed, thus it cannot be removed" << endl;
			#endif
			deleteIter++;
		} while(executed && deleteIter!=deleteList.end());
		// cEAllocationTable tmpTable = tab;							Table Backup: currently unuseful
		if (executed && deleteIter==deleteList.end()) {
			whatDelete = deleteList.front();
			#ifdef DEBUGMODE
			cerr << sc_time_stamp() << ": " << name() << " - All the modules are being executed: waiting for module " << whatDelete << " to be freed" << endl;
			#endif
			this->execLock(whatDelete);
			check = tab.remove(whatDelete,&whereDelete);
			this->execUnlock(whatDelete);
		}
		else check = tab.remove(whatDelete,&whereDelete);

		if (check != whatDelete)
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR - Unexpected error while removing " << whatDelete << " from central table" << endl);

		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " -  removing at address " << whatDelete << "..." << endl;
		#endif

		// Sends the removing command to device whereDelete-1
		messageData.address = whatDelete;
		messageData.latency = SC_ZERO_TIME;
		messageData.width = 0;
		messageData.height = 0;
		message.set_data_ptr(payload_buffer);
		message.set_write();
		message.set_address(1);
		message.set_response_status(TLM_INCOMPLETE_RESPONSE);
		delay = SC_ZERO_TIME;
		this->initiatorSocket[whereDelete-1]->b_transport(message,delay);
		if (message.get_response_status() != TLM_OK_RESPONSE)
			// Something wrong happened on the fabric...
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);

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
			delay = SC_ZERO_TIME;
			this->initiatorSocket[i]->b_transport(message,delay);
			if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid during search among devices!" << endl);
			else {
				responseValue = messageData.answer;
				if ( (int)responseValue!=-1 && (hasMin==-1 || ((int)responseValue < (int)min) ) ) {min=responseValue; hasMin=i;}
			}
		}
	}

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Best choice is # " << hasMin+1 << " with " << (int)min << endl;
	#endif

	// Inserts the new function data in the general table
	address = tab.add(funcName,hasMin+1);
	if (address == 0) {
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - ERROR: while inserting in the central table!" << endl;
		#endif
		return 0;
	}

	// Trick to prevent the execution of a function not fully configured
	this->execLock(address);

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
	delay = SC_ZERO_TIME;
	this->initiatorSocket[hasMin]->b_transport(message,delay);
	// Gets back the number of words of the bitstream, to be transferred from memory to device
	if (message.get_response_status() != TLM_OK_RESPONSE)
		// Something wrong happened on the fabric...
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while inserting in the device table!" << endl);
	responseValue = messageData.answer;

	// Function successfully configured, inserting a fix delay
	wait(configDelay);

	// 'READs' on the memory the bitstream to be configured on the selected device
	unsigned int bitstream_length = responseValue*sizeof(unsigned int);
	unsigned int* bitstream = (unsigned int*) malloc (bitstream_length);
	message.set_read();
	message.set_address(bitstream_source_address);		// We start to read the bitstream...
        message.set_data_length(bitstream_length);		// using the appropriate length!
	message.set_data_ptr((unsigned char*)bitstream);
	message.set_response_status(TLM_INCOMPLETE_RESPONSE);
	delay = SC_ZERO_TIME;
	this->ramSocket->b_transport(message,delay);

	// 'WRITEs' the bitstream on the selected device
	message.set_write();
	map<unsigned int, sc_dt::uint64>::iterator dest_address = bitstream_dest_address.find(hasMin+1);
	if (dest_address == bitstream_dest_address.end()) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: device " << hasMin+1 << " is not bound to any address!" << endl);
	message.set_address(dest_address->second);
	delay = SC_ZERO_TIME;
	if (this->destSocket.size() == 1) {
		this->destSocket->b_transport(message,delay);		// We send the 'write' message to the unique bitstream sink
	}
	else if (this->destSocket.size() > 1) {
		if (this->destSocket[hasMin] == NULL) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: bitstream sink for device " << hasMin+1 << " is not connected!" << endl);
		this->destSocket[hasMin]->b_transport(message,delay);	// We send the 'write' message to the correct bitstream sink
	}

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - " << tab.getName(address) << " configured at address " << address;
	cerr << " on device # " << tab.getDevice(address) << ";" << " configuration required " << responseValue << " words." << endl;
	cerr << " --> it uses " << width << " X " << height << " cells and its execution will take " << latency << endl;
	cerr << " --> the reuse option is " << reUse << endl;
	#endif

	// Now the fully configured function can be executed
	this->execUnlock(address);

	return address;
}

bool configEngine::exec(unsigned int funcAddr) {

	string name = this->tab.getName(funcAddr);
	unsigned int device = this->tab.getDevice(funcAddr);
	if ( device == 0 || name.length()==0) {
//		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: " << funcAddr << " not in the Configuration Engine table" << endl);
		cerr << sc_time_stamp() << ": " << this->name() << " - ERROR: Module #" << funcAddr << " not found on any device: configuration should be re-issued..." << endl;
		return false;
	}

	// Inserting a fix delay in the simulated communication Processing Element -> eFPGA
	wait(execDelay);

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << this->name() << " - Launching " << name << " #" << funcAddr << " on device " << device << endl;
	#endif

	sc_time delay = SC_ZERO_TIME;
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

	if ( message.get_response_status() != TLM_OK_RESPONSE )
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while executing from the device!" << endl);
	else if (!tab.exec(funcAddr))
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: This shouldn't be happening...the function existed at the beginning of the execution..." << endl);
	else {
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << this->name() << " - Function at #" << funcAddr << " executed on device " << device << endl;
		#endif
		return true;
	}
}

unsigned int configEngine::configure(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse) {
	this->configLock();
	unsigned int retVal = this->config(funcName,latency,width,height,reUse);
	this->configUnlock();
	return retVal;
}

bool configEngine::execute(unsigned int funcAddr) {
	this->execLock(funcAddr);
	bool retVal = this->exec(funcAddr);
	this->execUnlock(funcAddr);
	return retVal;
}

bool configEngine::execute(string funcName) {
	list<unsigned int> addresses = tab.getAddressList(funcName);
	list<unsigned int>::iterator toExecIter = addresses.begin();
	unsigned int address;
	if ( toExecIter == addresses.end() ) {
		cerr << sc_time_stamp() << ": " << this->name() << " - ERROR: Module " << funcName << " not found on any device: it cannot be executed..." << endl;
		return false;
	}
	list<unsigned int>::iterator execIter;
	bool executed;
	do {
		address = *toExecIter;
		executed = false;
		for (execIter = executing.begin(); execIter!=executing.end(); execIter++) {
			if (address == *execIter) executed = true;
		}
		#ifdef DEBUGMODE
		if (executed)
			cerr << sc_time_stamp() << ": " << name() << " - function at #" << address << " is already being executed..." << endl;
		#endif
		toExecIter++;
	} while(executed && toExecIter!=addresses.end());

	if (executed && toExecIter==addresses.end()) {
		address = addresses.front();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - All the modules are being executed: waiting for module " << address << " to be released" << endl;
		#endif
	}
	this->execLock(address);
	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Found function " << funcName << " at " << address << endl;
	#endif
	bool retVal = this->exec(address);
	this->execUnlock(address);
	return retVal;
}

void configEngine::executeForce(string funcName, sc_time latency, unsigned int width, unsigned int height, bool reUse) {
	this->configLock();
	unsigned int retVal;
	do {retVal=this->config(funcName,latency,width,height,reUse);} while(retVal==0);
	bool executed;
	do {
		this->execLock(retVal);
		executed = this->exec(retVal);
		this->execUnlock(retVal);
		if (!executed) do {retVal=this->config(funcName,latency,width,height,reUse);} while(retVal==0);
	} while (!executed);
	this->configUnlock();
}

bool configEngine::manualRemove(unsigned int funcAddr) {

	list<unsigned int> deleteList = tab.removeList();
	list<unsigned int>::iterator deleteIter = deleteList.begin();
	if ( deleteIter == deleteList.end() ) {
		ostringstream stream;
		stream << " - ERROR: while removing from the central allocation table; no functions are currently mapped;";
		stream << " probably the function you're trying to map is too big!" << endl;
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << stream.str());
	}
	bool existing = false;
	for (deleteIter = deleteList.begin(); deleteIter!=deleteList.end(); deleteIter++) {
		if (funcAddr == *deleteIter) existing = true;
	}
	if (!existing)  {
//		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: while manual removing in the central table!" << endl);
		cerr << sc_time_stamp() << ": " << this->name() << " - ERROR: Module #" << funcAddr << " not found on any device: it cannot be removed..." << endl;
		return false;
	}
	bool executed = false;
	list<unsigned int>::iterator execIter;
	for (execIter = executing.begin(); execIter!=executing.end(); execIter++) {
		if (funcAddr == *execIter) executed = true;
	}

	// cEAllocationTable tmpTable = tab;							Table Backup: currently unuseful
	unsigned int whatDelete, whereDelete;
	if (executed) {
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - " << funcAddr << " is being executed, thus it cannot be removed. Waiting for its termination" << endl;
		#endif
		this->execLock(funcAddr);
		whatDelete = tab.remove(funcAddr,&whereDelete);	
		this->execUnlock(whatDelete);
	}
	else whatDelete = tab.remove(funcAddr,&whereDelete);		
	if (whatDelete != funcAddr)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR - Unexpected error while removing " << funcAddr << " from central table" << endl);

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
	if ( message.get_response_status() != TLM_OK_RESPONSE )
		// Something wrong happened on the fabric...
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);

	// Everything OK, function removed, inserting a fix delay
	wait(removeDelay);

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Function at address " << whatDelete << " manually removed" << endl;
	#endif

	return true;
}

bool configEngine::manualRemove(string funcName) {

	unsigned int whatDelete,whereDelete,check;
	bool executed;
	list<unsigned int> deleteList = tab.getAddressList(funcName);
	list<unsigned int>::iterator deleteIter = deleteList.begin();
	if ( deleteIter == deleteList.end() ) {
		cerr << sc_time_stamp() << ": " << this->name() << " - ERROR: Module " << funcName << " not found on any device: it cannot be removed..." << endl;
		return false;
	}
	list<unsigned int>::iterator execIter;
	do {
		whatDelete = *deleteIter;
		executed = false;
		for (execIter = executing.begin(); execIter!=executing.end(); execIter++) {
			if (whatDelete == *execIter) executed = true;
		}
		#ifdef DEBUGMODE
		if (executed)
			cerr << sc_time_stamp() << ": " << name() << " - " << whatDelete << " is being executed, thus it cannot be removed" << endl;
		#endif
		deleteIter++;
	} while(executed && deleteIter!=deleteList.end());

	// cEAllocationTable tmpTable = tab;							Table Backup: currently unuseful
	if (executed && deleteIter==deleteList.end()) {
		whatDelete = deleteList.front();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - All the modules are being executed: waiting for module " << whatDelete << " to be freed" << endl;
		#endif
		this->execLock(whatDelete);
		check = tab.remove(whatDelete,&whereDelete);
		this->execUnlock(whatDelete);
		if (check == 0) {
			cerr << sc_time_stamp() << ": " << this->name() << " - ERROR: Module " << funcName << " has already been removed while waiting for access..." << endl;
			return false;
		}
	}
	else check = tab.remove(whatDelete,&whereDelete);

	if (check != whatDelete)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR - Unexpected error while removing " << whatDelete << " from central table" << endl);

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Manually removing at address " << whatDelete << "..." << endl;
	#endif

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
	if ( message.get_response_status() != TLM_OK_RESPONSE )
		// Something wrong happened on the fabric...
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);

	// Everything OK, function removed, inserting a fix delay
	wait(removeDelay);

	#ifdef DEBUGMODE
	cerr << sc_time_stamp() << ": " << name() << " - Function at address " << whatDelete << " manually removed" << endl;
	#endif

	return true;
}

void configEngine::printSystemStatus() {

	tab.printStatus();

	if (sc_start_of_simulation_invoked()) {
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

