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
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
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

#ifndef SIMPLEBUS_HPP
#define SIMPLEBUS_HPP

#include <string>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <boost/lexical_cast.hpp>

//#include "controller.hpp"
#include "utils.hpp"

//#define PROC_NUM_ADDR 0xF0000004

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename ADDRESS> class address_map {
protected:
	unsigned int m_target_port_rank;	// Target port binding rank (local initiator port <-> remote target port binding order)
	string m_entry_port_name;		// Name of the mapfile port name entry corresponding to the address range
	ADDRESS m_start_address;		// Start address
	ADDRESS m_end_address;			// End address
	bool lockBus;
public:
	address_map() :
		m_target_port_rank(0), m_start_address(0), m_end_address(0), lockBus(true)
			{}
	address_map(string& name, ADDRESS start, ADDRESS end, bool lock) :
		m_target_port_rank(0), m_entry_port_name(name), m_start_address(start), m_end_address(end), lockBus(lock)
			{}

	inline unsigned int get_target_port_rank() const {return(m_target_port_rank);}
	inline void set_target_port_rank(unsigned int target_port_rank) {m_target_port_rank = target_port_rank;}

	inline string& get_entry_port_name() {return(m_entry_port_name);}
	inline void set_entry_port_name(const char* name) {m_entry_port_name = name;}
	inline void set_entry_port_name(const string& name) {m_entry_port_name = name;}

	inline ADDRESS get_start_address() const {return(m_start_address);}
	inline void set_start_address(ADDRESS address) {m_start_address = address;}

	inline ADDRESS get_end_address() const {return(m_end_address);}
	inline void set_end_address(ADDRESS address) {m_end_address = address;}

	inline bool hasToLock() const {return this->lockBus;}
	inline void setLock(bool lockVal) {this->lockBus = lockVal;}

	/*--------------------------------------------------------------
	 * Decode method used to check port address range
	 * Returns true if input address matches the port address range
	 *--------------------------------------------------------------*/
	inline bool decode(const ADDRESS& address) const {
		return((address>=m_start_address)&&(address<=m_end_address));
	}
};


/*------------------------------------------------------------------------
 * Documentation on BusLT: it is a simple router taken from TLM2 examples
 *------------------------------------------------------------------------*/
