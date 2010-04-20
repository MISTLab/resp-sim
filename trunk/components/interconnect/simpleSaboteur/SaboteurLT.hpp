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

#ifndef SIMPLESABOUTER_HPP
#define SIMPLESABOUTER_HPP

#include <string>
#include <queue>
#include <iostream>

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <boost/lexical_cast.hpp>
#include "utils.hpp"

#include <cxxabi.h>

using namespace tlm;


/*------------------------------------------------------------------------
 * Documentation on SaboteurLT: it is a simple saboteur to be inserted 
 * between a master module and a slave one. It receives requests from the 
 * master and forwards them to the connected slave; when activated, it 
 * mutates the transmitted payload according to the specified corruption. 
 *------------------------------------------------------------------------*/
template<typename BUSDATATYPE> class SaboteurLT: public sc_module {

  //they are necessary since the payload contains only a pointer. Thus, it is necessary to
  //assign to the pointer an actual position where to save transmitted data
  BUSDATATYPE last_request_data;
  BUSDATATYPE last_response_data;

public:

	enum lineType {DATA, ADDRESS };
	enum maskFunctionType {VALUE_CHANGE, BIT_FLIP, BIT_FLIP0, BIT_FLIP1};

  tlm_utils::simple_target_socket<SaboteurLT, sizeof(BUSDATATYPE)*8>  targetSocket;
  tlm_utils::simple_initiator_socket<SaboteurLT, sizeof(BUSDATATYPE)*8> initiatorSocket;

  tlm_generic_payload last_request;
  tlm_generic_payload last_response;

  SaboteurLT(sc_module_name module_name) : 
            initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_initSocket").c_str()),
            targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSocket").c_str()){
    this->targetSocket.register_b_transport(this, &SaboteurLT::b_transport);
    
  	this->checkDataType(); //check if the template data type is supported

    this->last_request.set_data_ptr(reinterpret_cast<unsigned char*>(&last_request_data));
    this->last_response.set_data_ptr(reinterpret_cast<unsigned char*>(&last_response_data));
    
    end_module();
  }

  ~SaboteurLT(){}

private:
	//it represents a mask for the injection
	struct corruption_type{
		maskFunctionType faultMask;
		BUSDATATYPE mask;
		lineType line;
		corruption_type (maskFunctionType faultMask, BUSDATATYPE mask, lineType line)
		{
			this->faultMask = faultMask;
			this->mask = mask;
			this->line = line;
		}		
	};

  //used for storing all the masks used for injecting the fault at the next transaction
  std::vector<corruption_type> corruptions;

  //used for checking if the actual type of the template is compatible with the injection features
  void checkDataType()
  {
		const std::type_info & dataType_t = typeid(BUSDATATYPE);
		const std::type_info & int_t = typeid(int);
 		const std::type_info & uint_t = typeid(unsigned int);
		const std::type_info & lint_t = typeid(long int);
		const std::type_info & sint_t = typeid(short int);
		const std::type_info & usint_t = typeid(unsigned short int);
		const std::type_info & ulint_t = typeid(unsigned long int);
		const std::type_info & char_t = typeid(char);
     	
   	if (! (strcmp(dataType_t.name(),int_t.name())==0 ||
   		strcmp(dataType_t.name(),uint_t.name())==0 ||
   		strcmp(dataType_t.name(),lint_t.name())==0 ||
   		strcmp(dataType_t.name(),sint_t.name())==0 ||
   		strcmp(dataType_t.name(),usint_t.name())==0 ||
   		strcmp(dataType_t.name(),ulint_t.name())==0 ||
   		strcmp(dataType_t.name(),char_t.name())==0 )) 
 		{
     	int status;
     	const char * realname = abi::__cxa_demangle(dataType_t.name(), 0, 0, &status);
     	THROW_EXCEPTION("Sabouters do not support injection in " << realname << " datatype");
		}
  }
         
  //returns a corrupted value given a mask
  BUSDATATYPE corrupt(BUSDATATYPE value, BUSDATATYPE mask, maskFunctionType maskFunction)
  {
  	BUSDATATYPE result;
  	this->checkDataType();
	
  	if(maskFunction == BIT_FLIP)
		result = value^mask;
  	else if (maskFunction == BIT_FLIP0)
		result = ((value & ~mask)|(mask & 0));
  	else if (maskFunction == BIT_FLIP1)
  		result = ((value & ~mask)|(mask & 1));
	  else if (maskFunction == VALUE_CHANGE)
  		result = mask;
  	else
  		THROW_EXCEPTION("Sabouter received an unknown mask function ID");
	  return result;     	
  }

public:

  //add a mask to the list of masks used for the injection during the next transaction
  void setMask(maskFunctionType maskFunction, BUSDATATYPE mask, lineType line)
  {
  	if(maskFunction!=BIT_FLIP && maskFunction!=BIT_FLIP0 && maskFunction!=BIT_FLIP1 && maskFunction!=VALUE_CHANGE)
  		THROW_EXCEPTION("Sabouter received an unknown mask function ID");
  	if(line != ADDRESS && line != DATA)
  		THROW_EXCEPTION("Sabouters received an invalid line for the injection");
  	
  	this->corruptions.push_back(corruption_type(maskFunction, mask, line));
  }


  /*-----------------------------------
   * b_transport method implementation
   *-----------------------------------*/
  void b_transport(tlm_generic_payload& trans, sc_time& delay) {
    this->last_request.deep_copy_from(trans);
    initiatorSocket->b_transport(trans, delay);
    this->last_response.deep_copy_from(trans);
  }
  
  /*-----------------------------------
   * transport_dbg method implementation
   *-----------------------------------*/
  unsigned int transport_dbg(tlm::tlm_generic_payload& trans) {
    //simply forward the request/response and log the payload before 
    //transmitting to the target and returning to the initiator
    unsigned int result;
    this->last_request.deep_copy_from(trans);
    result = initiatorSocket->transport_dbg(trans);
    this->last_response.deep_copy_from(trans);    
    return result;
  }

};

#endif /* SIMPLESABOTEUR_HPP */
