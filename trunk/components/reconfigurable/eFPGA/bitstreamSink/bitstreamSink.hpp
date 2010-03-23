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

/***************************************************************************\
 *   Bitstream Sink, destination of the configuration bitstream, based on
 *   simpleMemoryLT
\***************************************************************************/

#ifndef BITSTREAMSINK_HPP
#define BITSTREAMSINK_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include <utils.hpp>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename BUSWIDTH> class bitstreamSink: public sc_module {
private:
	sc_event writeFree;
	bool busy;

public:
	multi_passthrough_target_socket<bitstreamSink, sizeof(BUSWIDTH)*8> targetSocket;

	bitstreamSink(sc_module_name module_name) : sc_module(module_name),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()) {
		this->targetSocket.register_b_transport(this, &bitstreamSink::b_transport);
//		this->targetSocket.register_get_direct_mem_ptr(this, &bitstreamSink::get_direct_mem_ptr);
//		this->targetSocket.register_transport_dbg(this, &bitstreamSink::transport_dbg);

		this->busy = false;
		end_module();
	}

	~bitstreamSink(){}

	void b_transport(int tag, tlm_generic_payload& trans, sc_time& delay){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();
		unsigned char*   byt = trans.get_byte_enable_ptr();
		unsigned int     wid = trans.get_streaming_width();

		unsigned int block_size = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) block_size++;

		while( this->busy) {
			wait( this->writeFree );
		}
		this->busy = true;

		if (trans.is_write()){
//			cerr << "Sink: Received " << block_size << " words!" << endl;
			trans.set_response_status(TLM_OK_RESPONSE);
		}
		else {
			cerr << "Sink: Unknown request type!" << endl;
			trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
		}

		this->busy = false;
		this->writeFree.notify();
	}

};

#endif /*BITSTREAMSINK_H_ */
