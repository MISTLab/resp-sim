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
 *   Component developed by: Antonio Miele miele@elet.polimi.it
 *
\***************************************************************************/

#ifndef DUMMYBUFFERLT_HPP
#define DUMMYBUFFERLT_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

#include "utils.hpp"

using namespace std;
using namespace tlm;
using namespace tlm_utils;

template<typename BUSWIDTH> class DummyBufferLT: public sc_module {
private:
  const sc_time latency;
  int* regs;
  unsigned int status;
  unsigned int size;
  
public:

  simple_target_socket<DummyBufferLT, sizeof(BUSWIDTH)*8> targetSocket;

  DummyBufferLT(sc_module_name module_name, unsigned int size, sc_time latency = SC_ZERO_TIME) :
      sc_module(module_name), size(size), latency(latency),
      targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()) {
    this->targetSocket.register_b_transport(this, &DummyBufferLT::b_transport);

    this->regs = new int[this->size];
    memset(this->regs, 0, size);
    this->status = 0;
    end_module();
  }

  ~DummyBufferLT(){
    delete this->regs;
  }
  
  unsigned int getStatus(){
    return this->status;
  }

  unsigned int getDatum(unsigned int index){
    if(index > size)
        THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": The buffer contains only  " << size << " valid positions" << endl);
    return this->regs[1+index];
  }
  
  void b_transport(tlm_generic_payload& trans, sc_time& delay){
    tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();

    // Checking consistency of the request
    if(adr/sizeof(BUSWIDTH) > this->size+1 || (adr + len)/sizeof(BUSWIDTH) > this->size+1) {
      trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
      THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error requesting address " << /*showbase << hex <<*/ adr << /*dec <<*/ " on " << name() << endl);
      return;
    }

    if(cmd == TLM_READ_COMMAND){
      if(adr == 0)
        memcpy(ptr,&status, sizeof(BUSWIDTH));
      else
        memcpy(ptr,&regs[adr/sizeof(BUSWIDTH)], sizeof(BUSWIDTH));
    }
    else if(cmd == TLM_WRITE_COMMAND){
      if(adr == 0){
        memcpy(&status, ptr, sizeof(BUSWIDTH));
        if (status != 0x0)
            sc_stop();
      }
      else{
        memcpy(&regs[adr/sizeof(BUSWIDTH)], ptr, sizeof(BUSWIDTH));
      }
    }

    wait(this->latency);

    trans.set_dmi_allowed(true);
    trans.set_response_status(TLM_OK_RESPONSE);
  }
};

#endif
