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
#include "utils.hpp"

#include "trafficGenerator.hpp"

trafficGenerator::trafficGenerator(sc_module_name name, sc_time latency = SC_ZERO_TIME) : 
      sc_module(name), initSocket((boost::lexical_cast<std::string>(name) + "_port").c_str()), latency(latency), completed(false),
      maxAddress(0), minAddress(0), step(1), size(8), numOfTransissions(1), endMessageAddress(0) { 
    SC_THREAD(action);
    end_module();
}

void trafficGenerator::action(){
    unsigned int currAddress = minAddress;
    unsigned char *datum = new unsigned char[size];
    //unsigned char datum[1024];
    
    if( minAddress + step >= maxAddress)
        THROW_EXCEPTION("The specified parameters are not valid");
        
    bool infiniteLoop = this->numOfTransissions==0?true:false;
    sent = 0;
        
    for(unsigned int i = 0; i < this->numOfTransissions || infiniteLoop; i++){
        tlm::tlm_generic_payload trans;
        sc_time delay;
        trans.set_address(currAddress);
        trans.set_data_ptr(datum);
        trans.set_data_length(size);
        trans.set_read();
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        this->initSocket->b_transport(trans, delay);

        currAddress += step;
        if(currAddress >= maxAddress) currAddress = minAddress;
        sent++;
        wait(latency);
    }
        
    //send end message
    tlm::tlm_generic_payload trans;
    sc_time delay;
    trans.set_address(this->endMessageAddress);
    trans.set_data_ptr(datum);
    trans.set_data_length(1);
    trans.set_write();
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    this->initSocket->b_transport(trans, delay);    
    this->completed = true;
    
    std::cout << name() << " completed!" << std::endl; 
    
    delete [] datum;
}

bool trafficGenerator::isTransmissionCompleted(){
  return this->completed;
}

void trafficGenerator::setParameters(unsigned int numOfTransissions, unsigned int min, unsigned int max, unsigned int step, 
        unsigned int size, unsigned int endMessageAddress){
    //if(size > MAX_SIZE) THROW_EXCEPTION("Max size is equal to " << MAX_SIZE);
    this->numOfTransissions = numOfTransissions;
    this->maxAddress = max;
    this->minAddress = min;
    this->step = step;
    this->size = size;
    this->endMessageAddress = endMessageAddress;
}


