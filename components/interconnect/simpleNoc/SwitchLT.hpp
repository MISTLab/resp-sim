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

#ifndef SWITCHLT_HPP
#define SWITCHLT_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <queue>
#include <map>

#include "utils.hpp"

using namespace std;
using namespace tlm;
using namespace tlm_utils;

typedef pair<sc_dt::uint64,sc_dt::uint64> addressSet;
typedef map<addressSet, unsigned int> forwardMap_t;
typedef map<unsigned int, unsigned int> targetToPortMap_t;

enum TopologyType {RING, STAR, MESHFC, TREE};

template<typename BUSWIDTH> class SwitchLT: public sc_module {
private:
	// Unique Identifier of the switch. Value 0 is reserved (will be used in the edge map for edges directly connected to slave components)
	unsigned int myId;
	sc_time latency;

	// Private definition of initiator sockets, in order to avoid the use of the standard TLM bind (use initSocketBind instead)
	multi_passthrough_initiator_socket<SwitchLT, sizeof(BUSWIDTH)*8> initSocket;

	// This map is used to store the unique ID of the switch connected to each outgoing edge (initiator port)...
	targetToPortMap_t outgoingEdges;
	// ...while this counter is used to keep trace of the next free slot (initiator port)
	unsigned int connectedEdges;

	// This map specifies, for each set of addresses, the ID of the switch upon which we have to forward the request
	forwardMap_t forwardMap;

	bool busy;
	bool locking;
	map<unsigned int, sc_event* > events;
	queue<unsigned int> requests;
	unsigned int ticket;
	unsigned int maxQueue;

	void lock(int tag) {
		// If there is more than one master component, requests have to be queued: there is an array of events, one
		// for each master component connected: the master component waits on this element (if the master is busy);
		// when it becomes free it goes on (and sets the bus to busy): at the end of its requests, it notifies the
		// next element in queue.
		if( this->busy ) {
//			cerr << sc_time_stamp() << " - noc-switch" << this->myId << ": Queuing request #" << tag << endl;
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
//			cerr << sc_time_stamp() << " - noc-switch" << this->myId << ": Waking up queued request #" << front << endl;
			this->requests.pop();
			(this->events[front])->notify();
			if (locking)
				this->busy = true;
		}
	}

public:
	// Target sockets are exposed, in order to be used by 
	multi_passthrough_target_socket<SwitchLT, sizeof(BUSWIDTH)*8> targetSocket;

	unsigned int numAccesses;
	unsigned int numWords;

	SwitchLT(unsigned int id, sc_time lat = SC_ZERO_TIME, bool isLocking = true, unsigned int mQ = 100): 
			sc_module(("nocSw_" + boost::lexical_cast<std::string>(id)).c_str()), myId(id), latency(lat), locking(isLocking),
			targetSocket(("swTargSock_" + boost::lexical_cast<std::string>(id)).c_str()), maxQueue(mQ), connectedEdges(0),
			initSocket(("swInitSock_" + boost::lexical_cast<std::string>(id)).c_str()) {
		if (id == 0) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": A switch has been initialized with ID 0, which is reserved");
		for(int i = 0; i < maxQueue; i++){
			events[i] = new sc_event;
		}
		busy = false;
		ticket = 0;
		numAccesses = 0;
		numWords = 0;

		this->targetSocket.register_b_transport(this, &SwitchLT::b_transport);
		this->targetSocket.register_transport_dbg(this, &SwitchLT::transport_dbg);
		end_module();
	}

	// Routine replacing the classic TLM port bind: used to assign a unique ID to the outgoing edge (initiator ports are incrementally assigned while calling this function)
	void initSocketBind(typename multi_init_base<sizeof(BUSWIDTH)*8, tlm::tlm_base_protocol_types, 0>::base_target_socket_type& s, unsigned int targetId) {
		this->initSocket.bind(s);
		outgoingEdges[targetId]=connectedEdges;
		connectedEdges++;
	}

	// Routine used to add a routing path, for a given set of addresses, to this switch
	void addRoutingPath(sc_dt::uint64 startAddress, sc_dt::uint64 endAddress, unsigned int forwardId) {
		if (startAddress > endAddress) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Adding a routing path - End address " << endAddress << " smaller than start address " << startAddress);
		forwardMap[addressSet(startAddress,endAddress)] = forwardId;
	}

	// Routine used to remove all the routing paths previously inserted into the switch
	void clearRoutingMap() {
		forwardMap.clear();
	}

	// TLM transport method, selects the appropiate routing path and forwards the requests to another switch (or to the target component)
	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay) {
		unsigned int ticketOld = ticket;
		ticket = (ticket+1)%maxQueue;
		this->lock(ticketOld);

		unsigned int destination = -1, destinationPort = -1;
		tlm_command cmd = trans.get_command();
		unsigned int adr = trans.get_address();
		unsigned int len = trans.get_data_length();
		unsigned int words = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) words++;

