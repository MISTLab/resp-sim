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
\***************************************************************************/


#ifndef MEMORYLT_HPP
#define MEMORYLT_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include <utils.hpp>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename BUSWIDTH> class MemoryLT: public sc_module {
private:
	const sc_time latency;
	unsigned int size;
	unsigned char * mem;

public:
	multi_passthrough_target_socket<MemoryLT, sizeof(BUSWIDTH)*8> targetSocket;

	MemoryLT(sc_module_name module_name, unsigned int size, sc_time latency = SC_ZERO_TIME) :
			sc_module(module_name), size(size), latency(latency),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()) {
		this->targetSocket.register_b_transport(this, &MemoryLT::b_transport);
		this->targetSocket.register_get_direct_mem_ptr(this, &MemoryLT::get_direct_mem_ptr);
		this->targetSocket.register_transport_dbg(this, &MemoryLT::transport_dbg);

		// Reset memory
		this->mem = new unsigned char[this->size];
		memset(this->mem, 0, size);
		end_module();
	}

	~MemoryLT(){
		delete this->mem;
	}

	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();
		unsigned char*   byt = trans.get_byte_enable_ptr();
		unsigned int     wid = trans.get_streaming_width();

		if(adr > this->size){
			trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
			cerr << "Error requesting address " << showbase << hex << adr << dec << endl;
			return;
		}
		if(byt != 0){
			trans.set_response_status(TLM_BYTE_ENABLE_ERROR_RESPONSE);
			return;
		}

		if(trans.get_command() == TLM_READ_COMMAND){
			memcpy(ptr, &mem[adr], len);
		}
		else if(cmd == TLM_WRITE_COMMAND){
			memcpy(&mem[adr], ptr, len);
		}

		// Use temporal decoupling: add memory latency to delay argument
		unsigned int words = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) words++;
//		delay += words*this->latency;
		wait(words*this->latency);

		trans.set_dmi_allowed(true);
		trans.set_response_status(TLM_OK_RESPONSE);
	}

	// TLM-2 DMI method
	bool get_direct_mem_ptr(int tag, tlm_generic_payload& trans, tlm_dmi& dmi_data){
		// Permit read and write access
		dmi_data.allow_read_write();
//		cerr << "Going DMI" << endl;

		// Set other details of DMI region
		dmi_data.set_dmi_ptr(this->mem);
		dmi_data.set_start_address(0);
		dmi_data.set_end_address(this->size);
		dmi_data.set_read_latency(this->latency);
		dmi_data.set_write_latency(this->latency);

		return true;
	}

	// TLM-2 debug transaction method
	unsigned int transport_dbg(int tag, tlm_generic_payload& trans){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();

		// Calculate the number of bytes to be actually copied
		unsigned int num_bytes = (len < this->size - adr) ? len : this->size - adr;

		if(cmd == TLM_READ_COMMAND)
			memcpy(ptr, &mem[adr], num_bytes);
		else if(cmd == TLM_WRITE_COMMAND)
			memcpy(&mem[adr], ptr, num_bytes);

		return num_bytes;
	}

	//Method used to directly write a word into memory; it is mainly used to load the
	//application program into memory
	void write_byte_dbg(const unsigned int & address, const unsigned char & datum){
		if(address >= this->size){
			THROW_EXCEPTION("Address " << hex << showbase << address << " out of memory");
		}
		this->mem[address] = datum;
	}
};

#endif
