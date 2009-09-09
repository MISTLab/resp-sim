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
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/


#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>

#include "testMaster.hpp"

testMaster::testMaster(sc_module_name name) : sc_module(name), initSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()){
    SC_THREAD(sendChars);
    srand((unsigned)time(0));
    end_module();
}

void testMaster::sendChars(){

    for( int i = 0 ; i < 10 ; i++  ) {
        unsigned char datum = (unsigned char)(rand()%128);
        tlm::tlm_generic_payload trans;
        sc_time delay;
        trans.set_address(0);
        trans.set_write();
        trans.set_data_ptr(&datum);
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::cerr << "testMaster: Error response from the TLM port" << std::endl;
        }
    }
}
