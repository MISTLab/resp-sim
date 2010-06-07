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
#include <fstream>

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
class ProbeLT: public sc_module {

  long int transactionId;
  static std::ofstream logfile;
  static std::string filename;
  static bool logEnable;
  
  //log the current transaction
  static void logCurrentTransaction(const char* probeName, tlm_generic_payload& pl, long int transactionId, bool isRequest){
    if(logEnable){
        if(!logfile.is_open()){
          logfile.open(filename.c_str());
          logfile << "time\tprobeName\ttransactionId\tdirection\ttype\tdataSize" << std::endl;
        }
        
        std::string direction;
        if(isRequest)
          direction = "request";
        else
          direction = "response";

        std::string type;
        if(pl.is_read())
          type = "read";
        else if(pl.is_write())
          type = "write";
        else
          type = "other";
          
        //TODO add other data; e.g. the address
        
        logfile << ((long)sc_time_stamp().to_default_time_units()) << "\t" << probeName << "\t" << transactionId << "\t" << direction << "\t" << type << "\t" << pl. get_data_length() << std::endl;
    }
  }
  

public:

  //at the end of the simulation the log file is closed
  void end_of_simulation(){
    if(ProbeLT::logEnable){
      if(ProbeLT::logfile.is_open())
        ProbeLT::logfile.close();
    }
  }

  //set the log file name
  static void setLogFileName(std::string name){
    if(ProbeLT::logfile.is_open())
      std::cerr << "It is not possible to set the log file name during the simulation" << std::endl;
    else
      ProbeLT::filename = name;
  }
  
  static void setLogEnable(bool enable){
    logEnable = enable;
  }

  tlm_utils::simple_target_socket<ProbeLT, 32>  targetSocket;
  tlm_utils::simple_initiator_socket<ProbeLT, 32> initiatorSocket;

  ProbeLT(sc_module_name module_name) : 
            initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_initSocket").c_str()),
            targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSocket").c_str()){
    this->targetSocket.register_b_transport(this, &ProbeLT::b_transport);
    this->targetSocket.register_transport_dbg(this, &ProbeLT::transport_dbg);

    this->transactionId = 0;

        
    end_module();
  }

  ~ProbeLT(){}

  /*-----------------------------------
   * b_transport method implementation
   *-----------------------------------*/
  void b_transport(tlm_generic_payload& trans, sc_time& delay) {
    logCurrentTransaction(this->name(), trans, this->transactionId, true);
    initiatorSocket->b_transport(trans, delay);
    logCurrentTransaction(this->name(), trans, this->transactionId, false);
    this->transactionId ++;
  }

  /*-----------------------------------
   * transport_dbg method implementation
   *-----------------------------------*/
  unsigned int transport_dbg(tlm::tlm_generic_payload& trans) {
    unsigned int result;
    logCurrentTransaction(this->name(), trans, this->transactionId, true);
    result = initiatorSocket->transport_dbg(trans);
    logCurrentTransaction(this->name(), trans, this->transactionId, false);
    this->transactionId ++;
    return result;
  }
};

#endif /* SIMPLEPROBE_HPP */