template<typename BUSWIDTH> class BusLT: public sc_module {

	typedef multi_passthrough_target_socket<BusLT, sizeof(BUSWIDTH)*8> target_socket_type;
	typedef multi_passthrough_initiator_socket<BusLT, sizeof(BUSWIDTH)*8> initiator_socket_type;
	typedef address_map<sc_dt::uint64> address_map_type;

private:
	vector<address_map_type*> m_address_map_list;			// List of address_map objects
	unsigned int curr_target_port_rank;

	bool busy;
	bool locking;
	map<unsigned int, sc_event* > events;
	queue<unsigned int> requests;

	/*----------------------------------------------------------------------------------
	 * If the given address matches one address map, returns a reference on the address
	 * map. The address parameter is modified: remove address map start address
	 *----------------------------------------------------------------------------------*/
	inline unsigned int decode(sc_dt::uint64& address, bool& lock) {
		// For each registered & mapped target port
		for(typename vector<address_map_type *>::iterator map = m_address_map_list.begin(); map != m_address_map_list.end(); map++) {
			if ((*map)->decode(address)) {
				address = address - (*map)->get_start_address();
				lock = (*map)->hasToLock();
				return(*map)->get_target_port_rank();
			}
		}
		return initiatorSocket.size();
	}

	void lock(int tag) {
		// If there is more than one master component, requests have to be queued: there is an array of events, one
		// for each master component connected: the master component waits on this element (if the master is busy);
		// when it becomes free it goes on (and sets the bus to busy): at the end of its requests, it notifies the
		// next element in queue.
		if( this->busy ) {
//			cerr << "bus" << ": Queuing request" << endl;
			this->requests.push(tag);
			wait(*(this->events[tag]));
		}
		if (locking)
			this->busy = true;
	}

	void unlock() {
		//Now we check if there are some elements which need to be awakened
		this->busy = false;
		if( !requests.empty() ) {
			unsigned int front = this->requests.front();
//			cerr << "bus" << ": Waking up a queued request" << endl;
			this->requests.pop();
			(this->events[front])->notify();
		}
	}

public:
	sc_time latency;
	unsigned int numAccesses;
	unsigned int numWords;
	unsigned int *accessCounter;
	unsigned int numMasters;
	target_socket_type targetSocket;
	initiator_socket_type initiatorSocket;

	BusLT(sc_module_name module_name, unsigned int numMasters, sc_time latency = SC_ZERO_TIME) : 
			initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_initSock").c_str()),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()),
			latency(latency), numMasters(numMasters) {
		this->numAccesses = 0;
		this->numWords = 0;
		this->accessCounter = (unsigned int*) malloc(2*numMasters*sizeof(unsigned int));
		for(int i = 0; i < numMasters; i++){
			events[i] = new sc_event;
			accessCounter[2*i]=0;
			accessCounter[2*i+1]=0;
		}
		this->targetSocket.register_b_transport(this, &BusLT::b_transport);
		this->targetSocket.register_get_direct_mem_ptr(this, &BusLT::get_direct_mem_ptr);
		this->targetSocket.register_transport_dbg(this, &BusLT::transport_dbg);
		this->busy = false;
		this->curr_target_port_rank = 0;
		end_module();
	}

	~BusLT() {
		for(typename vector<address_map_type*>::iterator map = m_address_map_list.begin();
				map != m_address_map_list.end(); map ++) {
			if ((*map)) delete (*map);
		}
		free(accessCounter);
	}

	/*---------------------------------------------------------
	 * Adds an address binding to the next unbound port
	 *                 (according to curr_target_port_rank)
	 * The portName is not bound to the target name, it
	 * is used only to name the binding in the printout.
	 * The user should add the bindings in the exact order
	 * used to connect the bus target ports to the peripherals
	 *---------------------------------------------------------*/
	void addBinding(string portName, sc_dt::uint64 startAddr, sc_dt::uint64 endAddr, bool lock = true){
		address_map_type* map = new address_map_type(portName, startAddr, endAddr, lock);

		if(endAddr < startAddr){
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Mapping of " << portName << " - End address " << endAddr << " smaller than start address " << startAddr);
		}

		map->set_target_port_rank(curr_target_port_rank++);
		m_address_map_list.push_back(map);

		// Address map overlap detection
		// For each registered address map object, the other address map objects are
		// checked to detect mapping overlap
		for(typename std::vector<address_map_type *>::iterator map = m_address_map_list.begin();
				map != m_address_map_list.end(); map++) {
			for(typename std::vector<address_map_type *>::iterator other_map = map + 1;
					other_map != m_address_map_list.end(); other_map++) {
				if  (  (  (*other_map)->decode((*map)->get_start_address() ) ) ||
						( (*other_map)->decode((*map)->get_end_address() ) )   ||
						( (*map)->decode((*other_map)->get_start_address() ) ) ||
						( (*map)->decode((*other_map)->get_end_address() ) )) {
					// Print a Warning message if a map definition collision is detected
					ostringstream stream;
					stream << "WARNING\t" << name() << ": "
						<< showbase << hex  << " \"" << (*other_map)->get_entry_port_name() << "\" map definition ("
						<< (*other_map)->get_start_address() << " - " << (*other_map)->get_end_address() << ")"<< endl
						<< "\t\toverlaps map defined for \"" << (*map)->get_entry_port_name() << "\" ("
						<< (*map)->get_start_address() << " - " << (*map)->get_end_address() << ")";
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Collision among bus addresses detected: " << stream.str());
				}
			}
		}
	}

	/*-------------------------------------
	 * Prints out the current bus bindings
	 *-------------------------------------*/
	void printBindings() {
		for(typename std::vector<address_map_type *>::iterator map = m_address_map_list.begin();
				map != m_address_map_list.end(); map++) {
			cout << (*map)->get_entry_port_name() << " " << (*map)->get_target_port_rank() << " ";
			cout << (*map)->get_start_address() << " " << (*map)->get_end_address() << endl;
		}
	}

	/*---------------------------------------
	 * Prints out the number of bus accesses
	 *---------------------------------------*/
	void printAccesses() {
		for (unsigned int i = 0; i < numMasters; i++) {
			cout << "Accesses with tag " << i << ":\t" << accessCounter[i] << endl;
		}
		cout << "Total Accesses:\t\t" << numAccesses << endl;
	}

	/*-----------------------------------
	 * b_transport method implementation
	 *-----------------------------------*/
	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay) {

		sc_dt::uint64 addr = trans.get_address();
//		cerr << "Incoming address was " << addr << " from " << tag;
		unsigned int portId = decode(addr,this->locking);
		if ( portId >= initiatorSocket.size() ) {
			trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
			ostringstream stream;
			stream << " " << name() << showbase << hex << ": No target at this address: "
				<< addr << " Requesting Master: " << dec << tag << " Decoded device: " << portId;
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << stream.str());
		}