//		this->printTables();
		for (forwardMap_t::iterator fMiter = forwardMap.begin(); fMiter != forwardMap.end(); fMiter++) {
			if (adr >= (fMiter->first).first && adr <= (fMiter->first).second) {
				destination = fMiter->second;
				destinationPort = outgoingEdges[destination];
			}
		}
		if (destination == -1) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Destination port for address " << adr << " is not listed in the routing maps");

//		cerr << sc_time_stamp() << " - noc-switch" << this->myId << ": Received " << cmd << " request for address " << adr << endl;
		if (cmd == TLM_READ_COMMAND) {
			// Here we only forward the packet containing the requested address (we assume this packet is only 1 word)
			wait(this->latency);
		}
		else if(cmd == TLM_WRITE_COMMAND){
			// On the other hand, WRITE request already contain the entire data
			wait(words*this->latency);
		}

		this->numAccesses++;
		this->numWords+=words;
		this->unlock();

//		cerr << sc_time_stamp() << " - noc-switch" << this->myId << ": Routing request to " << destination << " on port " << destinationPort << endl;
		this->initSocket[destinationPort]->b_transport(trans, delay);

		if (cmd == TLM_READ_COMMAND) {
			ticketOld = ticket;
			ticket = (ticket+1)%maxQueue;
			this->lock(ticketOld);

			// Finally, we route back the packet containing the data READ from memory
//			cerr << sc_time_stamp() << " - noc-switch" << this->myId << ": Receiving back the response packet" << endl;
			wait(words*this->latency);

			this->unlock();
		}
	}

	bool get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data){
		cout << "DMI not supported for NOC transactions! DMI requested by " << tag << endl;
		return false;
	}

	unsigned int transport_dbg(int tag, tlm::tlm_generic_payload& trans) {
		unsigned int ticketOld = ticket;
		ticket = (ticket+1)%maxQueue;
		this->lock(ticketOld);

		unsigned int destination = -1, destinationPort = -1;
		tlm_command cmd = trans.get_command();
		unsigned int adr = trans.get_address();

//		this->printTables();
		for (forwardMap_t::iterator fMiter = forwardMap.begin(); fMiter != forwardMap.end(); fMiter++) {
			if (adr >= (fMiter->first).first && adr <= (fMiter->first).second) {
				destination = fMiter->second;
				destinationPort = outgoingEdges[destination];
			}
		}
		if (destination == -1) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Destination port for address " << adr << " is not listed in the routing maps");

//		cerr << sc_time_stamp() << " - DBG noc-switch" << this->myId << ": Received " << cmd << " request for address " << adr << endl;

		this->unlock();

//		cerr << sc_time_stamp() << " - DBG noc-switch" << this->myId << ": Routing request to " << destination << " on port " << destinationPort << endl;
		this->initSocket[destinationPort]->transport_dbg(trans);

		if (cmd == TLM_READ_COMMAND) {
			ticketOld = ticket;
			ticket = (ticket+1)%maxQueue;
			this->lock(ticketOld);

			// Finally, we route back the packet containing the data READ from memory
//			cerr << sc_time_stamp() << " - DBG noc-switch" << this->myId << ": Receiving back the response packet" << endl;

			this->unlock();
		}
	}

	void printTables() {
		cerr << sc_time_stamp() << " - noc-switch" << this->myId << " - My edge table is: " << endl;
		for (targetToPortMap_t::iterator oEiter = outgoingEdges.begin(); oEiter != outgoingEdges.end(); oEiter++) {
			cerr << "\t" << oEiter->first << "->" << oEiter->second << endl;
		}
		cerr << sc_time_stamp() << " - noc-switch" << this->myId << " - My routing table is: " << endl;
		for (forwardMap_t::iterator fMiter = forwardMap.begin(); fMiter != forwardMap.end(); fMiter++) {
			cerr << "\t" << (fMiter->first).first << "-" << (fMiter->first).second << "\t";
			cerr << "routed to switch " << fMiter->second << " on port " << outgoingEdges[fMiter->second] << endl;
		}
		cerr << endl;
	}

	void printAccesses() {
		cout << "\tSwitch #" << this->myId << " had " << numAccesses << " accesses, for a total amount of " << numWords << " words" << endl;
	}

};

#endif
