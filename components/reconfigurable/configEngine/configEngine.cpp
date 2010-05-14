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
				initiatorSocket((boost::lexical_cast<std::string>(name) + "_initSock").c_str()),
				busSocket((boost::lexical_cast<std::string>(name) + "_busSock").c_str()),
				bitstream_source_address(bitstreamSource), sc_module(name), tab(delAlg) {
	requestDelay = SC_ZERO_TIME;
	execDelay = SC_ZERO_TIME;
	configDelay = SC_ZERO_TIME;
	removeDelay = SC_ZERO_TIME;
	busy=false;
	lastBinding=0;

	end_module();

	//status=0;
	#ifdef DEBUGMODE
	cerr << "Configuration Engine created with name " << name << endl;
	#endif
}

unsigned int configEngine::bindFPGA(sc_dt::uint64 dest_addr) {
	bitstream_dest_address[++lastBinding] = dest_addr;
	#ifdef DEBUGMODE
	cerr << "Bound eFPGA #" << lastBinding << " to destination address " << dest_addr << endl;
	#endif
	return lastBinding;
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
		#ifdef DEBUGMODE
		cerr << "Reusing previous function " << address << endl;
		#endif
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
		delay = SC_ZERO_TIME;
		this->initiatorSocket[i]->b_transport(message,delay);
		if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid during search among devices!" << endl);
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
			ostringstream stream;
			stream << " - ERROR: while removing from the central allocation table; probably it is already empty and the function you're trying to map is too big!" << endl;
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << stream.str());
		}
		#ifdef DEBUGMODE
		cerr << "Not enough space, removing at address " << whatDelete << "..." << endl;
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
		if (message.get_response_status() != TLM_OK_RESPONSE) {
			// Something wrong happened on the fabric...
			tab=tmpTable;
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);
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
			delay = SC_ZERO_TIME;
			this->initiatorSocket[i]->b_transport(message,delay);
			if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid during search among devices!" << endl);
			else {
				responseValue = messageData.answer;
				if (hasMin==-1 || (responseValue < min) ) {min=responseValue; hasMin=i;}
			}
		}
	}

	#ifdef DEBUGMODE
	cerr << "Best choice is # " << hasMin+1 << " with " << (int)min << endl;
	#endif

	// Inserts the new function data in the general table
	address = tab.add(funcName,hasMin+1);
	if (address == 0)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: while inserting in the central table!" << endl);

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
	this->busSocket->b_transport(message,delay);

	// 'WRITEs' the bitstream on the selected device
	message.set_write();
	map<unsigned int, sc_dt::uint64>::iterator dest_address = bitstream_dest_address.find(hasMin+1);
	if (dest_address == bitstream_dest_address.end()) THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: device " << hasMin+1 << " is not bound to any address!" << endl);
	message.set_address(dest_address->second);
	delay = SC_ZERO_TIME;
	this->busSocket->b_transport(message,delay);		// We send the 'write' message to the bitstream sink

	#ifdef DEBUGMODE
	cerr << tab.getName(address) << " configured at address " << address << " on device # " << tab.getDevice(address) << ";";
	cerr << " configuration required " << responseValue << " words." << endl;
	cerr << " --> it uses " << width << " X " << height << " cells and its execution will take " << latency << endl;
	cerr << " --> the reuse option is " << reUse << endl;
	#endif

	busy = false;
	configFree.notify();

	return address;
}

bool configEngine::execute(unsigned int funcAddr) {

	string name = tab.getName(funcAddr);
	unsigned int device = tab.getDevice(funcAddr);
	if ( device == 0 || name.length()==0)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: " << funcAddr << " not in the Configuration Engine table" << endl);

	#ifdef DEBUGMODE
	cerr << "Launching " << name << " on device " << device << endl;
	#endif
	// Inserting a fix delay in the simulated communication Processing Element -> eFPGA
	wait(execDelay);

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
	else return true;
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
	if (whatDelete == 0)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: while manual removing in the central table!" << endl);

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
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);
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
	if (whatDelete == 0)
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: while manual removing in the central table!" << endl);

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
		THROW_EXCEPTION(__PRETTY_FUNCTION__ << " - ERROR: TLM response not valid while removing from the device table!" << endl);
	}

	// Everything OK, function removed, inserting a fix delay
	wait(removeDelay);

	busy = false;
	configFree.notify();

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