//		cerr << ": requested " << addr << " on device # " << portId << endl;
		trans.set_address(addr);

		this->lock(tag);

		initiatorSocket[portId]->b_transport(trans,delay);
		unsigned int len = trans.get_data_length();
		unsigned int words = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) words++;

		// THIS METHOD IS NOT CORRECT! THE BUS TRANSACTION SEEMS TO BE INSTANTANEOUS AND CONCURRENT ACCESSES CANNOT BE CORRECTLY MODELED
		//	delay += words*this->latency;

		wait(words*this->latency);
		this->accessCounter[tag]++;
		this->numAccesses++;
		this->numWords+=words;
		trans.set_dmi_allowed(false);			// Disables DMI in order to insert the bus latency for each transaction

		//resp::sc_controller::getController().get_simulated_time()
//		ofstream outFile("bus.txt", ios::app);
//		outFile << /*sc_time_stamp()*/tag << " " << trans.get_address() << " " << /*trans.get_data_length() << " " <<*/ trans.is_write();
//		if (trans.get_data_length() == 4) outFile << " " << (unsigned int) (*trans.get_data_ptr()) << endl;
//		else outFile << endl;

		this->unlock();
	}

	bool get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data){
		cout << "DMI not supported for bus transactions! DMI requested by " << tag << endl;
		return false;
	}

	unsigned int transport_dbg(int tag, tlm::tlm_generic_payload& trans) {

		sc_dt::uint64 addr = trans.get_address();
//		cerr << "DBG - Incoming address was " << addr << " from " << tag;
/*		if(addr == PROC_NUM_ADDR){
			//I want to read the number of processors present in the system
			response.set_data(target_port.size());
			response.get_status().set_ok();
			return;
		}
*/		unsigned int portId = decode(addr,this->locking);
		if ( portId >= initiatorSocket.size() ) {
			trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
			ostringstream stream;
			stream << " " << name() << showbase << hex << ": No target at this address: "
				<< addr << " Requesting Master: " << dec << tag << " Decoded device: " << portId;
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << stream.str());
		}
//		cerr << ": requested " << addr << " on device # " << portId << endl;
		trans.set_address(addr);

		this->lock(tag);

		unsigned int retVal = initiatorSocket[portId]->transport_dbg(trans);

//		ofstream outFile("bus.txt", ios::app);
//		outFile << /*sc_time_stamp()*/tag << " " << trans.get_address() << " " << /*trans.get_data_length() << " " <<*/ trans.is_write();
//		if (trans.get_data_length() == 4) outFile << " " << (unsigned int) (*trans.get_data_ptr()) << endl;
//		else outFile << endl;

		this->unlock();

		return retVal;
	}
		
};

#endif /* SIMPLEBUS_HPP */
