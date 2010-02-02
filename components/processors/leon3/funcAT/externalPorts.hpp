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


#ifndef EXTERNALPORTS_HPP
#define EXTERNALPORTS_HPP

#include <memory.hpp>
#include <systemc.h>
#include <ToolsIf.hpp>
#include <tlm.h>
#include <trap_utils.hpp>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#define FUNC_MODEL
#define AT_IF
namespace leon3_funcat_trap{

    class TLMMemory : public MemoryInterface, public sc_module{
        private:
        MemoryToolsIf< unsigned int > * debugger;
        tlm::tlm_generic_payload * request_in_progress;
        sc_event end_request_event;
        sc_event end_response_event;
        tlm_utils::peq_with_cb_and_phase< TLMMemory > m_peq;

        public:
        TLMMemory( sc_module_name portName );
        void setDebugger( MemoryToolsIf< unsigned int > * debugger );
        inline tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload & trans, tlm::tlm_phase \
            & phase, sc_time & delay ) throw(){
            // TLM-2 backward non-blocking transport method
            // The timing annotation must be honored
            m_peq.notify(trans, phase, delay);
            return tlm::TLM_ACCEPTED;
        }
        void peq_cb( tlm::tlm_generic_payload & trans, const tlm::tlm_phase & phase );
        sc_dt::uint64 read_dword( const unsigned int & address ) throw();
        inline unsigned int read_word( const unsigned int & address ) throw(){
            unsigned int datum = 0;
            tlm::tlm_generic_payload trans;
            trans.set_address(address);
            trans.set_read();
            trans.set_data_ptr(reinterpret_cast<unsigned char*>(&datum));
            trans.set_data_length(sizeof(datum));
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

            if(this->request_in_progress != NULL){
                wait(this->end_request_event);
            }
            request_in_progress = &trans;

            // Non-blocking transport call on the forward path
            sc_time delay = SC_ZERO_TIME;
            tlm::tlm_phase phase = tlm::BEGIN_REQ;
            tlm::tlm_sync_enum status;
            status = initSocket->nb_transport_fw(trans, phase, delay);

            if(trans.is_response_error()){
                std::string errorStr("Error from nb_transport_fw, response status = " + trans.get_response_string());
                SC_REPORT_ERROR("TLM-2", errorStr.c_str());
            }

            // Check value returned from nb_transport_fw
            if(status == tlm::TLM_UPDATED){
                // The timing annotation must be honored
                m_peq.notify(trans, phase, delay);
                wait(this->end_response_event);
            }
            else if(status == tlm::TLM_COMPLETED){
                // The completion of the transaction necessarily ends the BEGIN_REQ phase
                this->request_in_progress = NULL;
                // The target has terminated the transaction, I check the correctness
                if(trans.is_response_error()){
                    SC_REPORT_ERROR("TLM-2", ("Transaction returned with error, response status = " + \
                        trans.get_response_string()).c_str());
                }
            }
            wait(this->end_response_event);
            //Now the code for endianess conversion: the processor is always modeled
            //with the host endianess; in case they are different, the endianess
            //is turned
            #ifdef LITTLE_ENDIAN_BO
            this->swapEndianess(datum);
            #endif

            return datum;
        }
        unsigned short int read_half( const unsigned int & address ) throw();
        unsigned char read_byte( const unsigned int & address ) throw();
        void write_dword( const unsigned int & address, sc_dt::uint64 datum ) throw();
        inline void write_word( const unsigned int & address, unsigned int datum ) throw(){
            //Now the code for endianess conversion: the processor is always modeled
            //with the host endianess; in case they are different, the endianess
            //is turned
            #ifdef LITTLE_ENDIAN_BO
            this->swapEndianess(datum);
            #endif
            if(this->debugger != NULL){
                this->debugger->notifyAddress(address, sizeof(datum));
            }
            tlm::tlm_generic_payload trans;
            trans.set_address(address);
            trans.set_write();
            trans.set_data_ptr((unsigned char*)&datum);
            trans.set_data_length(sizeof(datum));
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

            if(this->request_in_progress != NULL){
                wait(this->end_request_event);
            }
            request_in_progress = &trans;

            // Non-blocking transport call on the forward path
            sc_time delay = SC_ZERO_TIME;
            tlm::tlm_phase phase = tlm::BEGIN_REQ;
            tlm::tlm_sync_enum status;
            status = initSocket->nb_transport_fw(trans, phase, delay);

            if(trans.is_response_error()){
                std::string errorStr("Error from nb_transport_fw, response status = " + trans.get_response_string());
                SC_REPORT_ERROR("TLM-2", errorStr.c_str());
            }

            // Check value returned from nb_transport_fw
            if(status == tlm::TLM_UPDATED){
                // The timing annotation must be honored
                m_peq.notify(trans, phase, delay);
                wait(this->end_response_event);
            }
            else if(status == tlm::TLM_COMPLETED){
                // The completion of the transaction necessarily ends the BEGIN_REQ phase
                this->request_in_progress = NULL;
                // The target has terminated the transaction, I check the correctness
                if(trans.is_response_error()){
                    SC_REPORT_ERROR("TLM-2", ("Transaction returned with error, response status = " + \
                        trans.get_response_string()).c_str());
                }
            }
            wait(this->end_response_event);
        }
        void write_half( const unsigned int & address, unsigned short int datum ) throw();
        void write_byte( const unsigned int & address, unsigned char datum ) throw();
        sc_dt::uint64 read_dword_dbg( const unsigned int & address ) throw();
        unsigned int read_word_dbg( const unsigned int & address ) throw();
        unsigned short int read_half_dbg( const unsigned int & address ) throw();
        unsigned char read_byte_dbg( const unsigned int & address ) throw();
        void write_dword_dbg( const unsigned int & address, sc_dt::uint64 datum ) throw();
        void write_word_dbg( const unsigned int & address, unsigned int datum ) throw();
        void write_half_dbg( const unsigned int & address, unsigned short int datum ) throw();
        void write_byte_dbg( const unsigned int & address, unsigned char datum ) throw();
        void lock();
        void unlock();
        tlm_utils::simple_initiator_socket< TLMMemory, 32 > initSocket;
    };

};



#endif
