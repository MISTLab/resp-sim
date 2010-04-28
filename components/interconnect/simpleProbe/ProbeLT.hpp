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
 *   (c) Antonio Miele
 *       antoniorosmiele@gmail.com
 *
\***************************************************************************/

#ifndef SIMPLEPROBE_HPP
#define SIMPLEPROBE_HPP

#include <string>
#include <queue>
#include <iostream>

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include "utils.hpp"

using namespace tlm;


/*------------------------------------------------------------------------
 * Documentation on ProbeLT: it is a simple probe to be inserted between a
 * master module and a slave one. It receives requests from the master 
 * and forwards them to the connected slave. In the meanwhile it records
 * the last transmitted payload both in the forward transmission and in 
 * backward one
 *------------------------------------------------------------------------*/
template<typename BUSDATATYPE> class ProbeLT: public sc_module {

  //they are necessary since the payload contains only a pointer. Thus, it is necessary to
  //assign to the pointer an actual position where to save transmitted data
  BUSDATATYPE* last_request_data;
  BUSDATATYPE* last_response_data;

public:
  tlm_utils::simple_target_socket<ProbeLT, sizeof(BUSDATATYPE)*8>  targetSocket;
  tlm_utils::simple_initiator_socket<ProbeLT, sizeof(BUSDATATYPE)*8> initiatorSocket;

  tlm_generic_payload last_request;
  tlm_generic_payload last_response;

  ProbeLT(sc_module_name module_name) : 
            initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_initSocket").c_str()),
            targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSocket").c_str()){
    this->targetSocket.register_b_transport(this, &ProbeLT::b_transport);
    this->targetSocket.register_transport_dbg(this, &ProbeLT::transport_dbg);
    
    last_request_data = NULL;
    this->last_request.set_data_length(0);
    this->last_request.set_data_ptr(reinterpret_cast<unsigned char*>(last_request_data));
    last_response_data = NULL;
    this->last_request.set_data_length(0);
    this->last_response.set_data_ptr(reinterpret_cast<unsigned char*>(last_response_data));
    
    end_module();
  }

  ~ProbeLT(){}

  /*-----------------------------------
   * b_transport method implementation
   *-----------------------------------*/
  void b_transport(tlm_generic_payload& trans, sc_time& delay) {
    if(trans.get_data_length() != this->last_request.get_data_length()){
      delete[] last_request_data;
      if(trans.get_data_length()>0){
        last_request_data = new BUSDATATYPE[trans.get_data_length()];
      } else {
        last_request_data = NULL;
      }
      this->last_request.set_data_ptr(reinterpret_cast<unsigned char*>(last_request_data));      
    }
    this->last_request.deep_copy_from(trans);
    initiatorSocket->b_transport(trans, delay);
    if(trans.get_data_length() != this->last_response.get_data_length()){
      delete[] last_response_data;
      if(trans.get_data_length()>0){
        last_response_data = new BUSDATATYPE[trans.get_data_length()];
      } else {
        last_response_data = NULL;
      }
      this->last_response.set_data_ptr(reinterpret_cast<unsigned char*>(last_response_data));      
    }
    this->last_response.deep_copy_from(trans);
  }

  /*-----------------------------------
   * transport_dbg method implementation
   *-----------------------------------*/
  unsigned int transport_dbg(tlm::tlm_generic_payload& trans) {
    unsigned int result;
    if(trans.get_data_length() != this->last_request.get_data_length()){
      delete[] last_request_data;
      if(trans.get_data_length()>0){
        last_request_data = new BUSDATATYPE[trans.get_data_length()];
      } else {
        last_request_data = NULL;
      }
      this->last_request.set_data_ptr(reinterpret_cast<unsigned char*>(last_request_data));      
    }
    this->last_request.deep_copy_from(trans);
    result = initiatorSocket->transport_dbg(trans);
    if(trans.get_data_length() != this->last_response.get_data_length()){
      delete[] last_response_data;
      if(trans.get_data_length()>0){
        last_response_data = new BUSDATATYPE[trans.get_data_length()];
      } else {
        last_response_data = NULL;
      }
      this->last_response.set_data_ptr(reinterpret_cast<unsigned char*>(last_response_data));      
    }
    this->last_response.deep_copy_from(trans);
    return result;
  }
};

#endif /* SIMPLEPROBE_HPP */
