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

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <list>

#include "utils.hpp"
#include "protocolUtils.hpp"

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename BUSWIDTH> class DirectoryLT: public sc_module {

private:
	bool busy;
	map<unsigned int, sc_event* > events;
	queue<unsigned int> requests;

	// Maintains a mapping between each cache block (represented by its base address) and the current privilege assigned to it
	map<sc_dt::uint64,privilegeType> blockPrivilege;
	// Maintains a mapping between each cache block (represented by its base address) and the list of caches containing it (represented by their TLM object tag)
	map<sc_dt::uint64,list<int> > blockCachedIn;

	void lock(int tag) {
		// If there is more than one master component, requests have to be queued: there is an array of events, one
		// for each master component connected: the master component waits on this element (if the master is busy);
		// when it becomes free it goes on (and sets the bus to busy): at the end of its requests, it notifies the
		// next element in queue.
		if( this->busy ) {
			#ifdef DEBUGMODE
			cerr << sc_time_stamp() << ": " << name() << " - Queuing request " << tag << endl;
			#endif
			this->requests.push(tag);
			wait(*(this->events[tag]));
		}
		this->busy = true;
	}

	void unlock(int tag) {
		//Now we check if there are some elements which need to be awakened
		this->busy = false;
		if( !requests.empty() ) {
			unsigned int front = this->requests.front();
			#ifdef DEBUGMODE
			cerr << sc_time_stamp() << ": " << name() << " - Waking up queued request " << front << endl;
			#endif
			this->requests.pop();
			(this->events[front])->notify();
			this->busy = true;
		}
	}

public:
	multi_passthrough_target_socket<DirectoryLT, sizeof(BUSWIDTH)*8> targetSocket;
	multi_passthrough_initiator_socket<DirectoryLT, sizeof(BUSWIDTH)*8> initSocket;

	DirectoryLT (sc_module_name module_name, unsigned int numCaches): sc_module(module_name),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()),
			initSocket((boost::lexical_cast<std::string>(module_name) + "_initSock").c_str()) {
		this->targetSocket.register_b_transport(this, &DirectoryLT::b_transport);
		this->busy = false;
		for(int i = 0; i < numCaches; i++){
			events[i] = new sc_event;
		}
		end_module();
	}
	~DirectoryLT() {}

	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay) {

		sc_dt::uint64 adr = trans.get_address();
		tlm_command cmd = trans.get_command();
		tlm_generic_payload message;
		typename list<int>::iterator cacheListIter, tmpIter;

		delay = SC_ZERO_TIME;

		if (cmd != REMOVE) {this->lock(tag);}
//		this->printDir();
		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - ";
		#endif

		if ( blockPrivilege.find(adr) == blockPrivilege.end() ) {
			// The requested block is currently not loaded in any cache
			#ifdef DEBUGMODE
			cerr << "Block at address " << adr << " not loaded:";
			#endif
			if (cmd == TLM_READ_COMMAND) {
				#ifdef DEBUGMODE
				cerr << " adding SHARED privilege for Cache #" << tag << endl;
				#endif
				blockPrivilege[adr]=SHARED;
			}
			else if (cmd == TLM_WRITE_COMMAND) {
				#ifdef DEBUGMODE
				cerr << " adding EXCLUSIVE privilege for Cache #" << tag << endl;
				#endif
				blockPrivilege[adr]=EXCLUSIVE;
			}
			else if (cmd == REMOVE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Trying to remove a block that is not loaded in any cache...");
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			blockCachedIn[adr].push_back(tag);
			trans.set_response_status(TLM_OK_RESPONSE);
			this->unlock(tag);
			return;
		}
		else {
			if ( blockPrivilege[adr] == SHARED ) {
				// The block is currently loaded with SHARED privilege
				if (cmd == TLM_READ_COMMAND) {
					// For read accesses, we simply add another entry to the list of caches with SHARED privilege
					// THIS HAPPENS ONLY IF THE REQUESTING CACHE ISN'T ALREADY IN THAT LIST!
					bool alreadyIn = false;
					for (cacheListIter = blockCachedIn[adr].begin(); cacheListIter != blockCachedIn[adr].end(); cacheListIter++) {
						if (*cacheListIter == tag) alreadyIn = true;
					}
					if (!alreadyIn) {
						blockCachedIn[adr].push_back(tag);
						#ifdef DEBUGMODE
						cerr << "Adding Cache #" << tag << " with SHARED privilege for address " << adr << endl;
						#endif
					}
					else {
						#ifdef DEBUGMODE
						cerr << adr << " already SHARED for Cache #" << tag << endl;
						#endif
					}
					trans.set_response_status(TLM_OK_RESPONSE);
					this->unlock(tag);
					return;
				}
				else if (cmd == TLM_WRITE_COMMAND) {
					// For write accesses, we have to remove the SHARED block for all the caches in the list, except for the requesting cache
					// Furthermore, we have to add an EXCLUSIVE privilege to the requesting cache
					#ifdef DEBUGMODE
					cerr << "Converting address " << adr << " to an EXCLUSIVE entry for Cache #" << tag << endl;
					#endif
					for (cacheListIter = blockCachedIn[adr].begin(); cacheListIter != blockCachedIn[adr].end(); cacheListIter++) {
						if (*cacheListIter != tag) {
							message.set_command(INVALIDATE);
							message.set_address(adr);
							message.set_response_status(TLM_INCOMPLETE_RESPONSE);
							this->initSocket[*cacheListIter]->b_transport(message,delay);
							if (message.get_response_status() != TLM_OK_RESPONSE)
								THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while INVALIDATING block at address " << adr << " with TAG " << tag);
						}
					}
					blockPrivilege[adr]=EXCLUSIVE;
					blockCachedIn[adr].clear();
					blockCachedIn[adr].push_back(tag);
					trans.set_response_status(TLM_OK_RESPONSE);
					this->unlock(tag);
					return;
				}
				else if (cmd == REMOVE) {
					// Whenever a block it's removed from a cache, its SHARED entry should be removed from the Directory
					#ifdef DEBUGMODE
					cerr << "Removing SHARED entry of Cache #" << tag << " for block at address " << adr << endl;
					#endif
					for (cacheListIter = blockCachedIn[adr].begin(); cacheListIter != blockCachedIn[adr].end(); ) {
						if (*cacheListIter == tag) {
							tmpIter = cacheListIter;
							cacheListIter++;
							blockCachedIn[adr].erase(tmpIter);
						}
						else {cacheListIter++;}
					}
					// Furthermore, if the list of SHARED accesses is empty, we shall remove the privilege mapping for the current block
					if ( blockCachedIn[adr].empty() ) {
						typename map<sc_dt::uint64,privilegeType>::iterator bPiter = blockPrivilege.find(adr);
						if (bPiter == blockPrivilege.end())
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No assigned privileges for the removed block");
						else {blockPrivilege.erase(bPiter);}
					}
					trans.set_response_status(TLM_OK_RESPONSE);
					return;
				}
				else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			}
			if ( blockPrivilege[adr] == EXCLUSIVE ) {
				// The block is currently loaded with EXCLUSIVE privilege
				if (blockCachedIn[adr].size() != 1)
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": A block with EXCLUSIVE privileges is mapped to more than one cache...");
				if (cmd == TLM_READ_COMMAND) {
					// For read accesses, we have to flush the content of the cache with EXLUCSIVE privilege and to perform a privilege downgrade
					// THIS HAPPENS ONLY IF THE REQUESTING CACHE IS DIFFERENT THAN THE ONE WITH EXLUSIVE PRIVILEGE
					if (tag != blockCachedIn[adr].front() ) {
						#ifdef DEBUGMODE
						cerr << "Downgrading Cache #" << blockCachedIn[adr].front() << " privilege to SHARED for block at address " << adr << endl;
						#endif
						blockPrivilege[adr]=SHARED;
						message.set_command(FLUSH);
						message.set_address(adr);
						message.set_response_status(TLM_INCOMPLETE_RESPONSE);
						int cacheNum = blockCachedIn[adr].front();
						this->initSocket[cacheNum]->b_transport(message,delay);
						if (message.get_response_status() != TLM_OK_RESPONSE)
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while FLUSHING block at address " << adr << " with TAG " << tag);
						#ifdef DEBUGMODE
						cerr << "Adding Cache #" << tag << " with SHARED privilege for address " << adr << endl;
						#endif
						blockCachedIn[adr].push_back(tag);
					}
					else {
						#ifdef DEBUGMODE
						cerr << "EXCLUSIVE privilege already assigned to Cache #" << tag << " for address " << adr << endl;
						#endif
					}
					trans.set_response_status(TLM_OK_RESPONSE);
					this->unlock(tag);
					return;
				}
				else if (cmd == TLM_WRITE_COMMAND) {
					// For write accesses, we have to remove the assigned EXCLUSIVE privilege, and reassign it to the cache performing the request
					// THIS HAPPENS ONLY IF THE REQUESTING CACHE IS DIFFERENT THAN THE ONE WITH EXLUSIVE PRIVILEGE
					if (tag != blockCachedIn[adr].front() ) {
						#ifdef DEBUGMODE
						cerr << "Re-assigning to Cache #" << tag << " the EXCLUSIVE privilege for address " << adr << endl;
						#endif
						blockPrivilege[adr]=EXCLUSIVE;
						message.set_command(INVALIDATE);
						message.set_address(adr);
						message.set_response_status(TLM_INCOMPLETE_RESPONSE);
						int cacheNum = blockCachedIn[adr].front();
						this->initSocket[cacheNum]->b_transport(message,delay);
						if (message.get_response_status() != TLM_OK_RESPONSE)
							THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while INVALIDATING block at address " << adr << " with TAG " << tag);
						blockCachedIn[adr].clear();
						blockCachedIn[adr].push_back(tag);
					}
					else {
						#ifdef DEBUGMODE
						cerr << "EXCLUSIVE privilege already assigned to Cache #" << tag << " for address " << adr << endl;
						#endif
					}
					trans.set_response_status(TLM_OK_RESPONSE);
					this->unlock(tag);
					return;
				}
				else if (cmd == REMOVE) {
					// Whenever a block it's removed from a cache, its EXCLUSIVE entry should be removed from the Directory
					#ifdef DEBUGMODE
					cerr << "Removing EXCLUSIVE entry of Cache #" << tag << " for block at address " << adr << endl;
					#endif
					blockCachedIn[adr].clear();
					typename map<sc_dt::uint64,privilegeType>::iterator bPiter = blockPrivilege.find(adr);
					if (bPiter == blockPrivilege.end())
						THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": No assigned privileges for the removed block");
					else {
						blockPrivilege.erase(bPiter);
						trans.set_response_status(TLM_OK_RESPONSE);
					}
					return;
				}
				else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			}
		}
		trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		cerr << "This branch should never be executed, something's gone wrong..." << endl;
	}

	void printDir() {
		typename map<sc_dt::uint64,privilegeType>::iterator blockIterator;
		typename list<int>::iterator cacheListIter;
		cout << "*****  DIRECTORY  *****" << endl;
		for (blockIterator = blockPrivilege.begin(); blockIterator != blockPrivilege.end(); blockIterator++) {
			cout << blockIterator->first << ": ";
			if (blockIterator->second == SHARED) cout << "SHARED" << endl << "\t";
			else if (blockIterator->second == EXCLUSIVE) cout << "EXCLUSIVE" << endl << "\t";
			else cout << "??" << endl << "\t";
			for (cacheListIter = blockCachedIn[blockIterator->first].begin(); cacheListIter != blockCachedIn[blockIterator->first].end(); cacheListIter++) {
				cout << " " << *cacheListIter;
			}
			cout << endl;
		}
		cout << "***********************" << endl;
	}

};
