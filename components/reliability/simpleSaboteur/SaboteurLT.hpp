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

using namespace tlm;


/*------------------------------------------------------------------------
 * Documentation on SaboteurLT: it is a simple saboteur to be inserted 
 * between a master module and a slave one. It receives requests from the 
 * master and forwards them to the connected slave; when activated, it 
 * mutates the transmitted payload according to the specified corruption. 
 *------------------------------------------------------------------------*/
template<typename BUSWIDTH> class SaboteurLT: public sc_module {
  
public:

  //enumerative types
  enum lineType {DATA, ADDRESS};
  enum maskFunctionType {VALUE_CHANGE, BIT_FLIP, BIT_FLIP0, BIT_FLIP1};

  //ports
  tlm_utils::simple_target_socket<SaboteurLT, sizeof(BUSWIDTH)*8>  targetSocket;
  tlm_utils::simple_initiator_socket<SaboteurLT, sizeof(BUSWIDTH)*8> initiatorSocket;

  //constructor
  SaboteurLT(sc_module_name module_name) : 
            initiatorSocket((boost::lexical_cast<std::string>(module_name) + "_initSocket").c_str()),
            targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSocket").c_str()){
    this->targetSocket.register_b_transport(this, &SaboteurLT::b_transport);
    
    end_module();
  }

  //destructor
  ~SaboteurLT(){}

private:

  //this type represents a descriptor for a single injection
  struct corruption_type{
    maskFunctionType maskFunction; //type of mask function
    unsigned int mask; //mask value
    unsigned int line; //line to be corrupted
    lineType linetype; //type of line (DATA or ADDRESS)
    
    //constructor
    corruption_type (maskFunctionType maskFunction, unsigned int mask, unsigned int line, lineType linetype)
    {
      this->maskFunction = maskFunction;
      this->mask = mask;
      this->line = line;
      this->linetype = linetype;
    }    
  };

  //used for storing all the masks used for injecting the fault at the next transaction
  std::vector<corruption_type> corruptions;


  //returns a corrupted value given a mask function and a mask value
  unsigned int corrupt(unsigned int value, unsigned int mask, maskFunctionType maskFunction)
  {
    unsigned int result;
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

  //add a mask to the list of masks used for the injection during the next transaction (not debug ones)
  void setMask(maskFunctionType maskFunction, unsigned int mask, unsigned int line, lineType linetype)
  {
    if(maskFunction!=BIT_FLIP && maskFunction!=BIT_FLIP0 && maskFunction!=BIT_FLIP1 && maskFunction!=VALUE_CHANGE)
      THROW_EXCEPTION("Sabouter received an unknown mask function ID");
    if(line != ADDRESS && line != DATA)
      THROW_EXCEPTION("Sabouters received an invalid line for the injection");
    
    this->corruptions.push_back(corruption_type(maskFunction, mask, line, linetype));
  }

  /*-----------------------------------
   * b_transport method implementation
   *-----------------------------------*/
  void b_transport(tlm_generic_payload& trans, sc_time& delay) {
    for(int i = 0; i < corruptions.size(); i++){
      if(corruptions[i].linetype == ADDRESS){ //corrupt the address
        //corruptions are supported only in the first 32 bits. 
        trans.set_address(this->corrupt(trans.get_address(), this->corruptions[i].mask, this->corruptions[i].maskFunction));
      } else if(corruptions[i].linetype == DATA && trans.is_write() == true){
        //corrupt the data. do note: we corrupt the data pointed by the payload. thus, we modify also the master data. 
        //indeed we assume that the master has copied the data in a temporary location referenced by the payload
        unsigned int numWords = trans.get_data_length()/sizeof(unsigned int);
        unsigned char* dataPtr = trans.get_data_ptr();
        if(numWords < corruptions[i].line){
          unsigned int datum;
          unsigned int currDatumLenght = 0;
          currDatumLenght = sizeof(unsigned int);
          memcpy(&datum, &dataPtr[corruptions[i].line * sizeof(unsigned int)], currDatumLenght);
          datum = this->corrupt(datum, this->corruptions[i].mask, this->corruptions[i].maskFunction);
          memcpy(&dataPtr[corruptions[i].line * sizeof(unsigned int)], &datum, currDatumLenght);
        }else if(numWords == corruptions[i].line) {
          unsigned int currDatumLenght = 0;
          currDatumLenght = trans.get_data_length() - numWords * sizeof(unsigned int);
          if(currDatumLenght == 1){
            unsigned char datum;
            memcpy(&datum, &dataPtr[corruptions[i].line * sizeof(unsigned int)], currDatumLenght);
            datum = this->corrupt(datum, this->corruptions[i].mask, this->corruptions[i].maskFunction);
            memcpy(&dataPtr[corruptions[i].line * sizeof(unsigned int)], &datum, currDatumLenght);          
          } else if(currDatumLenght == 2){
            unsigned short int datum;
            memcpy(&datum, &dataPtr[corruptions[i].line * sizeof(unsigned int)], currDatumLenght);
            datum = this->corrupt(datum, this->corruptions[i].mask, this->corruptions[i].maskFunction);
            memcpy(&dataPtr[corruptions[i].line * sizeof(unsigned int)], &datum, currDatumLenght);          
          }
        } else THROW_EXCEPTION("The index of line to be corrupted is not valid");
      }
    } 
    initiatorSocket->b_transport(trans, delay);

    for(int i = 0; i < corruptions.size(); i++){
      if(corruptions[i].linetype == DATA && trans.is_write() == true){
      
      }
    } 
  }
  
  /*-----------------------------------
   * transport_dbg method implementation
   *-----------------------------------*/
  unsigned int transport_dbg(tlm::tlm_generic_payload& trans) {
    //simply forward the request/response
    unsigned int result;
    result = initiatorSocket->transport_dbg(trans);
    return result;
  }
};

#endif /* SIMPLESABOTEUR_HPP */
