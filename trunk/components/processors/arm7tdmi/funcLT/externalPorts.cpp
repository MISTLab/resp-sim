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
#include <trap_utils.hpp>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <registers.hpp>

using namespace arm7tdmi_funclt_trap;
void arm7tdmi_funclt_trap::TLMMemory::setDebugger( MemoryToolsIf< unsigned int > \
    * debugger ){
    this->debugger = debugger;
}

sc_dt::uint64 arm7tdmi_funclt_trap::TLMMemory::read_dword( const unsigned int & address \
    ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
    sc_dt::uint64 datum = 0;
    if (this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in reading memory data through DMI: address out of \
                bounds");
        }
        memcpy(&datum, this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() \
            + address, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_read_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_read();
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&datum));
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }
    #ifdef BIG_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif

    return datum;
}

unsigned short int arm7tdmi_funclt_trap::TLMMemory::read_half( const unsigned int \
    & address ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
    unsigned short int datum = 0;
    if (this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in reading memory data through DMI: address out of \
                bounds");
        }
        memcpy(&datum, this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() \
            + address, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_read_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_read();
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&datum));
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif

    return datum;
}

unsigned char arm7tdmi_funclt_trap::TLMMemory::read_byte( const unsigned int & address \
    ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
    unsigned char datum = 0;
    if (this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in reading memory data through DMI: address out of \
                bounds");
        }
        memcpy(&datum, this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() \
            + address, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_read_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_read();
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(&datum));
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }

    return datum;
}

void arm7tdmi_funclt_trap::TLMMemory::write_dword( const unsigned int & address, \
    sc_dt::uint64 datum ) throw(){
    #ifdef BIG_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    if(this->debugger != NULL){
        this->debugger->notifyAddress(address, sizeof(datum));
    }
    if(this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in writing memory data through DMI: address out of \
                bounds");
        }
        memcpy(this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() + address, \
            &datum, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_write_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_write();
        trans.set_data_ptr((unsigned char*)&datum);
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }
}

void arm7tdmi_funclt_trap::TLMMemory::write_half( const unsigned int & address, unsigned \
    short int datum ) throw(){
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    if(this->debugger != NULL){
        this->debugger->notifyAddress(address, sizeof(datum));
    }
    if(this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in writing memory data through DMI: address out of \
                bounds");
        }
        memcpy(this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() + address, \
            &datum, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_write_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_write();
        trans.set_data_ptr((unsigned char*)&datum);
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }
}

void arm7tdmi_funclt_trap::TLMMemory::write_byte( const unsigned int & address, unsigned \
    char datum ) throw(){
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    if(this->debugger != NULL){
        this->debugger->notifyAddress(address, sizeof(datum));
    }
    if(this->dmi_ptr_valid){
        if(address + this->dmi_data.get_start_address() > this->dmi_data.get_end_address()){
            SC_REPORT_ERROR("TLM-2", "Error in writing memory data through DMI: address out of \
                bounds");
        }
        memcpy(this->dmi_data.get_dmi_ptr() - this->dmi_data.get_start_address() + address, \
            &datum, sizeof(datum));
        this->quantKeeper.inc(this->dmi_data.get_write_latency());
    }
    else{
        sc_time delay = this->quantKeeper.get_local_time();
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_write();
        trans.set_data_ptr((unsigned char*)&datum);
        trans.set_data_length(sizeof(datum));
        trans.set_byte_enable_ptr(0);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        this->initSocket->b_transport(trans, delay);

        if(trans.is_response_error()){
            std::string errorStr("Error from b_transport, response status = " + trans.get_response_string());
            SC_REPORT_ERROR("TLM-2", errorStr.c_str());
        }
        if(trans.is_dmi_allowed()){
            this->dmi_data.init();
            this->dmi_ptr_valid = this->initSocket->get_direct_mem_ptr(trans, this->dmi_data);
        }
        //Now lets keep track of time
        this->quantKeeper.set(delay);
    }
}

sc_dt::uint64 arm7tdmi_funclt_trap::TLMMemory::read_dword_dbg( const unsigned int \
    & address ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(8);
    sc_dt::uint64 datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    #ifdef BIG_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif
    return datum;
}

unsigned int arm7tdmi_funclt_trap::TLMMemory::read_word_dbg( const unsigned int & \
    address ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
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
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    return datum;
}

unsigned short int arm7tdmi_funclt_trap::TLMMemory::read_half_dbg( const unsigned \
    int & address ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
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
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    return datum;
}

unsigned char arm7tdmi_funclt_trap::TLMMemory::read_byte_dbg( const unsigned int \
    & address ) throw(){
    if(address == 0xfffffff0L){
        return this->MP_ID;
    }
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_read();
    trans.set_data_length(1);
    unsigned char datum = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&datum));
    this->initSocket->transport_dbg(trans);
    return datum;
}

void arm7tdmi_funclt_trap::TLMMemory::write_dword_dbg( const unsigned int & address, \
    sc_dt::uint64 datum ) throw(){
    #ifdef BIG_ENDIAN_BO
    unsigned int datum1 = (unsigned int)(datum);
    this->swapEndianess(datum1);
    unsigned int datum2 = (unsigned int)(datum >> 32);
    this->swapEndianess(datum2);
    datum = datum1 | (((sc_dt::uint64)datum2) << 32);
    #endif
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(8);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void arm7tdmi_funclt_trap::TLMMemory::write_word_dbg( const unsigned int & address, \
    unsigned int datum ) throw(){
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    } 
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(4);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void arm7tdmi_funclt_trap::TLMMemory::write_half_dbg( const unsigned int & address, \
    unsigned short int datum ) throw(){
    //Now the code for endianess conversion: the processor is always modeled
    //with the host endianess; in case they are different, the endianess
    //is turned
    #ifdef BIG_ENDIAN_BO
    this->swapEndianess(datum);
    #endif
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(2);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void arm7tdmi_funclt_trap::TLMMemory::write_byte_dbg( const unsigned int & address, \
    unsigned char datum ) throw(){
    if(address == 0xfffffff0L){
        this->MP_ID = datum;
        return;
    }
    tlm::tlm_generic_payload trans;
    trans.set_address(address);
    trans.set_write();
    trans.set_data_length(1);
    trans.set_data_ptr((unsigned char *)&datum);
    this->initSocket->transport_dbg(trans);
}

void arm7tdmi_funclt_trap::TLMMemory::lock(){

}

void arm7tdmi_funclt_trap::TLMMemory::unlock(){

}

arm7tdmi_funclt_trap::TLMMemory::TLMMemory( sc_module_name portName, tlm_utils::tlm_quantumkeeper \
    & quantKeeper, Reg32_1 & MP_ID ) : sc_module(portName), quantKeeper(quantKeeper), \
    MP_ID(MP_ID){
    this->debugger = NULL;
    this->dmi_ptr_valid = false;
    end_module();
}


