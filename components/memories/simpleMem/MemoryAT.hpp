/***************************************************************************\
 *
 *
 *            ___        ___           ___           ___
 *           /  /\      /  /\         /  /\         /  /\
 *          /  /:/     /  /::\       /  /::\       /  /::\
 *         /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\
 *        /  /:/     /  /:/~/:/    /  /:/~/::\   /  /:/~/:/
 *       /  /::\    /__/:/ /:/___ /__/:/ /:/\:\ /__/:/ /:/
 *      /__/:/\:\   \  \:\/:::::/ \  \:\/:/__\/ \  \:\/:/
 *      \__\/  \:\   \  \::/~~~~   \  \::/       \  \::/
 *           \  \:\   \  \:\        \  \:\        \  \:\
 *            \  \ \   \  \:\        \  \:\        \  \:\
 *             \__\/    \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of TRAP.
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
 *   (c) Luca Fossati, fossati@elet.polimi.it
 *
\***************************************************************************/

#ifndef MEMORYAT_HPP
#define MEMORYAT_HPP

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <cstring>

#include <trap_utils.hpp>

DECLARE_EXTENDED_PHASE(internal_ph);

namespace trap{

template<unsigned int N_INITIATORS, unsigned int sockSize> class MemoryAT: public sc_module{
    public:
    tlm_utils::simple_target_socket_tagged<MemoryAT, sockSize> * socket[N_INITIATORS];

    MemoryAT(sc_module_name name, unsigned int size, sc_time latency = SC_ZERO_TIME) :
                                sc_module(name), size(size), latency(latency), transId(0),
                                transactionInProgress(false), m_peq(this, &MemoryAT::peq_cb){
        for(int i = 0; i < N_INITIATORS; i++){
            this->socket[i] = new tlm_utils::simple_target_socket_tagged<MemoryAT, sockSize>(("mem_socket_" + boost::lexical_cast<std::string>(i)).c_str());
            this->socket[i]->register_nb_transport_fw(this, &MemoryAT::nb_transport_fw, i);
            this->socket[i]->register_transport_dbg(this, &MemoryAT::transport_dbg, i);
        }

        // Reset memory
        this->mem = new unsigned char[size];
        memset(this->mem, 0, size);
        end_module();
    }

    // TLM-2 non-blocking transport method
    tlm::tlm_sync_enum nb_transport_fw(int tag, tlm::tlm_generic_payload& trans,
                                                tlm::tlm_phase& phase, sc_time& delay){
        sc_dt::uint64    adr = trans.get_address();
        unsigned int     len = trans.get_data_length();
        unsigned char*   byt = trans.get_byte_enable_ptr();
        unsigned int     wid = trans.get_streaming_width();

        // Obliged to check the transaction attributes for unsupported features
        // and to generate the appropriate error response
        if (byt != 0){
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            return tlm::TLM_COMPLETED;
        }
        if(adr > this->size){
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            std::cerr << "Error requesting address " << std::showbase << std::hex << adr << std::dec << std::endl;
            return tlm::TLM_COMPLETED;
        }
        // Now queue the transaction until the annotated time has elapsed
        if(phase == tlm::BEGIN_REQ){
            while(this->transactionInProgress){
                //std::cerr << "waiting for transactionInProgress" << std::endl;
                wait(this->transactionCompleted);
            }
            //std::cerr << "there are no transactionInProgress" << std::endl;
            this->transactionInProgress = true;
        }
        this->transId = tag;
        m_peq.notify(trans, phase, delay);
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        return tlm::TLM_ACCEPTED;
    }

    void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase){
        tlm::tlm_sync_enum status;
        sc_time delay;

        switch (phase){
            case tlm::BEGIN_REQ:
                status = send_end_req(trans);
            break;

            case tlm::END_RESP:
                //std::cerr << "tlm::END_RESP in memory peq_cb" << std::endl;
                this->transactionInProgress = false;
                this->transactionCompleted.notify();
            break;

            case tlm::END_REQ:
            case tlm::BEGIN_RESP:
                SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by target");
            break;

            default:
                if (phase == internal_ph){
                    // Execute the read or write commands
                    tlm::tlm_command cmd = trans.get_command();
                    sc_dt::uint64    adr = trans.get_address();
                    unsigned char*   ptr = trans.get_data_ptr();
                    unsigned int     len = trans.get_data_length();

                    if (cmd == tlm::TLM_READ_COMMAND){
                        memcpy(ptr, &mem[adr], len);
                    }
                    else if(cmd == tlm::TLM_WRITE_COMMAND){
                        memcpy(&mem[adr], ptr, len);
                    }

                    trans.set_response_status(tlm::TLM_OK_RESPONSE);

                    // Target must honor BEGIN_RESP/END_RESP exclusion rule
                    // i.e. must not send BEGIN_RESP until receiving previous END_RESP or BEGIN_REQ
                    send_response(trans);
                    //std::cerr << "Memory reading address in memory " << std::hex << std::showbase << adr << std::endl;
                }
            break;
        }
    }

