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



#include <externalPorts.hpp>
#include <memory.hpp>
#include <systemc.h>
#include <ToolsIf.hpp>
#include <tlm.h>
#include <trap_utils.hpp>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

using namespace leon3_funcat_trap;
void leon3_funcat_trap::TLMMemory::setDebugger( MemoryToolsIf< unsigned int > * debugger \
    ){
    this->debugger = debugger;
}

void leon3_funcat_trap::TLMMemory::peq_cb( tlm::tlm_generic_payload & trans, const \
    tlm::tlm_phase & phase ){
    // Payload event queue callback to handle transactions from target
    // Transaction could have arrived through return path or backward path
    if (phase == tlm::END_REQ || (&trans == request_in_progress && phase == tlm::BEGIN_RESP)){
        // The end of the BEGIN_REQ phase
        request_in_progress = NULL;
        end_request_event.notify();
    }
    else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP){
        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
    }

    if (phase == tlm::BEGIN_RESP){
        if (trans.is_response_error()){
            SC_REPORT_ERROR("TLM-2", ("Transaction returned with error, response status = " + \
                trans.get_response_string()).c_str());
        }

        // Send final phase transition to target
        tlm::tlm_phase fw_phase = tlm::END_RESP;
        sc_time delay = SC_ZERO_TIME;
        initSocket->nb_transport_fw(trans, fw_phase, delay);
        if (trans.is_response_error()){
            SC_REPORT_ERROR("TLM-2", ("Transaction returned with error, response status = " + \
                                        trans.get_response_string()).c_str());
        }
        this->end_response_event.notify(delay);
    }
}

sc_dt::uint64 leon3_funcat_trap::TLMMemory::read_dword( const unsigned int & address \
    ) throw(){
    sc_dt::uint64 datum = 0;
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
    #ifdef LITTLE_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif

    return datum;
}

unsigned short int leon3_funcat_trap::TLMMemory::read_half( const unsigned int & \
    address ) throw(){
    unsigned short int datum = 0;
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

unsigned char leon3_funcat_trap::TLMMemory::read_byte( const unsigned int & address \
    ) throw(){
    unsigned char datum = 0;
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

    return datum;
}

void leon3_funcat_trap::TLMMemory::write_dword( const unsigned int & address, sc_dt::uint64 \
    datum ) throw(){
    #ifdef LITTLE_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
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

void leon3_funcat_trap::TLMMemory::write_half( const unsigned int & address, unsigned \
    short int datum ) throw(){
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

void leon3_funcat_trap::TLMMemory::write_byte( const unsigned int & address, unsigned \
    char datum ) throw(){
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

sc_dt::uint64 leon3_funcat_trap::TLMMemory::read_dword_dbg( const unsigned int & \
    address ) throw(){
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(8);
    sc_dt::uint64 datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    #ifdef LITTLE_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif
    return datum;
}

unsigned int leon3_funcat_trap::TLMMemory::read_word_dbg( const unsigned int & address \
    ) throw(){
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(4);
    unsigned int datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef LITTLE_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    return datum;
}

unsigned short int leon3_funcat_trap::TLMMemory::read_half_dbg( const unsigned int \
    & address ) throw(){
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(2);
    unsigned short int datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef LITTLE_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    return datum;
}

unsigned char leon3_funcat_trap::TLMMemory::read_byte_dbg( const unsigned int & address \
    ) throw(){
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(1);
    unsigned char datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    return datum;
}

void leon3_funcat_trap::TLMMemory::write_dword_dbg( const unsigned int & address, \
    sc_dt::uint64 datum ) throw(){
    #ifdef LITTLE_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(8);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void leon3_funcat_trap::TLMMemory::write_word_dbg( const unsigned int & address, \
    unsigned int datum ) throw(){
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef LITTLE_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(4);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void leon3_funcat_trap::TLMMemory::write_half_dbg( const unsigned int & address, \
    unsigned short int datum ) throw(){
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef LITTLE_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(2);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void leon3_funcat_trap::TLMMemory::write_byte_dbg( const unsigned int & address, \
    unsigned char datum ) throw(){
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(1);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void leon3_funcat_trap::TLMMemory::lock(){

}

void leon3_funcat_trap::TLMMemory::unlock(){

}

leon3_funcat_trap::TLMMemory::TLMMemory( sc_module_name portName ) : sc_module(portName), \
    m_peq(this, &TLMMemory::peq_cb), request_in_progress(NULL){
    this->debugger = NULL;
    // Register callbacks for incoming interface method calls
    this->initSocket.register_nb_transport_bw(this, &TLMMemory::nb_transport_bw);
    end_module();
}