    tlm::tlm_sync_enum send_end_req(tlm::tlm_generic_payload& trans){
        tlm::tlm_sync_enum status;
        tlm::tlm_phase bw_phase;
        tlm::tlm_phase int_phase = internal_ph;

        // Queue the acceptance and the response with the appropriate latency
        bw_phase = tlm::END_REQ;
        sc_time zeroDelay = SC_ZERO_TIME;
        status = (*(this->socket[transId]))->nb_transport_bw(trans, bw_phase, zeroDelay);
        if (status == tlm::TLM_COMPLETED){
            // Transaction aborted by the initiator
            // (TLM_UPDATED cannot occur at this point in the base protocol, so need not be checked)
            trans.release();
            return status;
        }
        // Queue internal event to mark beginning of response
        m_peq.notify( trans, int_phase, this->latency );
        return status;
    }

    void send_response(tlm::tlm_generic_payload& trans){
        tlm::tlm_sync_enum status;
        tlm::tlm_phase bw_phase;

        bw_phase = tlm::BEGIN_RESP;
        sc_time zeroDelay = SC_ZERO_TIME;
        status = (*(this->socket[transId]))->nb_transport_bw(trans, bw_phase, zeroDelay);

        //std::cerr << "response status " << status << std::endl;
        if (status == tlm::TLM_UPDATED){
            // The timing annotation must be honored
            m_peq.notify(trans, bw_phase, SC_ZERO_TIME);
        }
        else if (status == tlm::TLM_COMPLETED){
            // The initiator has terminated the transaction
            trans.release();
        }
    }

    // TLM-2 debug transaction method
    unsigned int transport_dbg(int tag, tlm::tlm_generic_payload& trans){
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address();
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();

        // Calculate the number of bytes to be actually copied
        unsigned int num_bytes = (len < this->size - adr) ? len : this->size - adr;

        if(cmd == tlm::TLM_READ_COMMAND){
            memcpy(ptr, &mem[adr], num_bytes);
        }
        else if(cmd == tlm::TLM_WRITE_COMMAND){
            memcpy(&mem[adr], ptr, num_bytes);
        }

        return num_bytes;
    }

    //Method used to directly write a word into memory; it is mainly used to load the
    //application program into memory
    void write_byte_dbg(const unsigned int & address, const unsigned char & datum){
        if(address >= this->size){
            THROW_ERROR("Address " << std::hex << std::showbase << address << " out of memory");
        }
        this->mem[address] = datum;
    }

    private:
    const sc_time latency;
    unsigned int size;
    unsigned char * mem;
    int   transId;
    bool  transactionInProgress;
    sc_event transactionCompleted;
    tlm_utils::peq_with_cb_and_phase<MemoryAT> m_peq;
};

};

#endif
